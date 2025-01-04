#include "nickel/graphics/render_pass.hpp"
#include "nickel/graphics/internal/render_pass_impl.hpp"

namespace nickel::graphics {

RenderPass::RenderPass(RenderPassImpl* impl) : m_impl{impl} {}

RenderPass::RenderPass(const RenderPass& o) : m_impl{o.m_impl} {
    m_impl->IncRefcount();
}

RenderPass::RenderPass(RenderPass&& o) noexcept : m_impl{o.m_impl} {
    o.m_impl = nullptr;
}

RenderPass& RenderPass::operator=(const RenderPass& o) noexcept {
    if (&o != this) {
        m_impl->DecRefcount();
        m_impl = o.m_impl;
        m_impl->IncRefcount();
    }
    return *this;
}

RenderPass& RenderPass::operator=(RenderPass&& o) noexcept {
    if (&o != this) {
        m_impl = o.m_impl;
        o.m_impl = nullptr;
    }
    return *this;
}

RenderPass::~RenderPass() {
    m_impl->DecRefcount();
}

const RenderPassImpl& RenderPass::Impl() const noexcept {
    return *m_impl;
}

RenderPassImpl& RenderPass::Impl() noexcept {
    return *m_impl;
}

}  // namespace nickel::graphics