#pragma once

#include "core/cgmath.hpp"
#include "core/utf8string.hpp"
#include "gecs/entity/resource.hpp"
#include "renderer/texture.hpp"
#include "ui/style.hpp"


namespace nickel::ui {

class Button final {
public:
    utf8string text;

    cgmath::Color color;
    cgmath::Color hoverColor;
    cgmath::Color pressColor;
};

class TextureButton final {
public:
    TextureHandle texture;
    TextureHandle hoverTexture;
    TextureHandle pressTexture;
};

}  // namespace nickel::ui