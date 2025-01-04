#include "nickel/graphics/sampler.hpp"
#include "nickel/graphics/internal/sampler_impl.hpp"

namespace nickel::graphics {

Sampler::Sampler(SamplerImpl* impl) : m_impl{impl} {}

Sampler::Sampler(const Sampler& o) : m_impl{o.m_impl} {
    m_impl->IncRefcount();
}

Sampler::Sampler(Sampler&& o) noexcept : m_impl{o.m_impl} {
    o.m_impl = nullptr;
}

const SamplerImpl& Sampler::Impl() const noexcept {
    return *m_impl;
}

SamplerImpl& Sampler::Impl() noexcept {
    return *m_impl;
}

Sampler& Sampler::operator=(const Sampler& o) noexcept {
    if (&o != this) {
        m_impl->DecRefcount();
        m_impl = o.m_impl;
        m_impl->IncRefcount();
    }
    return *this;
}

Sampler& Sampler::operator=(Sampler&& o) noexcept {
    if (&o != this) {
        m_impl = o.m_impl;
        o.m_impl = nullptr;
    }
    return *this;
}

Sampler::~Sampler() {
    m_impl->DecRefcount();
}

}  // namespace nickel::graphics