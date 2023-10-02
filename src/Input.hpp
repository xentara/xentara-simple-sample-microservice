// Copyright (c) embedded ocean GmbH
#pragma once

#include <xentara/config/Resolver.hpp>
#include <xentara/data/ReadHandle.hpp>
#include <xentara/model/Element.hpp>
#include <xentara/utils/json/decoder/Value.hpp>

#include <memory>
#include <string>
#include <string_view>

namespace xentara::samples::simpleMicroservice
{

// A single input of the microservice
class Input final
{
public:
	// Loads the input from a configuration value
	auto load(utils::json::decoder::Value &value, config::Resolver &resolver) -> void;

	// Prepares the input
	auto prepare() -> void;

	// Reads the value as a certain type. Throws an exception on error.
	template <typename Type>
	auto read() -> Type;

private:
	// Gets a read handle
	auto readHandle(const model::Element &element, std::string_view attributeName) -> data::ReadHandle;

	// Gets a name for the element for use in error messages
	auto elementName() const -> std::string;
	
	// Checks the quality. Throws an exception on error.
	auto checkQuality() -> void;
	// Handles a read error. Always throws an exception.
	[[noreturn]] auto handleReadError(std::error_code error) -> void;

	// The element
	std::weak_ptr<model::Element> _element;

	// The read handle for the quality
	data::ReadHandle _quality;
	// The read handle for the value
	data::ReadHandle _value;
};

template <typename Type>
auto Input::read() -> Type
{
	// Check the quality first
	checkQuality();

	// Try to read the value
	const auto value = _value.read<Type>();
	// Check it
	if (!value)
	{
		handleReadError(value.error());
	}

	return std::move(*value);
}

} // namespace xentara::samples::simpleMicroservice