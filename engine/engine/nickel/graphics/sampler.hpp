#pragma once

namespace nickel::graphics {

class SamplerImpl;

class Sampler {
public:
    explicit Sampler(SamplerImpl*);
    Sampler(const Sampler&);
    Sampler(Sampler&&) noexcept;
    Sampler& operator=(const Sampler&) noexcept;
    Sampler& operator=(Sampler&&) noexcept;
    ~Sampler();

private:
    SamplerImpl* m_impl;
};

}  // namespace nickel::graphics