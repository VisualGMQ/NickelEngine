#pragma once
#include "nickel/fs/storage.hpp"
#include "nickel/graphics/common_resource.hpp"
#include "nickel/graphics/gltf_draw.hpp"
#include "nickel/graphics/imgui_draw.hpp"
#include "nickel/graphics/lowlevel/adapter.hpp"
#include "nickel/graphics/primitive_draw.hpp"

namespace nickel::graphics {

class ContextImpl {
public:
    ContextImpl(const Adapter& adapter, const video::Window& window,
                StorageManager& storage_mgr);

    void EnableRender(bool enable);
    bool IsRenderEnabled() const;

    void BeginFrame();
    void EndFrame();

    void DrawLineList(std::span<Vertex> vertices);
    void DrawTriangleList(std::span<Vertex> vertices,
                          std::span<uint32_t> indices);
    void DrawModel(const Transform& transform, const GLTFModel& model);

    void SetClearColor(const Color& color);
    void SetDepthClearValue(float depth, uint32_t stencil);

    void EnableWireFrame(bool enable);

    bool ShouldRender() const;

    GLTFRenderPass& GetGLTFRenderPass();
    CommonResource& GetCommonResource();

    void OnSwapchainRecreate(const video::Window& window, Adapter&);

private:
    CommonResource m_common_resource;
    PrimitiveRenderPass m_primitive_draw;
    ImGuiRenderPass m_imgui_draw;
    GLTFRenderPass m_gltf_draw;
    uint32_t m_swapchain_image_index{};
    uint32_t m_render_frame_index{};
    bool m_is_wireframe{};
    bool m_enable_render{true};
    std::array<ClearValue, 2> m_clear_values;
};

}  // namespace nickel::graphics