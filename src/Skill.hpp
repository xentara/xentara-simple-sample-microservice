// Copyright (c) embedded ocean GmbH
#pragma once

#include "Instance.hpp"

#include <xentara/skill/Skill.hpp>
#include <xentara/utils/core/Uuid.hpp>

#include <string_view>

namespace xentara::samples::simpleMicroservice
{

using namespace std::literals;

// The skill
class Skill final : public skill::Skill
{
public:
	///////////////////////////////////////////////////////
	// Virtual overrides for skill::Skill

	auto createElement(const skill::Element::Class &elementClass, skill::ElementFactory &factory)
		-> std::shared_ptr<skill::Element> final;

private:
	// The skill class
	using Class = ConcreteClass<Skill,
		"SimpleSampleMicroservice",
		"eb21b0b0-406e-41dd-b5a6-fdf4ff17e974"_uuid,
		Instance::Class>;

	// The skill class object
	static Class _class;
};

} // namespace xentara::samples::simpleMicroservice