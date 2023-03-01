// Copyright (c) embedded ocean GmbH
#pragma once

#include "SampleMicroservice.hpp"

#include <xentara/process/ServiceProvider.hpp>
#include <xentara/utils/core/Uuid.hpp>

#include <string_view>

namespace xentara::plugins::sampleMicroservice
{

using namespace std::literals;

// The service provider class. This class registers all the elements the service provider provides,
// and creates the service provider runtime environment.
class ServiceProvider final : public process::ServiceProvider
{
public:
	///////////////////////////////////////////////////////
	// Virtual overrides for process::ServiceProvider

	auto name() const -> std::string_view final
	{
		// This is the name of the service provider, as it appears in the model.json file
		return "Sample"sv;
	}

	auto uuid() const -> utils::core::Uuid final
	{
		// This is an arbitrary unique UUID for the service provider. This can be anything, but should never change.
		return "eb21b0b0-406e-41dd-b5a6-fdf4ff17e974"_uuid;
	}

	auto registerObjects(Registry &registry) -> void final
	{
		// Register the object class of the microservice
		registry << SampleMicroservice::Class::instance();
	}

	auto createEnvironment() -> std::unique_ptr<process::ServiceProvider::Environment> final;

private:
	// The service provider runtime environment
	class Environment;
};

} // namespace xentara::plugins::sampleMicroservice