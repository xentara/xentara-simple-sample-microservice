// Copyright (c) embedded ocean GmbH
#pragma once

#include <xentara/model/Attribute.hpp>

// Contains the Xentara attributes of the uplink skill
namespace xentara::samples::simpleMicroservice::attributes
{

// A Xentara attribute containing the execution state of a microservice
extern const model::Attribute kExecutionState;
// A Xentara attribute containing the execution time of a microservice
extern const model::Attribute kExecutionTime;
// A Xentara attribute containing an error message for a microservice
extern const model::Attribute kError;

} // namespace xentara::samples::simpleMicroservice::attributes
