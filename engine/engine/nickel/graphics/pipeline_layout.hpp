#pragma once

namespace nickel::graphics {

class PipelineLayoutImpl;

class PipelineLayout {
public:
    explicit PipelineLayout(PipelineLayoutImpl*);
    PipelineLayout(const PipelineLayout&);
    PipelineLayout(PipelineLayout&&) noexcept;
    PipelineLayout& operator=(const PipelineLayout&) noexcept;
    PipelineLayout& operator=(PipelineLayout&&) noexcept;
    ~PipelineLayout();

    const PipelineLayoutImpl& Impl() const noexcept;
    PipelineLayoutImpl& Impl() noexcept;
    
private:
    PipelineLayoutImpl* m_impl;
};

}