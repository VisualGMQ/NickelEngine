#pragma once
#include "nickel/graphics/common_resource.hpp"
#include "nickel/graphics/primitive_draw.hpp"

namespace nickel::graphics {
class Context {
public:
    Context(Device device, video::Window& window, StorageManager& storage_mgr);

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
    PrimitiveDrawPass m_primitive_draw;
    uint32_t m_swapchain_image_index{};
    bool m_is_wireframe{};
    bool m_enable_render{true};
    std::array<ClearValue, 2> m_clear_values;
};
}
