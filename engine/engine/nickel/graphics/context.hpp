#pragma once
#include "nickel/graphics/lowlevel/adapter.hpp"
#include "nickel/graphics/primitive_draw.hpp"

namespace nickel::graphics {
class Context {
public:
    Context(const Adapter&, const video::Window& window,
            StorageManager& storage_mgr);
    ~Context();

    void EnableRender(bool enable);
    bool IsRenderEnabled() const;

    void BeginFrame();
    void EndFrame();

    void DrawLineStrip(std::span<Vertex> vertices);
    void DrawTriangleList(std::span<Vertex> vertices,
                          std::span<uint32_t> indices);
    void SetClearColor(const Color& color);
    void SetDepthClearValue(float depth, uint32_t stencil);

    void EnableWireFrame(bool enable) const;

private:
    class Impl;
    std::unique_ptr<Impl> m_impl;
};

}
