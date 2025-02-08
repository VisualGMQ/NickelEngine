#include "nickel/graphics/context.hpp"
#include "nickel/common/macro.hpp"
#include "nickel/graphics/common_resource.hpp"
#include "nickel/graphics/imgui_draw.hpp"
#include "nickel/graphics/lowlevel/internal/adapter_impl.hpp"
#include "nickel/graphics/lowlevel/internal/device_impl.hpp"
#include "nickel/nickel.hpp"

namespace nickel::graphics {

class Context::Impl {
public:
    Impl(const Adapter& adapter, const video::Window& window,
         StorageManager& storage_mgr);

    void EnableRender(bool enable);
    bool IsRenderEnabled() const;

    void BeginFrame();
    void EndFrame();

    void DrawLineStrip(std::span<Vertex> vertices);
    void DrawTriangleList(std::span<Vertex> vertices,
                          std::span<uint32_t> indices);
    void SetClearColor(const Color& color);
    void SetDepthClearValue(float depth, uint32_t stencil);

    void EnableWireFrame(bool enable);

private:
    CommonResource m_common_resource;
    PrimitiveRenderPass m_primitive_draw;
    ImGuiRenderPass m_imgui_draw;
    uint32_t m_swapchain_image_index{};
    uint32_t m_render_frame_index{};
    bool m_is_wireframe{};
    bool m_enable_render{true};
    std::array<ClearValue, 2> m_clear_values;
};

Context::Impl::Impl(const Adapter& adapter, const video::Window& window,
                    StorageManager& storage_mgr)
    : m_common_resource{adapter.GetDevice(), window},
      m_primitive_draw{adapter.GetDevice(), storage_mgr,
                       m_common_resource.m_render_pass},
      m_imgui_draw{window, adapter} {}

void Context::Impl::EnableRender(bool enable) {
    m_enable_render = enable;
}

bool Context::Impl::IsRenderEnabled() const {
    return m_enable_render;
}

void Context::Impl::BeginFrame() {
    NICKEL_RETURN_IF_FALSE(m_enable_render);

    Fence fence = m_common_resource.GetFence(m_render_frame_index);

    auto device = nickel::Context::GetInst().GetGPUAdapter().GetDevice();
    m_swapchain_image_index = device.WaitAndAcquireSwapchainImageIndex(
        m_common_resource.GetImageAvaliableSemaphore(m_render_frame_index),
        std::span{&fence, 1});

    m_imgui_draw.Begin();
    m_primitive_draw.Begin();
}

void Context::Impl::EndFrame() {
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
        m_common_resource.GetFramebuffer(m_swapchain_image_index), rect,
        std::span{m_clear_values});
    render_pass_encoder.SetViewport(0, 0, rect.size.w, rect.size.h, 0, 1);
    render_pass_encoder.SetScissor(0, 0, rect.size.w, rect.size.h);

    if (m_primitive_draw.NeedDraw()) {
        m_primitive_draw.ApplyDrawCall(render_pass_encoder, m_is_wireframe);
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
        // m_common_resource.GetFence(m_render_frame_index));
        {});

    m_imgui_draw.End(device, m_common_resource, m_render_frame_index);

    device.Present(std::span{
        &m_common_resource.GetImGuiRenderFinishSemaphore(m_render_frame_index), 1});
    device.EndFrame();

    m_render_frame_index = (m_render_frame_index + 1) %
                           device.GetSwapchainImageInfo().m_image_count;
}

void Context::Impl::DrawLineStrip(std::span<Vertex> vertices) {
    NICKEL_RETURN_IF_FALSE(m_enable_render);

    m_primitive_draw.DrawLineStrip(vertices);
}

void Context::Impl::DrawTriangleList(std::span<Vertex> vertices,
                                     std::span<uint32_t> indices) {
    NICKEL_RETURN_IF_FALSE(m_enable_render);

    m_primitive_draw.DrawTriangleList(vertices, indices);
}

void Context::Impl::SetClearColor(const Color& color) {
    m_clear_values[0] = {color.r, color.g, color.b, color.a};
}

void Context::Impl::SetDepthClearValue(float depth, uint32_t stencil) {
    m_clear_values[1] = ClearValue::DepthStencilValue{depth, stencil};
}

void Context::Impl::EnableWireFrame(bool enable) {
    m_is_wireframe = enable;
}

Context::Context(const Adapter& adapter, const video::Window& window,
                 StorageManager& storage_mgr)
    : m_impl{std::make_unique<Impl>(adapter, window, storage_mgr)} {}

Context::~Context() {}

void Context::EnableRender(bool enable) {
    m_impl->EnableRender(enable);
}

bool Context::IsRenderEnabled() const {
    return m_impl->IsRenderEnabled();
}

void Context::BeginFrame() {
    m_impl->BeginFrame();
}

void Context::EndFrame() {
    m_impl->EndFrame();
}

void Context::DrawLineStrip(std::span<Vertex> vertices) {
    m_impl->DrawLineStrip(vertices);
}

void Context::DrawTriangleList(std::span<Vertex> vertices,
                               std::span<uint32_t> indices) {
    m_impl->DrawTriangleList(vertices, indices);
}

void Context::SetClearColor(const Color& color) {
    m_impl->SetClearColor(color);
}

void Context::SetDepthClearValue(float depth, uint32_t stencil) {
    m_impl->SetDepthClearValue(depth, stencil);
}

void Context::EnableWireFrame(bool enable) const {
    m_impl->EnableWireFrame(enable);
}

}  // namespace nickel::graphics
