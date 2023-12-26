#include "renderer/renderer2d.hpp"

namespace nickel {

Renderer2D::Renderer2D() {
    textureShader_ =
        initShader("nickel/shader/vertex.shader", "nickel/shader/frag.shader");
    fontShader_ = initShader("nickel/shader/font_vertex.shader",
                             "nickel/shader/font_frag.shader");
    whiteTexture_ = initWhiteTexture();
    vertexBuffer_ = initVertexBuffer();
    vertexBuffer_->Bind();
    indicesBuffer_ = initIndicesBuffer();
    indicesBuffer_->Bind();
    attrPtr_ = initAttrPtr();
    textureShader_->SetInt("image", 0);
    shader_ = textureShader_.get();
}

void Renderer2D::SetViewport(const cgmath::Vec2& offset,
                             const cgmath::Vec2& size) {
    GL_CALL(glViewport(
        static_cast<GLsizei>(offset.x), static_cast<GLsizei>(offset.y),
        static_cast<GLsizei>(size.w), static_cast<GLsizei>(size.h)));
}

void Renderer2D::BeginRenderTexture(Camera& camera) {
    camera.ApplyRenderTarget();
    shader_ = textureShader_.get();
    shader_->Use();
    shader_->SetMat4("Project", camera.Project());
    shader_->SetMat4("View", camera.View());
}

void Renderer2D::BeginRenderFont(Camera& camera) {
    camera.ApplyRenderTarget();
    shader_ = fontShader_.get();
    shader_->Use();
    shader_->SetMat4("Project", camera.Project());
    shader_->SetMat4("View", camera.View());
}

void Renderer2D::EndRender() {
    GL_CALL(glBindFramebuffer(GL_FRAMEBUFFER, 0));
}

template <typename Vertices, typename Indices>
void Renderer2D::DrawTriangles(Vertices vertices, const Indices& indices,
                               const cgmath::Mat44& model,
                               const std::optional<RectSampler>& sampler) {
    Draw(gogl::PrimitiveType::Triangles, vertices, indices, model,
         sampler ? sampler->texture : nullptr);
}

void Renderer2D::DrawRect(const cgmath::Rect& rect, const cgmath::Vec4& color,
                          const cgmath::Mat44& model) {
    const std::array<Vertex, 4> vertices = {
        Vertex::FromPosColor({rect.position.x, rect.position.y}, color),
        Vertex::FromPosColor({rect.position.x + rect.size.w, rect.position.y},
                             color),
        Vertex::FromPosColor(
            {rect.position.x + rect.size.w, rect.position.y + rect.size.h},
            color),
        Vertex::FromPosColor({rect.position.x, rect.position.y + rect.size.h},
                             color),
    };

    DrawLineLoop(vertices, model);
}

void Renderer2D::FillRect(const cgmath::Rect& rect, const cgmath::Vec4& color,
                          const cgmath::Mat44& model,
                          const std::optional<RectSampler>& image) {
    std::array<Vertex, 4> vertices = {
        Vertex::FromPosColor({rect.position.x, rect.position.y}, color),
        Vertex::FromPosColor({rect.position.x + rect.size.w, rect.position.y},
                             color),
        Vertex::FromPosColor(
            {rect.position.x + rect.size.w, rect.position.y + rect.size.h},
            color),
        Vertex::FromPosColor({rect.position.x, rect.position.y + rect.size.h},
                             color),
    };
    if (image) {
        const auto& region = image->region;
        vertices[0].texcoord = region.position;
        vertices[1].texcoord =
            cgmath::Vec2{region.position.x + region.size.w, region.position.y};
        vertices[2].texcoord = region.position + region.size;
        vertices[3].texcoord =
            cgmath::Vec2{region.position.x, region.position.y + region.size.h};
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
            cgmath::Vec3{center.x + radius * std::cos(radians),
                         center.y + radius * std::sin(radians), 0};
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
            cgmath::Vec3{center.x + radius * std::cos(radians),
                         center.y + radius * std::sin(radians), 0};
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
        cgmath::Vec3{center.x, center.y, 0},
        sampler ? sampler->center : cgmath::Vec2{},
        color
    });
    float step = (endDeg - beginDeg) / slice;
    for (uint32_t i = 0; i < slice; i++) {
        float radians = cgmath::Deg2Rad(beginDeg + step * i);
        vertices[i].position =
            cgmath::Vec3{center.x + radius * std::cos(radians),
                         center.y + radius * std::sin(radians), 0};
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
    Draw(gogl::PrimitiveType::TriangleFan, vertices, std::array<uint32_t, 0>{},
         cgmath::Mat44::Identity(), sampler ? sampler->texture : nullptr);
}

void Renderer2D::FillCircle(const cgmath::Vec2& center, float radius,
                            const cgmath::Vec4& color,
                            const std::optional<CircleSampler>& sampler,
                            uint32_t slice) {
    std::vector<Vertex> vertices(slice);
    vertices.push_back(Vertex{
        cgmath::Vec3{center.x, center.y, 0},
        sampler ? sampler->center : cgmath::Vec2{},
        color
    });
    float step = 2.0f * cgmath::PI / slice;
    for (uint32_t i = 0; i < slice; i++) {
        float radians = step * i;
        vertices[i].position =
            cgmath::Vec3{center.x + radius * std::cos(radians),
                         center.y + radius * std::sin(radians), 0};
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
    Draw(gogl::PrimitiveType::TriangleFan, vertices, std::array<uint32_t, 0>{},
         cgmath::Mat44::Identity(), sampler ? sampler->texture : nullptr);
}

void Renderer2D::DrawTexture(const Texture& texture, const cgmath::Rect& src,
                             const cgmath::Vec2& size,
                             const cgmath::Vec4& color,
                             const cgmath::Vec2& anchor, float z,
                             const cgmath::Mat44& model) {
    // clang-format off
    std::array<Vertex, 4> vertices = {
        Vertex{{0, 0, z}, {src.position.x / size.w, src.position.y / size.h},               color},
        Vertex{{1, 0, z}, {(src.position.x + src.size.w) / size.w, src.position.y / size.h}, color},
        Vertex{{0, 1, z}, {src.position.x / size.w, (src.position.y + src.size.h) / size.h}, color},
        Vertex{{1, 1, z}, {(src.position.x + src.size.w) / size.w, (src.position.y + src.size.h) / size.h}, color},
    };
    // clang-format on
    std::array<uint32_t, 6> indices = {0, 1, 2, 1, 2, 3};
    Draw(gogl::PrimitiveType::Triangles, vertices, indices,
         model * cgmath::CreateScale({size.x, size.y, 1.0}) *
             cgmath::CreateTranslation({-anchor.x, -anchor.y, 0.0}),
         &texture);
}

std::unique_ptr<gogl::Shader> Renderer2D::initShader(std::string_view vert,
                                                     std::string_view frag) {
    std::ifstream file(vert.data());
    if (file.fail()) {
        LOGE(log_tag::Renderer, "read vertex shader ", vert, " failed");
        return nullptr;
    }
    std::string vertex_source((std::istreambuf_iterator<char>(file)),
                              std::istreambuf_iterator<char>());
    gogl::ShaderModule vertexModel(gogl::ShaderModule::Type::Vertex,
                                   vertex_source);

    file.close();
    file.open(frag.data());
    if (file.fail()) {
        LOGE(log_tag::Renderer, "read fragment shader ", frag, " failed");
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
        gogl::Format::RGBA, gogl::DataType::UByte);
}

std::unique_ptr<gogl::Buffer> Renderer2D::initVertexBuffer() {
    return std::make_unique<gogl::Buffer>(gogl::BufferType::Array);
}

std::unique_ptr<gogl::Buffer> Renderer2D::initIndicesBuffer() {
    return std::make_unique<gogl::Buffer>(gogl::BufferType::Element);
}

std::unique_ptr<gogl::AttributePointer> Renderer2D::initAttrPtr() {
    return std::make_unique<gogl::AttributePointer>(Vertex::Layout());
}

void Renderer2D::SetRenderTarget(Texture* texture) {
    if (!texture) {
        GL_CALL(glBindFramebuffer(GL_FRAMEBUFFER, 0));
        return;
    }

    if (!renderBuffer_ || (renderBuffer_->Width() != texture->Width() ||
                           renderBuffer_->Height() != texture->Height())) {
        renderBuffer_ = std::make_unique<gogl::RenderBuffer>(texture->Width(),
                                                             texture->Height());
    }

    if (!framebuffer_) {
        framebuffer_ = std::make_unique<gogl::Framebuffer>(
            gogl::FramebufferAccess::ReadDraw);
        framebuffer_->Bind();
        framebuffer_->AttachColorTexture2D(*texture->texture_);
        framebuffer_->AttacheRenderBuffer(*renderBuffer_);
        if (!framebuffer_->CheckValid()) {
            LOGE(log_tag::Renderer,
                 "set render target failed: framebuffer incomplete");
            framebuffer_->Unbind();
        }
    }
}

void BeginRenderPipeline(gecs::resource<gecs::mut<Renderer2D>> renderer,
                        gecs::resource<gecs::mut<Camera>> camera,
                         gecs::resource<gecs::mut<RenderContext>> ctx) {
    GL_CALL(glEnable(GL_MULTISAMPLE));
    GL_CALL(glEnable(GL_BLEND));
    GL_CALL(glBlendFunc(GL_SRC_ALPHA, GL_ONE_MINUS_SRC_ALPHA));
    renderer->Clear(camera.get());
    renderer->ClearDepth(camera.get());
    ctx->ClearRenderInfo();
    ctx->ResetBias();
}

void EndRenderPipeline(gecs::resource<gecs::mut<Renderer2D>> renderer) {
}

}  // namespace nickel
