// Copyright (c) embedded ocean GmbH
#pragma once

#include <xentara/config/Resolver.hpp>
#include <xentara/data/WriteHandle.hpp>
#include <xentara/model/GenericElement.hpp>
#include <xentara/utils/json/decoder/Value.hpp>

#include <memory>
#include <string>
#include <new>

namespace xentara::plugins::sampleMicroservice
{

// A single output of the microservice
class Output
{
public:
	// Loads the output from a configuration value
	auto loadConfig(utils::json::decoder::Value &value, config::Resolver &resolver) -> void;

	// Prepares the output
	auto prepare() -> void;

	// Writes the value as a certain type. Throws an exception on error.
	template <typename Type>
	auto write(const Type &value) -> void;

	// Writes the value as a certain type without throwing any errors.
	template <typename Type>
	auto write(const Type &value, std::nothrow_t) -> std::error_code;

private:
	// Gets a write handle
	auto writeHandle(model::GenericElement &element, std::string_view attributeName) -> data::WriteHandle;

	// Gets a name for the element for use in error messages
	auto elementName() const -> std::string;
	
	// Handles a write error. Always throws an exception.
	[[noreturn]] auto handleWriteError(std::error_code error) -> void;

	// The element
	std::weak_ptr<model::GenericElement> _element;

	// The write handle for the value
	data::WriteHandle _value;
};

template <typename Type>
auto Output::write(const Type &value) -> void
{
	// Try to read the value
	const auto error = _value.write<Type>(value);
	// Handles errors
	if (error)
	{
		handleWriteError(error);
	}
}

template <typename Type>
auto Output::write(const Type &value, std::nothrow_t) -> std::error_code
{
	return _value.write<Type>(value);
}

} // namespace xentara::plugins::sampleMicroservice