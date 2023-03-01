// Copyright (c) embedded ocean GmbH
#pragma once

#include "ServiceProvider.hpp"

#include <xentara/plugin/Plugin.hpp>

namespace xentara::plugins::sampleMicroservice
{

// The class that registers the library as a Xentara plugin.
class Plugin final : plugin::Plugin
{
public:
	///////////////////////////////////////////////////////
	// Virtual overrides for plugin::Plugin

	auto registerObjects(Registry & registry) -> void final
	{
		// Register the service provider object.
		registry << _serviceProvider;
	}
	
private:
	// The service provider object
	ServiceProvider _serviceProvider;

	// The global plugin object
	static Plugin _instance;
};

} // namespace xentara::plugins::sampleMicroservice
