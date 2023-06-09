// Copyright (c) embedded ocean GmbH
#include "Events.hpp"

#include <xentara/utils/core/Uuid.hpp>

#include <string_view>

namespace xentara::samples::simpleMicroservice::events
{

using namespace std::literals;
using namespace xentara::literals;

const process::Event::Role kExecuted { "47d0e52c-98e2-44ad-a63a-c90799b8b6ce"_uuid, "executed"sv };

const process::Event::Role kError { "5fc3a10f-460e-4319-a692-f1d3bb649b87"_uuid, "error"sv };

} // namespace xentara::samples::simpleMicroservice::events