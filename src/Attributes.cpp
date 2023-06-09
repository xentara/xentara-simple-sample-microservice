// Copyright (c) embedded ocean GmbH
#include "Attributes.hpp"

#include <xentara/utils/core/Uuid.hpp>
#include <xentara/data/DataType.hpp>

#include <string_view>

namespace xentara::samples::simpleMicroservice::attributes
{

using namespace std::literals;

const model::Attribute kExecutionState { "8270cec1-050b-499c-b03f-6bcc41dad49e"_uuid, "executionState"sv, model::Attribute::Access::ReadOnly, data::DataType::kBoolean };
const model::Attribute kExecutionTime { "8270cec1-050b-499c-b03f-6bcc41dad49e"_uuid, "executionTime"sv, model::Attribute::Access::ReadOnly, data::DataType::kTimeStamp };
const model::Attribute kError { model::Attribute::kError, model::Attribute::Access::ReadOnly, data::DataType::kString };

} // namespace xentara::samples::simpleMicroservice::attributes
