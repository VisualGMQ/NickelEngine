#include "nickel/graphics/context.hpp"
#include "nickel/nickel.hpp"
#include "nickel/common/macro.hpp"

namespace nickel::graphics {
Context::Context(Device device, video::Window& window,
                 StorageManager& storage_mgr)
    : m_common_resource{device, window},
      m_primitive_draw(device, storage_mgr, m_common_resource.m_render_pass) {
}

void Context::EnableRender(bool enable) {
    m_enable_render = enable;
}

bool Context::IsRenderEnabled() const {
    return m_enable_render;
}

void Context::BeginFrame() {
    NICKEL_RETURN_IF_FALSE(m_enable_render);

    auto device = nickel::Context::GetInst().GetGPUAdapter().GetDevice();
    m_swapchain_image_index = device.WaitAndAcquireSwapchainImageIndex();

    m_primitive_draw.Begin();
}

void Context::EndFrame() {
    NICKEL_RETURN_IF_FALSE(m_enable_render);

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
        m_common_resource.m_fbos[m_swapchain_image_index],
        rect, std::span{m_clear_values});
    render_pass_encoder.SetViewport(0, 0, rect.size.w, rect.size.h, 0, 1);
    render_pass_encoder.SetScissor(0, 0, rect.size.w, rect.size.h);

    if (m_primitive_draw.NeedDraw()) {
        m_primitive_draw.ApplyDrawCall(render_pass_encoder, m_is_wireframe);
    }

    render_pass_encoder.End();

    // TODO: add other pipelines

    auto cmd = encoder.Finish();
    device.Submit(cmd);

    device.EndFrame();
}

void Context::DrawLineStrip(std::span<Vertex> vertices) {
    NICKEL_RETURN_IF_FALSE(m_enable_render);

    m_primitive_draw.DrawLineStrip(vertices);
}

void Context::DrawTriangleList(std::span<Vertex> vertices,
                               std::span<uint32_t> indices) {
    NICKEL_RETURN_IF_FALSE(m_enable_render);

    m_primitive_draw.DrawTriangleList(vertices, indices);
}

void Context::SetClearColor(const Color& color) {
    m_clear_values[0] = {color.r, color.g, color.b, color.a};
}

void Context::SetDepthClearValue(float depth, uint32_t stencil) {
    m_clear_values[1] = ClearValue::DepthStencilValue{depth, stencil};
}

void Context::EnableWireFrame(bool enable) {
    m_is_wireframe = enable;
}
}
