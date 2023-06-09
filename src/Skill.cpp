// Copyright (c) embedded ocean GmbH
#include "Skill.hpp"

#include <xentara/skill/Element.hpp>
#include <xentara/skill/ElementFactory.hpp>

namespace xentara::samples::simpleMicroservice
{

auto Skill::createElement(const skill::Element::Class &elementClass, skill::ElementFactory &factory)
	-> std::shared_ptr<skill::Element>
{
	if (&elementClass == &SampleMicroservice::Class::instance())
	{
		return factory.makeShared<SampleMicroservice>();
	}

	return nullptr;
}

} // namespace xentara::samples::simpleMicroservice