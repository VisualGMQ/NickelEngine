#pragma once

#include "pch.hpp"
#include "renderer/camera.hpp"

// clang-format off
#include "mirrow/srefl/srefl_begin.hpp"
srefl_class(::nickel::Camera2D::ConfigData,
    ctors()
    fields(
        field(&::nickel::Camera2D::ConfigData::left),
        field(&::nickel::Camera2D::ConfigData::right),
        field(&::nickel::Camera2D::ConfigData::top),
        field(&::nickel::Camera2D::ConfigData::bottom),
        field(&::nickel::Camera2D::ConfigData::near),
        field(&::nickel::Camera2D::ConfigData::far)
    )
)

#include "mirrow/srefl/srefl_end.hpp"
// clang-format on
