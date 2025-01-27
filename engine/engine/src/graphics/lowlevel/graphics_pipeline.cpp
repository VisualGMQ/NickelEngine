#include "nickel/graphics/lowlevel/graphics_pipeline.hpp"
#include "nickel/graphics/lowlevel/internal/graphics_pipeline_impl.hpp"

namespace nickel::graphics {

GraphicsPipeline::GraphicsPipeline(GraphicsPipelineImpl* impl) : m_impl{impl} {}

GraphicsPipeline::GraphicsPipeline(const GraphicsPipeline& o) : m_impl{o.m_impl} {
    if (m_impl) {
        m_impl->IncRefcount();
    }
}

GraphicsPipeline::GraphicsPipeline(GraphicsPipeline&& o) noexcept : m_impl{o.m_impl} {
    o.m_impl = nullptr;
}

GraphicsPipeline& GraphicsPipeline::operator=(const GraphicsPipeline& o) noexcept {
    if (&o != this) {
        if (m_impl) {
            m_impl->DecRefcount();
        }
        m_impl = o.m_impl;
        if (m_impl) {
            m_impl->IncRefcount();
        }
    }
    return *this;
}

GraphicsPipeline& GraphicsPipeline::operator=(GraphicsPipeline&& o) noexcept {
    if (&o != this) {
        m_impl = o.m_impl;
        o.m_impl = nullptr;
    }
    return *this;
}

GraphicsPipeline::~GraphicsPipeline() {
    Release();
}

GraphicsPipeline::operator bool() const noexcept {
    return m_impl;
}

const GraphicsPipelineImpl& GraphicsPipeline::Impl() const noexcept {
    return *m_impl;
}

GraphicsPipelineImpl& GraphicsPipeline::Impl() noexcept {
    return *m_impl;
}

void GraphicsPipeline::Release() {
    if (m_impl) {
        m_impl->DecRefcount();
        m_impl = nullptr;
    }
}

}  // namespace nickel::graphics