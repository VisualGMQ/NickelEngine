#pragma once

namespace nickel::graphics {

class ShaderModuleImpl;

class ShaderModule {
public:
    ShaderModule() = default;
    explicit ShaderModule(ShaderModuleImpl*);
    ShaderModule(const ShaderModule& o);
    ShaderModule(ShaderModule&& o) noexcept;
    ShaderModule& operator=(const ShaderModule& o) noexcept;
    ShaderModule& operator=(ShaderModule&& o) noexcept;
    ~ShaderModule();

    const ShaderModuleImpl& Impl() const noexcept;
    ShaderModuleImpl& Impl() noexcept;

    operator bool() const noexcept;

private:
    ShaderModuleImpl* m_impl;
};

}  // namespace nickel::graphics