// Copyright (c) embedded ocean GmbH
#pragma once

#include "Attributes.hpp"
#include "Input.hpp"
#include "Output.hpp"

#include <xentara/memory/Array.hpp>
#include <xentara/model/ElementCategory.hpp>
#include <xentara/process/Event.hpp>
#include <xentara/process/Task.hpp>
#include <xentara/skill/Element.hpp>
#include <xentara/skill/EnableSharedFromThis.hpp>
#include <xentara/utils/core/Uuid.hpp>

#include <functional>
#include <string>
#include <string_view>
#include <optional>
#include <new>

namespace xentara::samples::simpleMicroservice
{

using namespace std::literals;

class TemplateClient;

// A class representing a sample microservice.
class Instance final : public skill::Element, public skill::EnableSharedFromThis<Instance>
{
public:
	// The class object containing meta-information about this element type
	using Class = ConcreteClass<
		// This is the name of the microservice class, as it appears in the model.json file
		"Instance",
		// This is an arbitrary unique UUID for the microservice class. This can be anything, but should never
		// change.
		"fc5da59c-028f-49d0-b73d-38637957d376"_uuid,
		// This is a human readable name for the element class.
		// NOTE: The display name must be understandable event without knowing the skill it belongs to.
		"simple sample microservice">;

	///////////////////////////////////////////////////////
	// Virtual overrides for skill::Element

	auto forEachAttribute(const model::ForEachAttributeFunction &function) const -> bool final;

	auto forEachEvent(const model::ForEachEventFunction &function) -> bool final;

	auto forEachTask(const model::ForEachTaskFunction &function) -> bool final;

	auto makeReadHandle(const model::Attribute &attribute) const noexcept -> std::optional<data::ReadHandle> final;

	auto category() const noexcept -> model::ElementCategory final
	{
		return model::ElementCategory::Microservice;
	}

private:
	// The error message for a microservice that has not been executed yet
	static const std::string_view kPendingError;
	// The error message for a microservice that is in the process of being suspended
	static const std::string_view kSuspendingError;
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
		ExecuteTask(std::reference_wrapper<Instance> target) : _target(target)
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
		std::reference_wrapper<Instance> _target;
	};

	// This function is called by the "execute" task on startup.
	auto prePerformExecuteTask(const process::ExecutionContext &context) -> void;
	// This function is called by the "execute" task.
	auto performExecuteTask(const process::ExecutionContext &context) -> void;
	// This function is called by the "execute" task on shutdown.
	auto postPerformExecuteTask(const process::ExecutionContext &context) -> void;
	// This function determines if the shutdown or suspend of the "execute" task has completed.
	auto checkPostPerformExecuteTask(const process::ExecutionContext &context) -> process::Task::Status;

	// Executes the microservice. Throws an exception on error
	auto execute(std::chrono::system_clock::time_point timeStamp) -> void;
	// Safes the state. Returns an error on error.
	auto safe(std::chrono::system_clock::time_point timeStamp) -> std::error_code;

	// Checks whether the state is safe
	auto isSafe() -> bool;

	// Updates the state
	auto updateState(std::chrono::system_clock::time_point timeStamp, std::optional<std::string_view> error = std::nullopt) -> void;

	///////////////////////////////////////////////////////
	// Virtual overrides for skill::Element

	auto load(utils::json::decoder::Object &jsonObject, config::Context &context) -> void final;

	auto realize() -> void final;

	auto prepare() -> void final;

	// A Xentara event that is raised when the microservice has been executed successfully
	process::Event _executedEvent;
	// A Xentara event that is raised when an error occurred executing the microservice
	process::Event _executionErrorEvent;

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

	// The input that tells us whether we are safe
	Input _isSafe;
};

} // namespace xentara::samples::simpleMicroservice