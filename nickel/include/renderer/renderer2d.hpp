#pragma once

#include "core/cgmath.hpp"
#include "core/gogl.hpp"
#include <iterator>
#include <optional>

namespace nickel {

struct Vertex final {
    cgmath::Vec2 position;
    cgmath::Vec2 texcoord;
    cgmath::Vec4 color;

    static Vertex FromPosition(const cgmath::Vec2& position) {
        return {position};
    }

    static Vertex FromPosColor(const cgmath::Vec2& position,
                               const cgmath::Vec4& color) {
        return {position, {}, color};
    }
};

struct RectSampler final {
    gogl::Texture* texture = nullptr;
    cgmath::Rect region = {0.0, 0.0, 1.0, 1.0};
};

struct CircleSampler final {
    gogl::Texture* texture = nullptr;
    float radius = 0.5;
    cgmath::Vec2 center = {0.5, 0.5};
};

class Renderer2D {
public:
    virtual ~Renderer2D() = default;

    Renderer2D();

    void SetViewport(const cgmath::Vec2& offset, const cgmath::Vec2& size);

    void SetLineWidth(int width) { GL_CALL(glLineWidth(width)); }

    void SetClearColor(const cgmath::Vec4& color) {
        GL_CALL(glClearColor(color.x, color.y, color.z, color.w));
    }

    void Clear() { GL_CALL(glClear(GL_COLOR_BUFFER_BIT)); }

    template <typename Vertices>
    void DrawLines(const Vertices& vertices) {
        draw(gogl::PrimitiveType::LineStrip, vertices,
             std::array<uint32_t, 0>{});
    }

    template <typename Vertices>
    void DrawLineLoop(const Vertices& vertices,
                      const cgmath::Mat44& model = cgmath::Mat44::Identity()) {
        draw(gogl::PrimitiveType::LineLoop, vertices,
             std::array<uint32_t, 0>{});
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

private:
    std::unique_ptr<gogl::Shader> shader_;
    std::unique_ptr<gogl::Texture> whiteTexture_;
    std::unique_ptr<gogl::Buffer> vertexBuffer_;
    std::unique_ptr<gogl::Buffer> indicesBuffer_;
    std::unique_ptr<gogl::AttributePointer> attrPtr_;

    template <typename Vertices, typename Indices>
    void draw(gogl::PrimitiveType primitive, Vertices vertices,
              const Indices& indices,
              const cgmath::Mat44& model = cgmath::Mat44::Identity(),
              gogl::Texture* texture = nullptr);

    std::unique_ptr<gogl::Shader> initShader();
    std::unique_ptr<gogl::Texture> initWhiteTexture();
    std::unique_ptr<gogl::Buffer> initVertexBuffer();
    std::unique_ptr<gogl::Buffer> initIndicesBuffer();
    std::unique_ptr<gogl::AttributePointer> initAttrPtr();
};

template <typename Vertices, typename Indices>
void Renderer2D::draw(gogl::PrimitiveType primitive, Vertices vertices,
                      const Indices& indices, const cgmath::Mat44& model,
                      gogl::Texture* texture) {
    static_assert(std::is_same_v<Vertices::value_type, Vertex>);
    static_assert(std::is_same_v<Indices::value_type, uint32_t>);
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
        texture->Bind();
        int w = texture->Width();
        int h = texture->Height();
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

}  // namespace nickel