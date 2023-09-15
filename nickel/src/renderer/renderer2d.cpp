#include "renderer/renderer2d.hpp"

namespace nickel {

Renderer2D::Renderer2D() {
    shader_ = initShader();
    whiteTexture_ = initWhiteTexture();
    vertexBuffer_ = initVertexBuffer();
    vertexBuffer_->Bind();
    indicesBuffer_ = initIndicesBuffer();
    indicesBuffer_->Bind();
    attrPtr_ = initAttrPtr();
    shader_->SetInt("image", 0);

    GL_CALL(glEnable(GL_BLEND));
    GL_CALL(glEnable(GL_MULTISAMPLE));
}

void Renderer2D::SetViewport(const cgmath::Vec2& offset,
                             const cgmath::Vec2& size) {
    GL_CALL(glViewport(
        static_cast<GLsizei>(offset.x), static_cast<GLsizei>(offset.y),
        static_cast<GLsizei>(size.w), static_cast<GLsizei>(size.h)));
    shader_->Use();
    shader_->SetMat4("Project",
                     cgmath::CreateOrtho(0, size.w, 0.0, size.h, -1.0, 1.0));
}

template <typename Vertices, typename Indices>
void Renderer2D::DrawTriangles(Vertices vertices, const Indices& indices,
                               const cgmath::Mat44& model,
                               const std::optional<RectSampler>& sampler) {
    draw(gogl::PrimitiveType::Triangles, vertices, indices, model,
         sampler ? sampler->texture : nullptr);
}

void Renderer2D::DrawRect(const cgmath::Rect& rect, const cgmath::Vec4& color,
                          const cgmath::Mat44& model) {
    const std::array<Vertex, 4> vertices = {
        Vertex::FromPosColor({rect.x, rect.y}, color),
        Vertex::FromPosColor({rect.x + rect.w, rect.y}, color),
        Vertex::FromPosColor({rect.x + rect.w, rect.y + rect.h}, color),
        Vertex::FromPosColor({rect.x, rect.y + rect.h}, color),
    };

    DrawLineLoop(vertices, model);
}

void Renderer2D::FillRect(const cgmath::Rect& rect, const cgmath::Vec4& color,
                          const cgmath::Mat44& model,
                          const std::optional<RectSampler>& image) {
    std::array<Vertex, 4> vertices = {
        Vertex::FromPosColor({rect.x, rect.y}, color),
        Vertex::FromPosColor({rect.x + rect.w, rect.y}, color),
        Vertex::FromPosColor({rect.x + rect.w, rect.y + rect.h}, color),
        Vertex::FromPosColor({rect.x, rect.y + rect.h}, color),
    };
    if (image) {
        const auto& region = image->region;
        vertices[0].texcoord = cgmath::Vec2{region.x, region.y};
        vertices[1].texcoord = cgmath::Vec2{region.x + region.w, region.y};
        vertices[2].texcoord =
            cgmath::Vec2{region.x + region.w, region.y + region.h};
        vertices[3].texcoord = cgmath::Vec2{region.x, region.y + region.h};
    }

    DrawTriangles(vertices, std::array<uint32_t, 6>{0, 1, 2, 0, 2, 3}, model,
                  image);
}

void Renderer2D::DrawArc(const cgmath::Vec2& center, float radius,
                         float beginDeg, float endDeg,
                         const cgmath::Vec4& color, uint32_t slice) {
    std::vector<Vertex> vertices(slice);
    float step = (endDeg - beginDeg) / slice;
    for (uint32_t i = 0; i < slice; i++) {
        float radians = cgmath::Deg2Rad(beginDeg + step * i);
        vertices[i].position =
            cgmath::Vec2{center.x + radius * std::cos(radians),
                         center.y + radius * std::sin(radians)};
        vertices[i].color = color;
    }
    DrawLines(vertices);
}

void Renderer2D::DrawCircle(const cgmath::Vec2& center, float radius,
                            const cgmath::Vec4& color, uint32_t slice) {
    std::vector<Vertex> vertices(slice);
    float step = 2.0f * cgmath::PI / slice;
    for (uint32_t i = 0; i < slice; i++) {
        float radians = step * i;
        vertices[i].position =
            cgmath::Vec2{center.x + radius * std::cos(radians),
                         center.y + radius * std::sin(radians)};
        vertices[i].color = color;
    }
    DrawLineLoop(vertices);
}

void Renderer2D::FillFan(const cgmath::Vec2& center, float radius,
                         float beginDeg, float endDeg,
                         const cgmath::Vec4& color,
                         const std::optional<CircleSampler>& sampler,
                         uint32_t slice) {
    std::vector<Vertex> vertices(slice);
    vertices.push_back(Vertex{
        center, sampler ? sampler->center : cgmath::Vec2{0.0, 0.0},
             color
    });
    float step = (endDeg - beginDeg) / slice;
    for (uint32_t i = 0; i < slice; i++) {
        float radians = cgmath::Deg2Rad(beginDeg + step * i);
        vertices[i].position =
            cgmath::Vec2{center.x + radius * std::cos(radians),
                         center.y + radius * std::sin(radians)};
        vertices[i].color = color;
        if (sampler) {
            cgmath::Vec2 size(static_cast<float>(sampler->texture->Width()),
                              static_cast<float>(sampler->texture->Height()));
            vertices[i].texcoord =
                sampler->center +
                sampler->radius *
                    cgmath::Vec2{std::cos(radians), std::sin(radians)};
        }
    }
    draw(gogl::PrimitiveType::TriangleFan, vertices, std::array<uint32_t, 0>{},
         cgmath::Mat44::Identity(), sampler ? sampler->texture : nullptr);
}

void Renderer2D::FillCircle(const cgmath::Vec2& center, float radius,
                            const cgmath::Vec4& color,
                            const std::optional<CircleSampler>& sampler,
                            uint32_t slice) {
    std::vector<Vertex> vertices(slice);
    vertices.push_back(Vertex{
        center, sampler ? sampler->center : cgmath::Vec2{0.0, 0.0},
             color
    });
    float step = 2.0f * cgmath::PI / slice;
    for (uint32_t i = 0; i < slice; i++) {
        float radians = step * i;
        vertices[i].position =
            cgmath::Vec2{center.x + radius * std::cos(radians),
                         center.y + radius * std::sin(radians)};
        vertices[i].color = color;
        if (sampler) {
            cgmath::Vec2 size(static_cast<float>(sampler->texture->Width()),
                              static_cast<float>(sampler->texture->Height()));
            vertices[i].texcoord =
                sampler->center +
                sampler->radius *
                    cgmath::Vec2{std::cos(radians), std::sin(radians)};
        }
    }
    draw(gogl::PrimitiveType::TriangleFan, vertices, std::array<uint32_t, 0>{},
         cgmath::Mat44::Identity(), sampler ? sampler->texture : nullptr);
}

void Renderer2D::DrawTexture(const Texture& texture, const cgmath::Rect& src,
                             const cgmath::Vec2& size,
                             const cgmath::Vec4& color,
                             const cgmath::Mat44& model) {
    // clang-format off
    std::array<Vertex, 4> vertices = {
        Vertex{     {0, 0},                     {src.x / size.w, src.y / size.h}, color},
        Vertex{{size.w, 0},           {(src.x + src.w) / size.w, src.y / size.h}, color},
        Vertex{{0, size.h},           {src.x / size.w, (src.y + src.h) / size.h}, color},
        Vertex{       size, {(src.x + src.w) / size.w, (src.y + src.h) / size.h}, color},
    };
    // clang-format on
    std::array<uint32_t, 6> indices = {0, 1, 2, 1, 2, 3};
    draw(gogl::PrimitiveType::Triangles, vertices, indices, model,
         texture.texture_.get());
}

std::unique_ptr<gogl::Shader> Renderer2D::initShader() {
    std::ifstream file("shader/vertex.shader");
    if (file.fail()) {
        LOGE(log_tag::Renderer, "read vertex shader failed");
        return nullptr;
    }
    std::string vertex_source((std::istreambuf_iterator<char>(file)),
                              std::istreambuf_iterator<char>());
    gogl::ShaderModule vertexModel(gogl::ShaderModule::Type::Vertex,
                                   vertex_source);

    file.open("shader/frag.shader");
    if (file.fail()) {
        LOGE(log_tag::Renderer, "read vertex shader failed");
        return nullptr;
    }
    std::string frag_source((std::istreambuf_iterator<char>(file)),
                            std::istreambuf_iterator<char>());
    gogl::ShaderModule fragmentModel(gogl::ShaderModule::Type::Fragment,
                                     frag_source);

    return std::make_unique<gogl::Shader>(vertexModel, fragmentModel);
}

std::unique_ptr<gogl::Texture> Renderer2D::initWhiteTexture() {
    uint32_t pixels[] = {0xFFFFFFFF};
    return std::make_unique<gogl::Texture>(
        gogl::Texture::Type::Dimension2, pixels, 1, 1,
        gogl::Sampler::CreateLinearRepeat(), gogl::Format::RGBA,
        gogl::Format::RGBA);
}

std::unique_ptr<gogl::Buffer> Renderer2D::initVertexBuffer() {
    return std::make_unique<gogl::Buffer>(gogl::BufferType::Array);
}

std::unique_ptr<gogl::Buffer> Renderer2D::initIndicesBuffer() {
    return std::make_unique<gogl::Buffer>(gogl::BufferType::Element);
}

std::unique_ptr<gogl::AttributePointer> Renderer2D::initAttrPtr() {
    return std::make_unique<gogl::AttributePointer>(
        gogl::BufferLayout::CreateFromTypes({gogl::Attribute::Type::Vec2,
                                             gogl::Attribute::Type::Vec2,
                                             gogl::Attribute::Type::Vec4}));
}

}  // namespace nickel
