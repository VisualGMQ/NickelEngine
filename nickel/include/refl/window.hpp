#pragma once

#include "pch.hpp"
#include "window/window.hpp"

#include "refl/cgmath.hpp"

// clang-format off
#include "mirrow/srefl/srefl_begin.hpp"
srefl_class(::nickel::WindowBuilder::Data,
    ctors()
    fields(
        field(&nickel::WindowBuilder::Data::title),
        field(&nickel::WindowBuilder::Data::size)
    )
)
#include "mirrow/srefl/srefl_end.hpp"
// clang-format on