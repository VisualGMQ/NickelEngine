#pragma once
#include "nickel/internal/pch.hpp"
#include "nickel/graphics/enums.hpp"

namespace nickel::graphics {

#define GET_BIT(flags, src, dst) (flags & src ? dst : 0)

constexpr VkImageAspectFlagBits CvtVkImageAspectFlagBits(
    Flags<ImageAspect> aspect) {
    return static_cast<VkImageAspectFlagBits>(
        GET_BIT(aspect, ImageAspect::Color, VK_IMAGE_ASPECT_COLOR_BIT) |
        GET_BIT(aspect, ImageAspect::Depth, VK_IMAGE_ASPECT_DEPTH_BIT) |
        GET_BIT(aspect, ImageAspect::Stencil, VK_IMAGE_ASPECT_STENCIL_BIT));
}

#undef GET_BIT

}  // namespace nickel::graphics