// Copyright (c) embedded ocean GmbH
#include "ServiceProvider.hpp"

#include <xentara/plugin/SharedFactory.hpp>
#include <xentara/process/Microservice.hpp>

namespace xentara::plugins::sampleMicroservice
{

class ServiceProvider::Environment : public process::ServiceProvider::Environment
{
public:
	///////////////////////////////////////////////////////
	// Virtual overrides for process::ServiceProvider::Environment

	auto createMicroservice(const process::MicroserviceClass &componentClass, plugin::SharedFactory<process::Microservice> &factory)
		-> std::shared_ptr<process::Microservice> final;
};

auto ServiceProvider::Environment::createMicroservice(const process::MicroserviceClass &componentClass,
	plugin::SharedFactory<process::Microservice> &factory)
	-> std::shared_ptr<process::Microservice>
{
	if (&componentClass == &SampleMicroservice::Class::instance())
	{
		return factory.makeShared<SampleMicroservice>();
	}

	return nullptr;
}

auto ServiceProvider::createEnvironment() -> std::unique_ptr<process::ServiceProvider::Environment>
{
	return std::make_unique<Environment>();
}

} // namespace xentara::plugins::sampleMicroservice