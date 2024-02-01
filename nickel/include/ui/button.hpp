#pragma once

#include "common/cgmath.hpp"
#include "common/utf8string.hpp"
#include "gecs/entity/resource.hpp"
#include "graphics/texture.hpp"
#include "ui/style.hpp"


namespace nickel::ui {

class Button final {
public:
    cgmath::Color color = {1, 1, 1, 1};
    cgmath::Color hoverColor = {1, 1, 1, 1};
    cgmath::Color pressColor = {1, 1, 1, 1};
};

class TextureButton final {
public:
    TextureHandle texture;
    TextureHandle hoverTexture;
    TextureHandle pressTexture;
};

}  // namespace nickel::ui