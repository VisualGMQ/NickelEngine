#pragma once

#include "core/gogl.hpp"
#include "rhi/shader.hpp"

namespace nickel::rhi::gl {

class ShaderModule : public rhi::ShaderModule {
public:
    static auto CreateVertexShader(const std::vector<uint8_t>& code) {
        return ShaderModule(rhi::ShaderModule::Type::Vertex, code);
    }

    static auto CreateFragmentShader(const std::vector<uint8_t>& code) {
        return ShaderModule(rhi::ShaderModule::Type::Fragment, code);
    }

    static auto CreateComputeShader(const std::vector<uint8_t>& code) {
        return ShaderModule(rhi::ShaderModule::Type::Compute, code);
    }

    ShaderModule(ShaderModule&& o)
        : rhi::ShaderModule{rhi::ShaderModule::Type::Unknown} {
        swap(o, *this);
    }

    ShaderModule& operator=(ShaderModule&& o) {
        if (this != &o) {
            swap(*this, o);
        }
        return *this;
    }

    ShaderModule(rhi::ShaderModule::Type type,
                 const std::vector<uint8_t>& code);

    ~ShaderModule() { GL_CALL(glDeleteShader(id_)); }

    toml::table Save2Toml() const override {
        // TODO: not finish
        return {};
    }

    GLuint Raw() const { return id_; }

private:
    GLuint id_ = 0;

    friend void swap(ShaderModule& s1, ShaderModule& s2) noexcept {
        using std::swap;
        swap(s1.id_, s2.id_);
    }
};

}  // namespace nickel::rhi::gl