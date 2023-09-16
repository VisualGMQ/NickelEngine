#pragma once

#include "pch.hpp"
#include "renderer/sprite.hpp"

// clang-format off
#include "mirrow/srefl/srefl_begin.hpp"
srefl_class(::nickel::Sprite,
    ctors()
    fields(
        field(&::nickel::Sprite::color),
        field(&::nickel::Sprite::region),
        field(&::nickel::Sprite::customSize),
        field(&::nickel::Sprite::anchor)
    )
)

srefl_class(::nickel::SpriteBundle,
    ctors()
    fields(
        field(&::nickel::SpriteBundle::sprite),
        field(&::nickel::SpriteBundle::image),
        field(&::nickel::SpriteBundle::flip),
        field(&::nickel::SpriteBundle::visiable)
    )
)

#include "mirrow/srefl/srefl_end.hpp"
// clang-format on
