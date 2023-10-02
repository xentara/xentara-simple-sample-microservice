// Copyright (c) embedded ocean GmbH
#include "Output.hpp"

#include <xentara/data/Quality.hpp>
#include <xentara/model/Attribute.hpp>
#include <xentara/utils/string/cat.hpp>

#include <functional>
#include <stdexcept>

using namespace std::literals;

namespace xentara::samples::simpleMicroservice
{

auto Output::load(utils::json::decoder::Value &value, config::Resolver &resolver) -> void
{
	// Just submit a request
	resolver.submit<model::Element>(value, std::ref(_element));
}

auto Output::prepare() -> void
{
	// Make sure the element was actually loaded
	auto element = _element.lock();
	if (!element)
	{
		return;
	}

	// Resolve the handle
	_value = writeHandle(*element, model::Attribute::kValue.name());

	// Make sure the value is not read only
	if (_value == data::WriteHandle::Error::ReadOnly)
	{
		throw std::runtime_error(utils::string::cat("the value of element \"", element->primaryKey(), "\" is read only"));
	}
}

auto Output::writeHandle(model::Element &element, std::string_view attributeName) -> data::WriteHandle
{
	// Get the handle
	auto handle = element.attributeWriteHandle(attributeName);
	// Check that it exists
	if (handle == data::WriteHandle::Error::Unknown)
	{
		throw std::runtime_error(utils::string::cat("the element \"", element.primaryKey(), "\" does not have an attribute named \"", attributeName, '"'));
	}

	return handle;
}

auto Output::elementName() const -> std::string
{
	// Get the element
	auto element = _element.lock();
	if (!element)
	{
		return "output"s;
	}

	return element->primaryKey();
}

auto Output::handleWriteError(std::error_code error) -> void
{
	throw std::system_error(error, utils::string::cat("could not write ", elementName()));
}

} // namespace xentara::samples::simpleMicroservice