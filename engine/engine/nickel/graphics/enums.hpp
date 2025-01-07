#pragma once

namespace nickel::graphics {

enum class ImageAspect {
    None = 0x00,
    Color = 0x01,
    Depth = 0x02,
    Stencil = 0x04,
};

}