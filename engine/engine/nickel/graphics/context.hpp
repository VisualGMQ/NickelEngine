#pragma once
#include "nickel/common/transform.hpp"
#include "nickel/fs/storage.hpp"
#include "nickel/graphics/gltf.hpp"
#include "nickel/graphics/lowlevel/adapter.hpp"
#include "nickel/graphics/primitive_draw.hpp"

namespace nickel::graphics {

class ContextImpl;

class Context {
public:
    Context(const Adapter&, const video::Window& window,
            StorageManager& storage_mgr);
    ~Context();

    void EnableRender(bool enable);
    bool IsRenderEnabled() const;

    void BeginFrame();
    void EndFrame();

    void DrawLineList(std::span<Vertex> vertices);
    void DrawTriangleList(std::span<Vertex> vertices,
                          std::span<uint32_t> indices);
    void SetClearColor(const Color& color);
    void SetDepthClearValue(float depth, uint32_t stencil);

    void DrawModel(const Transform& transform, const GLTFModel& model);

    void EnableWireFrame(bool enable) const;

    const ContextImpl* GetImpl() const;
    ContextImpl* GetImpl() ;

private:
    std::unique_ptr<ContextImpl> m_impl;
};

}
