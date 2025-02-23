#pragma once
#include "nickel/common/math/math.hpp"
#include "nickel/common/transform.hpp"
#include "nickel/graphics/material.hpp"

namespace nickel::graphics {
struct Primitive final {
    BufferView m_pos_buf_view;
    BufferView m_norm_buf_view;
    BufferView m_tan_buf_view;
    BufferView m_uv_buf_view;
    BufferView m_color_buf_view;
    BufferView m_indices_buf_view;
    IndexType m_index_type;
    std::optional<uint32_t> m_material;
};

struct GPUMesh final {
    friend class GLTFLoader;
    friend class Scene;
    
    std::string m_name;
    Transform m_transform;
    std::vector<Primitive> m_primitives;
    std::vector<std::unique_ptr<GPUMesh>> m_children;

    Mat44 GetModelMat() const;
    
private:
    Transform m_global_transform;
};

struct Scene final {
    std::vector<std::unique_ptr<GPUMesh>> m_nodes;

    void updateTransform(const Transform&);

private:
    void preorderGPUMesh(const Transform& parent_transform, GPUMesh& mesh);
};
}
