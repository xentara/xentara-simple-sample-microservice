// Copyright (c) embedded ocean GmbH
#include "SampleMicroservice.hpp"

#include "Attributes.hpp"

#include <xentara/data/DataType.hpp>
#include <xentara/data/ReadHandle.hpp>
#include <xentara/data/WriteHandle.hpp>
#include <xentara/memory/memoryResources.hpp>
#include <xentara/memory/WriteSentinel.hpp>
#include <xentara/model/Attribute.hpp>
#include <xentara/process/ExecutionContext.hpp>
#include <xentara/utils/json/decoder/Object.hpp>
#include <xentara/utils/json/decoder/Errors.hpp>
#include <xentara/utils/eh/currentErrorCode.hpp>

#include <concepts>
#include <algorithm>

namespace xentara::plugins::sampleMicroservice
{
	
using namespace std::literals;

SampleMicroservice::Class SampleMicroservice::Class::_instance;

const std::string_view SampleMicroservice::kPendingError = "the microservice has not been executed yet"sv;
const std::string_view SampleMicroservice::kSuspendedError = "the microservice is suspended"sv;

auto SampleMicroservice::loadConfig(const ConfigIntializer &initializer,
		utils::json::decoder::Object &jsonObject,
		config::Resolver &resolver,
		const FallbackConfigHandler &fallbackHandler) -> void
{
	// Keep track of which inpouts/outputs have been loaded
	bool leftLoaded = false;
	bool rightLoaded = false;
	bool setpointLoaded = false;
	bool safeLoaded = false;

	// Go through all the members of the JSON object that represents this object
	for (auto && [name, value] : jsonObject)
    {
		if (name == "left")
		{
			_left.loadConfig(value, resolver);
			leftLoaded = true;
		}
		else if (name == "right")
		{
			_right.loadConfig(value, resolver);
			rightLoaded = true;
		}
		else if (name == "setpoint")
		{
			_setpoint.loadConfig(value, resolver);
			setpointLoaded = true;
		}
		else if (name == "safe")
		{
			_safe.loadConfig(value, resolver);
			safeLoaded = true;
		}
		else
		{
			// Pass any unknown parameters on to the fallback handler, which will load the built-in parameters ("id", "uuid", and "children"),
			// and throw an exception if the key is unknown
            fallbackHandler(name, value);
		}
    }

	// Check that ell inputs have been loaded
	if (!leftLoaded)
	{
		utils::json::decoder::throwWithLocation(jsonObject, std::runtime_error("no left input specified for sample microservice"));
	}
	if (!rightLoaded)
	{
		utils::json::decoder::throwWithLocation(jsonObject, std::runtime_error("no right input specified for sample microservice"));
	}
	if (!setpointLoaded)
	{
		utils::json::decoder::throwWithLocation(jsonObject, std::runtime_error("no setpoint output specified for sample microservice"));
	}
	if (!safeLoaded)
	{
		utils::json::decoder::throwWithLocation(jsonObject, std::runtime_error("no safe output specified for sample microservice"));
	}
}

auto SampleMicroservice::performExecuteTask(const process::ExecutionContext &context) -> void
{
	// Get the time stamp
	const auto timeStamp = context.scheduledTime();

	try
	{
		// execute the task
		execute(timeStamp);
		// The execution was successful
		updateState(timeStamp);
	}
	catch (const std::exception &exception)
	{
		// Update the state
		updateState(timeStamp, exception.what());
	}
}

auto SampleMicroservice::prePerformExecuteTask(const process::ExecutionContext &context) -> void
{
	// We are now pending
	updateState(context.scheduledTime(), kPendingError);
}

auto SampleMicroservice::postPerformExecuteTask(const process::ExecutionContext &context) -> void
{
	// Get the time stamp
	const auto timeStamp = context.scheduledTime();

	// safe the state
	const auto error = safe(timeStamp);
	// Check for errors
	if (error)
	{
		updateState(timeStamp, utils::string::cat("could not safe state: ", error.message()));
		return;
	}

	// We are now suspended
	updateState(timeStamp, kSuspendedError);
}

auto SampleMicroservice::execute(std::chrono::system_clock::time_point timeStamp) -> void
{
	try
	{
		// Read the inputs
		const auto left = _left.read<double>();
		const auto right = _right.read<double>();

		// Use the maximum as the set point
		_setpoint.write(std::max(left, right));
		// Remove the safety
		_safe.write(false);
	}
	catch (...)
	{
		// Try to safe the state, but ignore any further errors
		safe(timeStamp);

		throw;
	}
}

auto SampleMicroservice::safe(std::chrono::system_clock::time_point timeStamp) -> std::error_code
{
	// Set the safe state
	return _safe.write(true, std::nothrow);
}

auto SampleMicroservice::updateState(
	std::chrono::system_clock::time_point timeStamp, std::optional<std::string_view> error) -> void
{
	// Make a write sentinel
	memory::WriteSentinel sentinel { _stateDataBlock };
	auto &state = *sentinel;

	// Update the state
	state._executionState = !error;
	state._executionTime = timeStamp;
	state._error = std::string(error.value_or(""sv));

	// Commit the data
	sentinel.commit();

	// Fire the correct event
	if (error)
	{
		_errorEvent.fire();
	}
	else
	{
		_executedEvent.fire();
	}
}

auto SampleMicroservice::resolveAttribute(std::string_view name) -> const model::Attribute *
{
	return model::Attribute::resolve(name,
		attributes::kExecutionState,
		attributes::kExecutionTime,
		attributes::kError);
}

auto SampleMicroservice::resolveTask(std::string_view name) -> std::shared_ptr<process::Task>
{
	if (name == "execute"sv)
	{
		return std::shared_ptr<process::Task>(sharedFromThis(), &_executeTask);
	}

	return nullptr;
}

auto SampleMicroservice::resolveEvent(std::string_view name) -> std::shared_ptr<process::Event>
{
	// Check all the events we support
	if (name == "executed"sv)
	{
		return std::shared_ptr<process::Event>(sharedFromThis(), &_executedEvent);
	}
	else if (name == "error"sv)
	{
		return std::shared_ptr<process::Event>(sharedFromThis(), &_errorEvent);
	}

	return nullptr;
}

auto SampleMicroservice::readHandle(const model::Attribute &attribute) const noexcept -> data::ReadHandle
{
	// Try our attributes
	if (attribute == attributes::kExecutionState)
	{
		return _stateDataBlock.member(&State::_executionState);
	}
	else if (attribute == attributes::kExecutionTime)
	{
		return _stateDataBlock.member(&State::_executionTime);
	}
	else if (attribute == attributes::kError)
	{
		return _stateDataBlock.member(&State::_error);
	}

	return data::ReadHandle::Error::Unknown;
}

auto SampleMicroservice::realize() -> void
{
	// Create the data block
	_stateDataBlock.create(memory::memoryResources::data());
}

auto SampleMicroservice::prepare() -> void
{
	// Prepare all the inputs and outputs
	_left.prepare();
	_right.prepare();
	_setpoint.prepare();
	_safe.prepare();
}

auto SampleMicroservice::ExecuteTask::preparePreOperational(const process::ExecutionContext &context) -> Status
{
	_target.get().prePerformExecuteTask(context);
	return Status::Completed;
}

auto SampleMicroservice::ExecuteTask::operational(const process::ExecutionContext &context) -> void
{
	_target.get().performExecuteTask(context);
}

auto SampleMicroservice::ExecuteTask::preparePostOperational(const process::ExecutionContext &context) -> Status
{
	_target.get().postPerformExecuteTask(context);
	return Status::Completed;
}

} // namespace xentara::plugins::sampleMicroservice