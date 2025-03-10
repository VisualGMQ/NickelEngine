#include "nickel/graphics/context.hpp"
#include "nickel/graphics/common_resource.hpp"
#include "nickel/graphics/imgui_draw.hpp"
#include "nickel/graphics/internal/context_impl.hpp"
#include "nickel/graphics/lowlevel/internal/device_impl.hpp"
#include "nickel/nickel.hpp"

namespace nickel::graphics {

Context::Context(const Adapter& adapter, const video::Window& window,
                 StorageManager& storage_mgr)
    : m_impl{std::make_unique<ContextImpl>(adapter, window, storage_mgr)} {}

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

void Context::DrawLineList(std::span<Vertex> vertices) {
    m_impl->DrawLineList(vertices);
}

void Context::DrawTriangleList(std::span<Vertex> vertices,
                               std::span<uint32_t> indices, bool wireframe) {
    m_impl->DrawTriangleList(vertices, indices, wireframe);
}

void Context::SetClearColor(const Color& color) {
    m_impl->SetClearColor(color);
}

void Context::SetDepthClearValue(float depth, uint32_t stencil) {
    m_impl->SetDepthClearValue(depth, stencil);
}

void Context::DrawModel(const Transform& transform, const GLTFModel& model) {
    m_impl->DrawModel(transform, model);
}

void Context::EnableWireFrame(bool enable) const {
    m_impl->EnableWireFrame(enable);
}

void Context::OnSwapchainRecreate(const video::Window& window,
                                  Adapter& adapter) {
    m_impl->OnSwapchainRecreate(window, adapter);
}

const ContextImpl* Context::GetImpl() const {
    return m_impl.get();
}

ContextImpl* Context::GetImpl() {
    return m_impl.get();
}

}  // namespace nickel::graphics
