#pragma once

#include "graphics/rhi/bind_group.hpp"
#include "graphics/rhi/impl/command.hpp"
#include "graphics/rhi/vk/pch.hpp"
#include "graphics/rhi/vk/util.hpp"
#include "graphics/rhi/vk/render_pipeline.hpp"
#include "graphics/rhi/renderpass.hpp"

namespace nickel::rhi::vulkan {

class DeviceImpl;
class RenderPassImpl;

class RenderPassEncoderImpl : public rhi::RenderPassEncoderImpl {
public:
    RenderPassEncoderImpl(DeviceImpl&, vk::CommandBuffer, RenderPassImpl&);

    void Draw(uint32_t vertexCount, uint32_t instanceCount,
              uint32_t firstVertex, uint32_t firstInstance) override;
    void DrawIndexed(uint32_t indexCount, uint32_t instanceCount,
                     uint32_t firstIndex, uint32_t baseVertex,
                     uint32_t firstInstance) override;
    void SetVertexBuffer(uint32_t slot, Buffer buffer, uint64_t offset,
                         uint64_t size) override;
    void SetIndexBuffer(Buffer buffer, IndexType, uint32_t offset,
                        uint32_t size) override;
    void SetBindGroup(BindGroup) override;
    void SetBindGroup(BindGroup, const std::vector<uint32_t>& dynamicOffset) override;
    void SetPipeline(RenderPipeline) override;
    void SetPushConstant(ShaderStage stage, const void* value, uint32_t offset, uint32_t size) override;

    void End() override;

    RenderPassImpl& GetRenderPass() const;

private:
    DeviceImpl& dev_;
    vk::CommandBuffer cmd_;
    RenderPipeline pipeline_;
    RenderPassImpl& renderPass_;
};

enum class CmdType {
    None = 0,
    RenderPass,
    CopyData,

    // use for number record, dont use
    CmdTypeNumber,
};

class CommandEncoderImpl : public rhi::CommandEncoderImpl {
public:
    explicit CommandEncoderImpl(DeviceImpl&, vk::CommandPool);
    ~CommandEncoderImpl();

    void CopyBufferToBuffer(const Buffer& src, uint64_t srcOffset,
                            const Buffer& dst, uint64_t dstOffset,
                            uint64_t size) override;
    void CopyBufferToTexture(const CommandEncoder::BufTexCopySrc& src,
                             const CommandEncoder::BufTexCopyDst& dst,
                             const Extent3D& copySize) override;
    RenderPassEncoder BeginRenderPass(const RenderPass::Descriptor&) override;

    CommandBuffer Finish() override;

    CmdType Type() const { return type_; }

    vk::CommandPool pool;
    vk::CommandBuffer buf;

private:
    CommandBufferImpl* cmdBuf_{};
    DeviceImpl& dev_;
    CmdType type_ = CmdType::None;
};

struct LayoutTransition final {
    vk::ImageLayout& oldLayout;
    vk::ImageLayout newLayout;
};

class CommandBufferImpl : public rhi::CommandBufferImpl {
public:
    friend class CommandEncoderImpl;

    explicit CommandBufferImpl(vk::CommandBuffer buf) : buf{buf} {}

    vk::CommandBuffer buf;

    CmdType Type() const { return type_; }

    void AddLayoutTransition(vk::ImageLayout& layout, vk::ImageLayout newLayout) {
        layoutTransition_.emplace_back(
            LayoutTransition{layout, newLayout});
    }

    void ApplyLayoutTransition() {
        for (auto& trans : layoutTransition_) {
            trans.oldLayout = trans.newLayout;
        }
        layoutTransition_.clear();
    }

private:
    CmdType type_ = CmdType::None;
    std::vector<LayoutTransition> layoutTransition_;
};

}  // namespace nickel::rhi::vulkan