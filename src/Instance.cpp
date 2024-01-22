// Copyright (c) embedded ocean GmbH
#include "Instance.hpp"

#include "Attributes.hpp"
#include "Events.hpp"
#include "Tasks.hpp"

#include <xentara/config/FallbackHandler.hpp>
#include <xentara/data/DataType.hpp>
#include <xentara/data/ReadHandle.hpp>
#include <xentara/data/WriteHandle.hpp>
#include <xentara/memory/memoryResources.hpp>
#include <xentara/memory/WriteSentinel.hpp>
#include <xentara/model/Attribute.hpp>
#include <xentara/model/ForEachAttributeFunction.hpp>
#include <xentara/model/ForEachEventFunction.hpp>
#include <xentara/model/ForEachTaskFunction.hpp>
#include <xentara/process/EventList.hpp>
#include <xentara/process/ExecutionContext.hpp>
#include <xentara/utils/json/decoder/Object.hpp>
#include <xentara/utils/json/decoder/Errors.hpp>
#include <xentara/utils/eh/currentErrorCode.hpp>

#include <algorithm>
#include <concepts>
#include <format>

namespace xentara::samples::simpleMicroservice
{
	
using namespace std::literals;

const std::string_view Instance::kPendingError = "the microservice instance has not been executed yet"sv;
const std::string_view Instance::kSuspendedError = "the microservice instance is suspended"sv;

auto Instance::load(utils::json::decoder::Object &jsonObject,
	config::Resolver &resolver,
	const config::FallbackHandler &fallbackHandler) -> void
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
			_left.load(value, resolver);
			leftLoaded = true;
		}
		else if (name == "right")
		{
			_right.load(value, resolver);
			rightLoaded = true;
		}
		else if (name == "setpoint")
		{
			_setpoint.load(value, resolver);
			setpointLoaded = true;
		}
		else if (name == "safe")
		{
			_safe.load(value, resolver);
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
		utils::json::decoder::throwWithLocation(jsonObject, std::runtime_error("no left input specified for simple sample microservice instance"));
	}
	if (!rightLoaded)
	{
		utils::json::decoder::throwWithLocation(jsonObject, std::runtime_error("no right input specified for simple sample microservice instance"));
	}
	if (!setpointLoaded)
	{
		utils::json::decoder::throwWithLocation(jsonObject, std::runtime_error("no setpoint output specified for simple sample microservice instance"));
	}
	if (!safeLoaded)
	{
		utils::json::decoder::throwWithLocation(jsonObject, std::runtime_error("no safe output specified for simple sample microservice instance"));
	}
}

auto Instance::performExecuteTask(const process::ExecutionContext &context) -> void
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

auto Instance::prePerformExecuteTask(const process::ExecutionContext &context) -> void
{
	// We are now pending
	updateState(context.scheduledTime(), kPendingError);
}

auto Instance::postPerformExecuteTask(const process::ExecutionContext &context) -> void
{
	// Get the time stamp
	const auto timeStamp = context.scheduledTime();

	// safe the state
	const auto error = safe(timeStamp);
	// Check for errors
	if (error)
	{
		updateState(timeStamp, std::format("could not save state: {}", error.message()));
		return;
	}

	// We are now suspended
	updateState(timeStamp, kSuspendedError);
}

auto Instance::execute(std::chrono::system_clock::time_point timeStamp) -> void
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

auto Instance::safe(std::chrono::system_clock::time_point timeStamp) -> std::error_code
{
	// Set the safe state
	return _safe.write(true, std::nothrow);
}

auto Instance::updateState(
	std::chrono::system_clock::time_point timeStamp, std::optional<std::string_view> error) -> void
{
	// Make a write sentinel
	memory::WriteSentinel sentinel { _stateDataBlock };
	auto &state = *sentinel;

	// Update the state
	state._executionState = !error;
	state._executionTime = timeStamp;
	state._error = std::string(error.value_or(""sv));

	// Determine the correct event
	const auto &event = error ? _executionErrorEvent : _executedEvent;
	// Commit the data and raise the event
	sentinel.commit(timeStamp, event);
}

auto Instance::forEachAttribute(const model::ForEachAttributeFunction &function) const -> bool
{
	// Handle all the attributes we support
	return
		function(attributes::kExecutionState) ||
		function(attributes::kExecutionTime) ||
		function(attributes::kError);
}

auto Instance::forEachEvent(const model::ForEachEventFunction &function) -> bool
{
	// Handle all the events we support
	return
		function(events::kExecuted, sharedFromThis(&_executedEvent)) ||
		function(events::kExecutionError, sharedFromThis(&_executionErrorEvent));
}

auto Instance::forEachTask(const model::ForEachTaskFunction &function) -> bool
{
	// We only have the "execute" task
	return function(tasks::kExecute, sharedFromThis(&_executeTask));
}

auto Instance::makeReadHandle(const model::Attribute &attribute) const noexcept -> std::optional<data::ReadHandle>
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

	return std::nullopt;
}

auto Instance::realize() -> void
{
	// Create the data block
	_stateDataBlock.create(memory::memoryResources::data());
}

auto Instance::prepare() -> void
{
	// Prepare all the inputs and outputs
	_left.prepare();
	_right.prepare();
	_setpoint.prepare();
	_safe.prepare();
}

auto Instance::ExecuteTask::preparePreOperational(const process::ExecutionContext &context) -> Status
{
	_target.get().prePerformExecuteTask(context);
	return Status::Completed;
}

auto Instance::ExecuteTask::operational(const process::ExecutionContext &context) -> void
{
	_target.get().performExecuteTask(context);
}

auto Instance::ExecuteTask::preparePostOperational(const process::ExecutionContext &context) -> Status
{
	_target.get().postPerformExecuteTask(context);
	return Status::Completed;
}

} // namespace xentara::samples::simpleMicroservice