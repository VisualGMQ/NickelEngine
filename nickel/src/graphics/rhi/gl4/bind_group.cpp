#include "graphics/rhi/gl4/bind_group.hpp"
#include "graphics/rhi/gl4/buffer.hpp"
#include "graphics/rhi/gl4/glcall.hpp"
#include "graphics/rhi/gl4/render_pipeline.hpp"
#include "graphics/rhi/gl4/sampler.hpp"
#include "graphics/rhi/gl4/texture.hpp"


namespace nickel::rhi::gl4 {

BindGroupLayoutImpl::BindGroupLayoutImpl(
    const BindGroupLayout::Descriptor& desc)
    : desc_{desc} {}

const BindGroupLayout::Descriptor& BindGroupLayoutImpl::Descriptor() const {
    return desc_;
}

struct ResourceBindHelper final {
    explicit ResourceBindHelper(const RenderPipelineImpl& pipeline,
                                const ResourceEntry& entry)
        : pipeline_{pipeline}, entry_{entry} {}

    void operator()(const BufferBinding& binding) const {
        auto buffer = static_cast<const BufferImpl*>(binding.buffer.Impl());
        GLenum bufferType;
        switch (binding.type) {
            case BufferType::ReadOnlyStorage:
            case BufferType::Storage:
                bufferType = GL_SHADER_STORAGE_BUFFER;
                break;
            case BufferType::Uniform:
                bufferType = GL_UNIFORM_BUFFER;
                break;
        }
        GL_CALL(glBindBuffer(bufferType, buffer->id));
        if (binding.minBindingSize) {
            GL_CALL(glBindBufferRange(bufferType, entry_.binding, buffer->id, 0,
                                      binding.minBindingSize.value()));
        } else {
            GL_CALL(glBindBufferBase(bufferType, entry_.binding, buffer->id));
        }
    }

    void operator()(const SamplerBinding& binding) {
        GLuint index =
            glGetUniformLocation(pipeline_.GetShaderID(), binding.name.c_str());
        GL_CALL(glActiveTexture(GL_TEXTURE0 + textureCount_));
        static_cast<const TextureImpl*>(binding.view.Texture().Impl())->Bind();
        GL_CALL(glUniform1i(index, textureCount_));
        GL_CALL(glBindSampler(
            textureCount_,
            static_cast<const SamplerImpl*>(binding.sampler.Impl())->id));
        textureCount_++;
    }

    void operator()(const StorageTextureBinding&) const {
        // TODO: not finish
    }

    void operator()(const TextureBinding& binding) {
        GLuint index =
            glGetUniformLocation(pipeline_.GetShaderID(), binding.name.c_str());
        GL_CALL(glActiveTexture(GL_TEXTURE0 + textureCount_));
        static_cast<const TextureImpl*>(binding.view.Texture().Impl())->Bind();
        GL_CALL(glUniform1i(index, textureCount_));
        textureCount_++;
    }

private:
    const RenderPipelineImpl& pipeline_;
    const ResourceEntry& entry_;
    uint32_t textureCount_ = 0;
};

BindGroupImpl::BindGroupImpl(const BindGroup::Descriptor& desc) : desc_{desc} {}

void BindGroupImpl::Apply(const RenderPipelineImpl& pipeline) const {
    for (auto& entry : desc_.entries) {
        ResourceBindHelper helper{pipeline, entry};
        std::visit(helper, entry.entry);
    }
}

BindGroupLayout BindGroupImpl::GetLayout() const {
    return desc_.layout;
}

}  // namespace nickel::rhi::gl4