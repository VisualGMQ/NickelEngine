#pragma once

#include "core/cgmath.hpp"
#include "core/gogl.hpp"
#include "core/log_tag.hpp"
#include "renderer/camera.hpp"
#include "renderer/context.hpp"
#include "renderer/texture.hpp"
#include "renderer/vertex.hpp"


namespace nickel {

struct RectSampler final {
    const Texture* texture = nullptr;
    cgmath::Rect region = {0.0, 0.0, 1.0, 1.0};
};

struct CircleSampler final {
    const Texture* texture = nullptr;
    float radius = 0.5;
    cgmath::Vec2 center = {0.5, 0.5};
};

class Renderer2D {
public:
    virtual ~Renderer2D() = default;

    Renderer2D();

    void BeginRenderTexture(const Camera& camera);
    void BeginRenderFont(const Camera& camera);
    void EndRender();

    void EnableDepthTest() { GL_CALL(glEnable(GL_DEPTH_TEST)); }

    void DisableDepthTest() { GL_CALL(glDisable(GL_DEPTH_TEST)); }

    void SetViewport(const cgmath::Vec2& offset, const cgmath::Vec2& size);

    void SetLineWidth(float width) { GL_CALL(glLineWidth(width)); }

    void SetClearColor(const cgmath::Color& color) {
        GL_CALL(glClearColor(color.x, color.y, color.z, color.w));
    }

    void Clear(const Camera& camera) {
        camera.ApplyRenderTarget();
        GL_CALL(glClear(GL_COLOR_BUFFER_BIT));
    }

    void ClearDepth(const Camera& camera) {
        camera.ApplyRenderTarget();
        GL_CALL(glClear(GL_DEPTH_BUFFER_BIT));
    }

    template <typename Vertices, typename = std::enable_if_t<std::is_same_v<
                                     typename Vertices::value_type, Vertex>>>
    void DrawLines(const Vertices& vertices) {
        Draw(gogl::PrimitiveType::LineStrip, vertices,
             std::array<uint32_t, 0>{});
    }

    template <typename Vertices, typename = std::enable_if_t<std::is_same_v<
                                     typename Vertices::value_type, Vertex>>>
    void DrawLineLoop(const Vertices& vertices,
                      const cgmath::Mat44& model = cgmath::Mat44::Identity()) {
        Draw(gogl::PrimitiveType::LineLoop, vertices,
             std::array<uint32_t, 0>{});
    }

    template <typename Container,
              typename = std::enable_if_t<
                  std::is_same_v<typename Container::value_type, cgmath::Vec2>>>
    void DrawLines(const Container& pts, const cgmath::Color& color) {
        for (int i = 0; i < pts.size() - 1; i++) {
            DrawLine(pts[i], pts[(i + 1) % pts.size()], color);
        }
    }

    template <typename Container,
              typename = std::enable_if_t<
                  std::is_same_v<typename Container::value_type, cgmath::Vec2>>>
    void DrawLineLoop(const Container& pts, const cgmath::Color& color) {
        for (int i = 0; i < pts.size(); i++) {
            DrawLine(pts[i], pts[(i + 1) % pts.size()], color);
        }
    }

    void DrawLine(const cgmath::Vec2& p1, const cgmath::Vec2& p2,
                  const cgmath::Vec4& color) {
        DrawLines(std::array<Vertex, 2>{Vertex::FromPosColor(p1, color),
                                        Vertex::FromPosColor(p2, color)});
    }

    template <typename Vertices, typename Indices>
    void DrawTriangles(
        Vertices vertices, const Indices& indices,
        const cgmath::Mat44& model = cgmath::Mat44::Identity(),
        const std::optional<RectSampler>& sampler = std::nullopt);

    void DrawRect(const cgmath::Rect& rect, const cgmath::Vec4& color,
                  const cgmath::Mat44& model = cgmath::Mat44::Identity());

