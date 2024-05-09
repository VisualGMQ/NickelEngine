#include "graphics/rhi/gl4/bind_group.hpp"
#include "graphics/rhi/gl4/buffer.hpp"
#include "graphics/rhi/gl4/glcall.hpp"
#include "graphics/rhi/gl4/render_pipeline.hpp"
#include "graphics/rhi/gl4/sampler.hpp"
#include "graphics/rhi/gl4/texture.hpp"
#include "graphics/rhi/gl4/texture_view.hpp"

namespace nickel::rhi::gl4 {

BindGroupLayoutImpl::BindGroupLayoutImpl(
    const BindGroupLayout::Descriptor& desc)
    : desc_{desc} {}

const BindGroupLayout::Descriptor& BindGroupLayoutImpl::Descriptor() const {
    return desc_;
}

struct ResourceBindHelper final {
    explicit ResourceBindHelper(const RenderPipelineImpl& pipeline,
                                uint32_t dynamicOffset)
        : pipeline_{pipeline}, dynamicOffset_{dynamicOffset} {}

    void SetEntry(const BindingPoint& entry) {
        entry_ = &entry;
    }

    bool operator()(const BufferBinding& binding) const {
        auto buffer = static_cast<const BufferImpl*>(binding.buffer.Impl());
        GLenum bufferType;
#ifdef NICKEL_HAS_GL4
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
        GL_CALL(glBindBufferRange(bufferType, entry_->binding, buffer->id,
                                  offset, size));
        return binding.hasDynamicOffset;

#else
        switch (binding.type) {
            case BufferType::ReadOnlyStorage:
            case BufferType::Storage:
            case BufferType::Uniform:
                bufferType = GL_UNIFORM_BUFFER;
                break;
        }

        auto index = glGetUniformBlockIndex(pipeline_.GetShaderID(),
                                                   binding.name.c_str());
        if (index == GL_INVALID_INDEX) {
            LOGE(log_tag::GL, "uniform buffer ", binding.name, " not exists");
        }
        GL_CALL(glUniformBlockBinding(pipeline_.GetShaderID(), index, entry_->binding));
        GL_CALL(glBindBuffer(bufferType, buffer->id));
        auto offset = binding.hasDynamicOffset ? dynamicOffset_ : 0;
        auto size = binding.minBindingSize ? binding.minBindingSize.value()
                                           : buffer->Size();
        GL_CALL(glBindBufferRange(bufferType, entry_->binding, buffer->id, offset, size));
        return binding.hasDynamicOffset;

#endif
    }

    bool operator()(const SamplerBinding& binding) {
#ifdef NICKEL_HAS_GL4
        static_cast<const TextureViewImpl*>(binding.view.Impl())
            ->Bind(entry_->binding);
        GL_CALL(glBindSampler(
            entry_->binding,
            static_cast<const SamplerImpl*>(binding.sampler.Impl())->id));
#else
        GLint loc =
            glGetUniformLocation(pipeline_.GetShaderID(), binding.name.c_str());
        if (loc == -1) {
            LOGE(log_tag::GL, "uniform ", binding.name, " not exists");
        } else {
            GL_CALL(glUniform1i(loc, textureCount_));
        }
        static_cast<const TextureViewImpl*>(binding.view.Impl())
            ->Bind(textureCount_);

        GL_CALL(glBindSampler(
            textureCount_,
            static_cast<const SamplerImpl*>(binding.sampler.Impl())->id));
#endif

        textureCount_++;
        return false;
    }

    bool operator()(const StorageTextureBinding& binding) const {
        // TODO: not finish
        return false;
    }

    bool operator()(const TextureBinding& binding) {
        auto view = static_cast<const TextureViewImpl*>(binding.view.Impl());

#ifdef NICKEL_HAS_GL4
        view->Bind(entry_->binding);
#else
        GLint loc =
            glGetUniformLocation(pipeline_.GetShaderID(), binding.name.c_str());
        if (loc == -1) {
            LOGE(log_tag::GL, "uniform ", binding.name, " not exists");
        } else {
            GL_CALL(glUniform1i(loc, textureCount_));
        }
        view->Bind(textureCount_);
#endif
        textureCount_++;
        return false;
    }

private:
    const RenderPipelineImpl& pipeline_;
    const BindingPoint* entry_;
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
        static_cast<BindGroupLayoutImpl*>(desc_.layout.Impl())->Descriptor();

    uint32_t bufferIdx = 0;
    ResourceBindHelper helper{pipeline, dynamicOffset.size() <= bufferIdx
                                            ? 0
                                            : dynamicOffset[bufferIdx]};
    for (auto& entry : desc_.entries) {
        helper.SetEntry(entry);
        if (std::visit(helper, entry.entry)) {
            bufferIdx++;
        }
    }
}

BindGroupLayout BindGroupImpl::GetLayout() const {
    return desc_.layout;
}

}  // namespace nickel::rhi::gl4