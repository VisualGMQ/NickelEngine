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
                                const BindingPoint& entry,
                                uint32_t dynamicOffset)
        : pipeline_{pipeline}, entry_{entry}, dynamicOffset_{dynamicOffset} {}

    bool operator()(const BufferBinding& binding) const {
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
        auto offset = binding.hasDynamicOffset ? dynamicOffset_ : 0;
        auto size = binding.minBindingSize ? binding.minBindingSize.value()
                                           : buffer->Size();
        GL_CALL(glBindBufferRange(bufferType, entry_.binding, buffer->id,
                                  offset, size));
        return binding.hasDynamicOffset;
    }

    bool operator()(const SamplerBinding& binding) {
        GLuint index =
            glGetUniformLocation(pipeline_.GetShaderID(), binding.name.c_str());
        GL_CALL(glActiveTexture(GL_TEXTURE0 + textureCount_));
        static_cast<const TextureImpl*>(binding.view.Texture().Impl())->Bind();
        GL_CALL(glUniform1i(index, textureCount_));
        GL_CALL(glBindSampler(
            textureCount_,
            static_cast<const SamplerImpl*>(binding.sampler.Impl())->id));
        textureCount_++;
        return false;
    }

    bool operator()(const StorageTextureBinding& binding) const {
        // TODO: not finish
        return false;
    }

    bool operator()(const TextureBinding& binding) {
        GLuint index =
            glGetUniformLocation(pipeline_.GetShaderID(), binding.name.c_str());
        GL_CALL(glActiveTexture(GL_TEXTURE0 + textureCount_));
        static_cast<const TextureImpl*>(binding.view.Texture().Impl())->Bind();
        GL_CALL(glUniform1i(index, textureCount_));
        textureCount_++;
        return false;
    }

private:
    const RenderPipelineImpl& pipeline_;
    const BindingPoint& entry_;
    uint64_t dynamicOffset_;
    uint32_t textureCount_ = 0;
};

BindGroupImpl::BindGroupImpl(const BindGroup::Descriptor& desc) {
    auto& layoutDesc =
        static_cast<const BindGroupLayoutImpl*>(desc.layout.Impl())
            ->Descriptor();
    desc_.layout = desc.layout;
    for (auto& layoutEntry : layoutDesc.entries) {
        desc_.entries.emplace_back(layoutEntry.binding);
        for (auto& groupEntry : desc.entries) {
            if (groupEntry.binding == layoutEntry.binding.binding) {
                desc_.entries.back() = groupEntry;
                break;
            }
        }
    }
}

void BindGroupImpl::Apply(const RenderPipelineImpl& pipeline,
                          const std::vector<uint32_t>& dynamicOffset) const {
    auto& layoutDesc =
        static_cast<const BindGroupLayoutImpl*>(desc_.layout.Impl())
            ->Descriptor();

    uint32_t bufferIdx = 0;
    for (auto& entry : desc_.entries) {
        ResourceBindHelper helper{
            pipeline, entry,
            dynamicOffset.size() <= bufferIdx ? 0 : dynamicOffset[bufferIdx]};
        if (std::visit(helper, entry.entry)) {
            bufferIdx++;
        }
    }
}

BindGroupLayout BindGroupImpl::GetLayout() const {
    return desc_.layout;
}

}  // namespace nickel::rhi::gl4