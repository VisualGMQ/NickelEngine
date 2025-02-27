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

struct MeshImpl;

struct Mesh final : public ImplWrapper<MeshImpl> {
    using ImplWrapper::ImplWrapper;
};

}  // namespace nickel::graphics
