// Copyright (c) embedded ocean GmbH
#include "Tasks.hpp"

#include <xentara/utils/core/Uuid.hpp>

#include <string_view>

namespace xentara::samples::simpleMicroservice::tasks
{

using namespace std::literals;
using namespace xentara::literals;

const process::Task::Role kExecute { "db2775d9-21c6-4bcf-abbb-0f56332bc495"_uuid, "execute"sv };

} // namespace xentara::samples::simpleMicroservice::tasks