    void FillRect(const cgmath::Rect& rect, const cgmath::Vec4& color,
                  const cgmath::Mat44& model = cgmath::Mat44::Identity(),
                  const std::optional<RectSampler>& image = std::nullopt);

    void DrawArc(const cgmath::Vec2& center, float radius, float beginDeg,
                 float endDeg, const cgmath::Vec4& color, uint32_t slice = 100);

    void DrawCircle(const cgmath::Vec2& center, float radius,
                    const cgmath::Vec4& color, uint32_t slice = 100);

    void FillFan(const cgmath::Vec2& center, float radius, float beginDeg,
                 float endDeg, const cgmath::Vec4& color,
                 const std::optional<CircleSampler>& sampler = std::nullopt,
                 uint32_t slice = 20);

    void FillCircle(const cgmath::Vec2& center, float radius,
                    const cgmath::Vec4& color,
                    const std::optional<CircleSampler>& sampler = std::nullopt,
                    uint32_t slice = 20);

    void DrawTexture(const Texture& texture, const cgmath::Rect& region,
                     const cgmath::Vec2& size, const cgmath::Vec4& color,
                     const cgmath::Vec2& anchor = {}, float z = 0,
                     const cgmath::Mat44& model = cgmath::Mat44::Identity());

    void SetRenderTarget(Texture* texture);

    template <typename Vertices, typename Indices>
    void Draw(gogl::PrimitiveType primitive, Vertices vertices,
              const Indices& indices,
              const cgmath::Mat44& model = cgmath::Mat44::Identity(),
              const Texture* texture = nullptr);

private:
    gogl::Shader* shader_;
    std::unique_ptr<gogl::Shader> textureShader_;
    std::unique_ptr<gogl::Shader> fontShader_;
    std::unique_ptr<gogl::Framebuffer> framebuffer_;
    std::unique_ptr<gogl::RenderBuffer> renderBuffer_;
    std::unique_ptr<gogl::Texture> whiteTexture_;
    std::unique_ptr<gogl::Buffer> vertexBuffer_;
    std::unique_ptr<gogl::Buffer> indicesBuffer_;
    std::unique_ptr<gogl::AttributePointer> attrPtr_;

    std::unique_ptr<gogl::Shader> initShader(std::string_view vert,
                                             std::string_view frag);
    std::unique_ptr<gogl::Texture> initWhiteTexture();
    std::unique_ptr<gogl::Buffer> initVertexBuffer();
    std::unique_ptr<gogl::Buffer> initIndicesBuffer();
    std::unique_ptr<gogl::AttributePointer> initAttrPtr();
};

template <typename Vertices, typename Indices>
void Renderer2D::Draw(gogl::PrimitiveType primitive, Vertices vertices,
                      const Indices& indices, const cgmath::Mat44& model,
                      const Texture* texture) {
    static_assert(std::is_same_v<typename Vertices::value_type, Vertex>);
    static_assert(std::is_same_v<typename Indices::value_type, uint32_t>);
    if (indices.empty()) {
        indicesBuffer_->Unbind();
    } else {
        indicesBuffer_->Bind();
        indicesBuffer_->SetData((void*)indices.data(),
                                sizeof(uint32_t) * indices.size());
    }
    if (!texture) {
        whiteTexture_->Bind();
    } else {
        texture->texture_->Bind();
    }
    vertexBuffer_->Bind();
    vertexBuffer_->SetData((void*)vertices.data(),
                           sizeof(Vertex) * vertices.size());
    attrPtr_->Bind();
    shader_->Use();
    shader_->SetMat4("Model", model);

    if (indices.empty()) {
        shader_->DrawArray(primitive, 0, vertices.size());
    } else {
        shader_->DrawElements(primitive, indices.size(), GL_UNSIGNED_INT, 0);
    }
}

void BeginRenderPipeline(gecs::resource<gecs::mut<Renderer2D>>,
                         gecs::resource<Camera>,
                         gecs::resource<gecs::mut<RenderContext>>);

void EndRenderPipeline(gecs::resource<gecs::mut<Renderer2D>>);

}  // namespace nickel