#include "graphics/rhi/gl4/bind_group.hpp"
#include "graphics/rhi/gl4/buffer.hpp"
#include "graphics/rhi/gl4/glcall.hpp"

namespace nickel::rhi::gl4 {

BindGroupLayoutImpl::BindGroupLayoutImpl(
    const BindGroupLayout::Descriptor& desc)
    : desc_{desc} {}

const BindGroupLayout::Descriptor& BindGroupLayoutImpl::Descriptor() const {
    return desc_;
}

struct ResourceBindHelper final {
    explicit ResourceBindHelper(const Entry& entry)
        : entry_{entry} {}

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
            GL_CALL(glBindBufferRange(bufferType, entry_.binding, buffer->id,
                              0, binding.minBindingSize.value()));
        } else {
            GL_CALL(glBindBufferBase(bufferType, entry_.binding, buffer->id));
        }
    }

    void operator()(const SamplerBinding&) const {
        // TODO: not finish
    }

    void operator()(const StorageTextureBinding&) const {
        // TODO: not finish
    }

    void operator()(const TextureBinding& binding) const {
        // TODO: not finish
    }

private:
    const Entry& entry_;
};

BindGroupImpl::BindGroupImpl(const BindGroup::Descriptor& desc)
    : desc_{desc} {}

void BindGroupImpl::Apply() const {
    for (auto& entry : desc_.entries) {
        ResourceBindHelper helper{entry};
        std::visit(helper, entry.resourceLayout);
    }
}

BindGroupLayout BindGroupImpl::GetLayout() const {
    return desc_.layout;
}

}  // namespace nickel::rhi::gl4