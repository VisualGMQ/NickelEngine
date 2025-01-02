#pragma once

namespace nickel::graphics {

class ShaderModuleImpl;

class ShaderModule {
public:
    explicit ShaderModule(ShaderModuleImpl*);
    ShaderModule(const ShaderModule& o);
    ShaderModule(ShaderModule&& o) noexcept;
    ShaderModule& operator=(const ShaderModule& o) noexcept;
    ShaderModule& operator=(ShaderModule&& o) noexcept;
    ~ShaderModule();

private:
    ShaderModuleImpl* m_impl;
};

}  // namespace nickel::graphics