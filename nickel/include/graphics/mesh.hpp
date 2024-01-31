#pragma once

#include "stdpch.hpp"
#include "graphics/gogl.hpp"
#include "graphics/vertex.hpp"

namespace nickel {

struct GPUDatas final {
    std::unique_ptr<gogl::Buffer> vertBuf_;
    std::unique_ptr<gogl::Buffer> indexBuf_;
    std::unique_ptr<gogl::AttributePointer> attr_;
};

struct LocalDatas {
    std::vector<Vertex> vertices;
    std::vector<uint32_t> indices;
};

class Mesh final {
public:
    auto& LocalData() const { return local_; }

    auto& LocalData() { return local_; }

    explicit Mesh(const std::vector<Vertex>& vertices,
                  const std::vector<uint32_t>& indices)
        : local_{vertices, indices} {
        gpu_.vertBuf_ = std::make_unique<gogl::Buffer>(gogl::BufferType::Array);
        gpu_.indexBuf_ =
            std::make_unique<gogl::Buffer>(gogl::BufferType::Element);
        gpu_.attr_ = std::make_unique<gogl::AttributePointer>(Vertex::Layout());

        Upload2GPU();
    }

    void Upload2GPU() const {
        gpu_.vertBuf_->SetData(local_.vertices.data(),
                               local_.vertices.size() * sizeof(Vertex));
        gpu_.indexBuf_->SetData(local_.indices.data(),
                                local_.indices.size() * sizeof(uint32_t));
    }

    void Bind2GPU() const {
        gpu_.attr_->Bind();
        gpu_.vertBuf_->Bind();
        gpu_.indexBuf_->Bind();
    }

    void Unbind2GPU() const {
        gpu_.attr_->Unbind();
        gpu_.vertBuf_->Unbind();
        gpu_.indexBuf_->Unbind();
    }

private:
    GPUDatas gpu_;
    LocalDatas local_;
};

}  // namespace nickel