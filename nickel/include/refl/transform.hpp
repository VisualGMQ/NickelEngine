#pragma once

#include "misc/transform.hpp"
#include "pch.hpp"

// clang-format off
#include "mirrow/srefl/srefl_begin.hpp"
srefl_class(::nickel::Transform,
    ctors()
    fields(
        field(&::nickel::Transform::translation),
        field(&::nickel::Transform::rotation),
        field(&::nickel::Transform::scale)
    )
)

#include "mirrow/srefl/srefl_end.hpp"
    // clang-format on
