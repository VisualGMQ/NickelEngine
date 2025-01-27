#pragma once
#include "nickel/common/dllexport.hpp"

namespace nickel::graphics {

class ShaderModuleImpl;

class NICKEL_API ShaderModule {
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
    void Release();

private:
    ShaderModuleImpl* m_impl{};
};

}  // namespace nickel::graphics