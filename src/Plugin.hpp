// Copyright (c) embedded ocean GmbH
#pragma once

#include "Skill.hpp"

#include <xentara/plugin/Plugin.hpp>

namespace xentara::samples::simpleMicroservice
{

// The class that registers the library as a Xentara plugin.
class Plugin final : plugin::Plugin
{
public:
	///////////////////////////////////////////////////////
	// Virtual overrides for plugin::Plugin

	auto registerSkills(Registry & registry) -> void final
	{
		// Register the skill class.
		registry << _skillClass;
	}
	
private:
	// The skill class
	Skill::Class _skillClass;

	// The global plugin object
	static Plugin _instance;
};

} // namespace xentara::samples::simpleMicroservice
