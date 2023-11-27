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
        field(&::nickel::Sprite::anchor),
        field(&::nickel::Sprite::flip),
        field(&::nickel::Sprite::visiable),
        field(&::nickel::Sprite::texture)
    )
)

srefl_class(::nickel::TextureHandle,
    ctors()
)

#include "mirrow/srefl/srefl_end.hpp"
// clang-format on
