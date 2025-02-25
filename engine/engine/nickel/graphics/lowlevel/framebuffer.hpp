#pragma once
#include "nickel/common/dllexport.hpp"
#include "nickel/common/impl_wrapper.hpp"
#include "nickel/common/math/math.hpp"
#include "nickel/graphics/lowlevel/render_pass.hpp"

namespace nickel::graphics {
class ImageView;

class FramebufferImpl;

class NICKEL_API Framebuffer: public ImplWrapper<FramebufferImpl> {
public:
    struct Descriptor {
        std::vector<ImageView> m_views;
        SVector<uint32_t, 3> m_extent;
        RenderPass m_render_pass;
    };

    using ImplWrapper::ImplWrapper;
};

}  // namespace nickel::graphics