#pragma once
#include "nickel/graphics/material.hpp"

namespace nickel::graphics {
struct Primitive final {
    BufferView m_pos_buf_view;
    BufferView m_norm_buf_view;
    BufferView m_tan_buf_view;
    BufferView m_uv_buf_view;
    BufferView m_indices_buf_view;
    BufferView m_bone_indices_buf_view;
    BufferView m_weight_buf_view;
    IndexType m_index_type;
    Material3D m_material;
};

struct MeshImpl;

struct Mesh final : public ImplWrapper<MeshImpl> {
    using ImplWrapper::ImplWrapper;
};

}  // namespace nickel::graphics
