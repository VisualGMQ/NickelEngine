#pragma once

#include "config/config.hpp"
#include "core/cgmath.hpp"
#include "core/gogl.hpp"
#include "pch.hpp"
#include "renderer/vertex.hpp"

namespace nickel {

namespace internal {

template <typename VertexType>
struct RenderBatch final {
    std::vector<VertexType> vertices;
    std::vector<uint32_t> indices;

    RenderBatch() {
        vertices.reserve(config::MaxVertexPerBatch);
        indices.reserve(config::MaxVertexPerBatch * 4);
    }
};

struct DrawCall final {
    gogl::PrimitiveType primitive = gogl::PrimitiveType::Unknown;
    gogl::Texture* texture = nullptr;
    size_t vertex_start_idx = 0;
    size_t vertex_count = 0;
    std::optional<size_t> index_start_idx;
    size_t index_count = 0;
};

/**
 * help to do batch rendering
 * usage:
 *
 * @code {.cpp}
 * Batch batch;
 * // start to draw a primitive
 * batch.BeginPrimitive(gogl::PrimitiveType::TriangleList, white_texture);
 * // push all vertices
 * batch.PushVertex(Vertex1);
 * ...
 * // if need, push all indices
 * batch.PushIndex(index1);
 * ...
 *
 * // star to draw another primitive
 * batch.BeginPrimitive(gogl::PrimitiveType::Lines, image);
 * // push vertices and indices
 * ...
 *
 * // after record all drawcalls, do `Render()` once to draw them
 * batch.Render(shader, texture_name);
 * @endcode
 *
 */
class Batch final {
public:
    using vertex_type = Vertex;

    Batch();

    /**
     * @brief prepare to draw a kine of primitive
     *
     * @note begin a new primitive or texture different from last will create a
     * new drawcall. The efficient way to use batch rendering is recording the
     * same primitive & texture objects together as much as possible
     */
    void BeginPrimitive(gogl::PrimitiveType, gogl::Texture& texture);

    /**
     * @brief push a vertex into batch
     */
    template <typename... Args>
    void PushVertex(Args&&... args) {
        batch.vertices.emplace_back(std::forward<Args>(args)...);
    }

    /**
     * @brief push a index into batch
     */
    void PushIndex(uint32_t index);

    /**
     * @brief transfer data to GPU and render all drawcalls, then clear all
     * datas & drawcalls
     *
     * @param shader your shader
     * @param texture_uniform_name your texture uniform name in shader
     */
    void Render(gogl::Shader& shader, std::string_view texture_uniform_name);

private:
    RenderBatch<vertex_type> batch;
    std::vector<DrawCall> drawcalls;
    gogl::Buffer vertex_buffer;
    gogl::Buffer indices_buffer;
    gogl::AttributePointer attr_ptr;

    void doOneDrawCall(DrawCall&, gogl::Shader&,
                       std::string_view texture_uniform_name);
};

}  // namespace internal

}  // namespace nickel