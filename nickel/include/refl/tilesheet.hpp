#pragma once

#include "pch.hpp"

#include "renderer/tilesheet.hpp"

// clang-format off
#include "mirrow/srefl/srefl_begin.hpp"

srefl_class(::nickel::Margin,
    fields(
        field(&::nickel::Margin::left),
        field(&::nickel::Margin::right),
        field(&::nickel::Margin::top),
        field(&::nickel::Margin::bottom)
    )
)

srefl_class(::nickel::Spacing,
    fields(
        field(&::nickel::Spacing::x),
        field(&::nickel::Spacing::y)
    )
)

srefl_class(::nickel::TilesheetConfig,
    fields(
        field(&::nickel::TilesheetConfig::name),
        field(&::nickel::TilesheetConfig::row),
        field(&::nickel::TilesheetConfig::col),
        field(&::nickel::TilesheetConfig::margin),
        field(&::nickel::TilesheetConfig::spacing)
    )
)

#include "mirrow/srefl/srefl_end.hpp"
    // clang-format on