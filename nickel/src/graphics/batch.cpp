#include "graphics/batch.hpp"

namespace nickel {

namespace internal {

Batch::Batch()
    : vertex_buffer(gogl::BufferType::Array),
      indices_buffer(gogl::BufferType::Element),
      attr_ptr(Vertex::Layout()) {
    drawcalls.reserve(config::MaxDrawCallNum);
}

void Batch::BeginPrimitive(gogl::PrimitiveType primitive, gogl::Texture& texture) {
    DrawCall* drawcall = nullptr;

    if (drawcalls.empty()) {
        drawcall = &drawcalls.emplace_back();
    } else {
        if (primitive == drawcalls.back().primitive && &texture == drawcalls.back().texture) {
            drawcall = &drawcalls.back();
        } else {
            drawcall = &drawcalls.emplace_back();
        }
    }

    drawcall->primitive = primitive;
    drawcall->texture = &texture;
    drawcall->vertex_start_idx = batch.vertices.size();
}

void Batch::PushIndex(uint32_t index) {
    auto& drawcall = drawcalls.back();
    if (!drawcall.index_start_idx) {
        drawcall.index_start_idx = batch.indices.size();
    }
    drawcall.index_count ++;
    batch.indices.push_back(index);
}

void Batch::Render(gogl::Shader& shader, std::string_view texture_uniform_name) {
    vertex_buffer.Bind();
    indices_buffer.Bind();

    vertex_buffer.SetData(batch.vertices.data(), sizeof(decltype(batch.indices)::value_type) * batch.vertices.size());
    indices_buffer.SetData(batch.indices.data(), sizeof(decltype(batch.indices)::value_type) * batch.indices.size());
    attr_ptr.Bind();
    shader.Use();

    for (auto& drawcall : drawcalls) {
        doOneDrawCall(drawcall, shader, texture_uniform_name);
    }

    drawcalls.clear();
    batch.vertices.clear();
    batch.indices.clear();
}

void Batch::doOneDrawCall(DrawCall& drawcall, gogl::Shader& shader, std::string_view texture_uniform_name) {
    drawcall.texture->Bind();
    shader.SetInt(texture_uniform_name, 0);

    if (drawcall.index_start_idx) {
        shader.DrawElements(drawcall.primitive, drawcall.index_count,
                            GL_UNSIGNED_INT, drawcall.index_start_idx.value());
    } else {
        shader.DrawArray(drawcall.primitive, drawcall.vertex_start_idx, drawcall.vertex_count);
    }
}

}  // namespace internal

}  // namespace nickel