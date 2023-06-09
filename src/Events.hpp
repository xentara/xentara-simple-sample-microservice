// Copyright (c) embedded ocean GmbH
#pragma once

#include <xentara/process/Event.hpp>

/// @brief Contains the Xentara events of the driver
namespace xentara::samples::simpleMicroservice::events
{

/// @brief A Xentara event that is fired when the microservice was executed correctly
extern const process::Event::Role kExecuted;
/// @brief A Xentara event that is fired an error occurs executing the microservice
extern const process::Event::Role kError;

} // namespace xentara::samples::simpleMicroservice::events