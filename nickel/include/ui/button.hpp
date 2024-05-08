#pragma once

#include "common/cgmath.hpp"
#include "common/utf8string.hpp"
#include "gecs/entity/resource.hpp"
#include "graphics/texture.hpp"
#include "ui/style.hpp"


namespace nickel::ui {

struct Button final {
    cgmath::Color color = {1, 1, 1, 1};
    cgmath::Color hoverColor = {1, 1, 1, 1};
    cgmath::Color pressColor = {1, 1, 1, 1};
    TextureClip texture;
    TextureClip hoverTexture;
    TextureClip pressTexture;
};

}  // namespace nickel::ui