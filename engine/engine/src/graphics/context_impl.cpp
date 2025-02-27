#include "nickel/graphics/internal/context_impl.hpp"

#include "nickel/common/macro.hpp"
#include "nickel/nickel.hpp"

namespace nickel::graphics {

ContextImpl::ContextImpl(const Adapter& adapter, const video::Window& window,
                    StorageManager& storage_mgr)
    : m_common_resource{adapter.GetDevice(), window},
      m_primitive_draw{adapter.GetDevice(), storage_mgr,
                       m_common_resource.m_render_pass, m_common_resource},
      m_imgui_draw{window, adapter},
      m_gltf_draw{adapter.GetDevice(), m_common_resource} {}

void ContextImpl::EnableRender(bool enable) {
    m_enable_render = enable;
}

bool ContextImpl::IsRenderEnabled() const {
    return m_enable_render;
}

bool ContextImpl::ShouldRender() const {
    return m_enable_render &&
           !nickel::Context::GetInst().GetWindow().IsMinimize();
}

void ContextImpl::BeginFrame() {
    m_imgui_draw.Begin();
    
    NICKEL_RETURN_IF_FALSE(ShouldRender());

    Fence fence = m_common_resource.GetFence(m_render_frame_index);

    auto device = nickel::Context::GetInst().GetGPUAdapter().GetDevice();
    m_swapchain_image_index = device.WaitAndAcquireSwapchainImageIndex(
        m_common_resource.GetImageAvaliableSemaphore(m_render_frame_index),
        std::span{&fence, 1});

    m_common_resource.Begin();
    m_primitive_draw.Begin();
}

void ContextImpl::EndFrame() {
    m_imgui_draw.PrepareForRender();
    
    NICKEL_RETURN_IF_FALSE(ShouldRender());

    auto& ctx = nickel::Context::GetInst();
    Device device = ctx.GetGPUAdapter().GetDevice();

    CommandEncoder encoder = device.CreateCommandEncoder();

    if (m_primitive_draw.NeedDraw()) {
        m_primitive_draw.UploadData2GPU(device);
    }

    Rect rect;
    rect.size.w = ctx.GetWindow().GetSize().w;
    rect.size.h = ctx.GetWindow().GetSize().h;

    auto render_pass_encoder = encoder.BeginRenderPass(
        m_common_resource.m_render_pass,
        m_common_resource.GetFramebuffer(m_swapchain_image_index), rect,
        std::span{m_clear_values});
    render_pass_encoder.SetViewport(0, 0, rect.size.w, rect.size.h, 0, 1);
    render_pass_encoder.SetScissor(0, 0, rect.size.w, rect.size.h);

    if (m_primitive_draw.NeedDraw()) {
        m_primitive_draw.ApplyDrawCall(render_pass_encoder, m_is_wireframe);
    }

    if (m_gltf_draw.NeedDraw()) {
        m_gltf_draw.ApplyDrawCall(render_pass_encoder, m_is_wireframe);
    }

    render_pass_encoder.End();

    auto cmd = encoder.Finish();
    device.Submit(
        cmd,
        std::span{
            &m_common_resource.GetImageAvaliableSemaphore(m_render_frame_index),
            1},
        std::span{
            &m_common_resource.GetRenderFinishSemaphore(m_render_frame_index),
            1},
        {});

    m_imgui_draw.End(device, m_common_resource, m_render_frame_index);

    device.Present(std::span{
        &m_common_resource.GetImGuiRenderFinishSemaphore(m_render_frame_index),
        1});
    device.EndFrame();

    m_gltf_draw.End();
    m_common_resource.End();

    m_render_frame_index = (m_render_frame_index + 1) %
                           device.GetSwapchainImageInfo().m_image_count;
}

void ContextImpl::DrawLineList(std::span<Vertex> vertices) {
    NICKEL_RETURN_IF_FALSE(ShouldRender());

    m_primitive_draw.DrawLineList(vertices);
}

void ContextImpl::DrawTriangleList(std::span<Vertex> vertices,
                                   std::span<uint32_t> indices) {
    NICKEL_RETURN_IF_FALSE(ShouldRender());

    m_primitive_draw.DrawTriangleList(vertices, indices);
}

void ContextImpl::DrawModel(const Transform& transform,
                            const GLTFModel& model) {
    NICKEL_RETURN_IF_FALSE(ShouldRender());

    m_gltf_draw.RenderModel(transform, model);
}

void ContextImpl::SetClearColor(const Color& color) {
    m_clear_values[0] = {color.r, color.g, color.b, color.a};
}

void ContextImpl::SetDepthClearValue(float depth, uint32_t stencil) {
    m_clear_values[1] = ClearValue::DepthStencilValue{depth, stencil};
}

void ContextImpl::EnableWireFrame(bool enable) {
    m_is_wireframe = enable;
}

GLTFRenderPass& ContextImpl::GetGLTFRenderPass() {
    return m_gltf_draw;
}

CommonResource& ContextImpl::GetCommonResource() {
    return m_common_resource;
}

}  // namespace nickel::graphics