#include "nickel/graphics/pipeline_layout.hpp"
#include "nickel/graphics/internal/pipeline_layout_impl.hpp"

namespace nickel::graphics {

PipelineLayout::PipelineLayout(PipelineLayoutImpl* impl) : m_impl{impl} {}

PipelineLayout::PipelineLayout(const PipelineLayout& o) : m_impl{o.m_impl} {
    m_impl->IncRefcount();
}

PipelineLayout::PipelineLayout(PipelineLayout&& o) noexcept : m_impl{o.m_impl} {
    o.m_impl = nullptr;
}

PipelineLayout& PipelineLayout::operator=(const PipelineLayout& o) noexcept {
    if (&o != this) {
        m_impl->DecRefcount();
        m_impl = o.m_impl;
        m_impl->IncRefcount();
    }
    return *this;
}

PipelineLayout& PipelineLayout::operator=(PipelineLayout&& o) noexcept {
    if (&o != this) {
        m_impl = o.m_impl;
        o.m_impl = nullptr;
    }
    return *this;
}

PipelineLayout::~PipelineLayout() {
    m_impl->DecRefcount();
}

const PipelineLayoutImpl& PipelineLayout::Impl() const noexcept {
    return *m_impl;
}

PipelineLayoutImpl& PipelineLayout::Impl() noexcept {
    return *m_impl;
}

}  // namespace nickel::graphics