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
	// The class object containing meta-information about this skill
	class Class final : public skill::Skill::Class
	{
	public:
		///////////////////////////////////////////////////////
		// Virtual overrides for skill::Skill::Class

		auto name() const -> std::string_view final
		{
			// This is the name of the service provider, as it appears in the model.json file
			return "SimpleSampleMicroservice"sv;
		}

		auto uuid() const -> utils::core::Uuid final
		{
			// This is an arbitrary unique UUID for the service provider. This can be anything, but should never change.
			return "eb21b0b0-406e-41dd-b5a6-fdf4ff17e974"_uuid;
		}

		auto registerElements(Registry &registry) -> void final
		{
			// Register the object class of the microservice
			registry << Instance::Class::instance();
		}

		auto createSkill() -> std::unique_ptr<skill::Skill> final
		{
			return std::make_unique<Skill>();
		}
	};

	///////////////////////////////////////////////////////
	// Virtual overrides for skill::Skill

	auto createElement(const skill::Element::Class &elementClass, skill::ElementFactory &factory)
		-> std::shared_ptr<skill::Element> final;
};

} // namespace xentara::samples::simpleMicroservice