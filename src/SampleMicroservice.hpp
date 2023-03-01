// Copyright (c) embedded ocean GmbH
#pragma once

#include "Attributes.hpp"
#include "Input.hpp"
#include "Output.hpp"

#include <xentara/memory/Array.hpp>
#include <xentara/plugin/EnableSharedFromThis.hpp>
#include <xentara/process/Event.hpp>
#include <xentara/process/Microservice.hpp>
#include <xentara/process/MicroserviceClass.hpp>
#include <xentara/process/Task.hpp>
#include <xentara/utils/core/Uuid.hpp>

#include <functional>
#include <string>
#include <string_view>
#include <optional>
#include <new>

namespace xentara::plugins::sampleMicroservice
{

using namespace std::literals;

class TemplateClient;

// A class representing a sample microservice.
class SampleMicroservice final : public process::Microservice, public plugin::EnableSharedFromThis<SampleMicroservice>
{
public:
	// The class object containing meta-information about this element type
	class Class final : public process::MicroserviceClass
	{
	public:
		// Gets the global object
		static auto instance() -> Class &
		{
			return _instance;
		}

		///////////////////////////////////////////////////////
		// Virtual overrides for process::MicroserviceClass

		auto name() const -> std::string_view final
		{
			// This is the name of the microservice class, as it appears in the model.json file
			return "Microservice"sv;
		}

		auto uuid() const -> utils::core::Uuid final
		{
			// This is an arbitrary unique UUID for the microservice class. This can be anything, but should never
			// change.
			return "fc5da59c-028f-49d0-b73d-38637957d376"_uuid;
		}

	private:
		// The global object that represents the class
		static Class _instance;
	};

	///////////////////////////////////////////////////////
	// Virtual overrides for process::Microservice

	auto resolveAttribute(std::string_view name) -> const model::Attribute * final;

	auto resolveTask(std::string_view name) -> std::shared_ptr<process::Task> final;

	auto resolveEvent(std::string_view name) -> std::shared_ptr<process::Event> final;

	auto readHandle(const model::Attribute &attribute) const noexcept -> data::ReadHandle final;

	auto realize() -> void final;

	auto prepare() -> void final;

protected:
	///////////////////////////////////////////////////////
	// Virtual overrides for process::Microservice

	auto loadConfig(const ConfigIntializer &initializer,
		utils::json::decoder::Object &jsonObject,
		config::Resolver &resolver,
		const FallbackConfigHandler &fallbackHandler) -> void final;

private:
	// The error message for a microservice that has not been executed yet
	static const std::string_view kPendingError;
	// The error message for a microservice that is suspended
	static const std::string_view kSuspendedError;

	// This structure represents the current state of the microservice
	struct State final
	{
		// Whether the microservice is being executed correctly
		bool _executionState { false };
		// The last time the microservice was executed (successfully or not)
		std::chrono::system_clock::time_point _executionTime { std::chrono::system_clock::time_point::min() };
		// The error message, or an empty string for none.
		std::string _error { kPendingError };
	};

	// This class provides callbacks for the Xentara scheduler for the "execute" task
	class ExecuteTask final : public process::Task
	{
	public:
		// This constuctor attached the task to its target
		ExecuteTask(std::reference_wrapper<SampleMicroservice> target) : _target(target)
		{
		}

		///////////////////////////////////////////////////////
		// Virtual overrides for process::Task

		auto stages() const -> Stages final
		{
			return Stage::PreOperational | Stage::Operational | Stage::PostOperational;
		}

		auto preparePreOperational(const process::ExecutionContext &context) -> Status final;

		auto operational(const process::ExecutionContext &context) -> void final;

		auto preparePostOperational(const process::ExecutionContext &context) -> Status final;

	private:
		// A reference to the microservice
		std::reference_wrapper<SampleMicroservice> _target;
	};

	// This function is called by the "execute" task on startup.
	auto prePerformExecuteTask(const process::ExecutionContext &context) -> void;
	// This function is called by the "execute" task.
	auto performExecuteTask(const process::ExecutionContext &context) -> void;
	// This function is called by the "execute" task on shutdown.
	auto postPerformExecuteTask(const process::ExecutionContext &context) -> void;

	// Executes the microservice. Throws an exception on error
	auto execute(std::chrono::system_clock::time_point timeStamp) -> void;
	// Safes the state. Returns an error on error.
	auto safe(std::chrono::system_clock::time_point timeStamp) -> std::error_code;

	// Updates the state
	auto updateState(std::chrono::system_clock::time_point timeStamp, std::optional<std::string_view> error = std::nullopt) -> void;

	// A Xentara event that is fired when the microservice has been executed successfully
	process::Event _executedEvent;
	// A Xentara event that is fired when an error occurred
	process::Event _errorEvent;

	// The "execute" task
	ExecuteTask _executeTask { *this };

	// The data block that contains the state
	memory::ObjectBlock<State> _stateDataBlock;

	///////////////////////////////////////////////////////
	// Input and outputs of the microservice

	// Some random inputs
	Input _left;
	Input _right;

	// Some random outputs
	Output _setpoint;
	Output _safe;
};

} // namespace xentara::plugins::sampleMicroservice