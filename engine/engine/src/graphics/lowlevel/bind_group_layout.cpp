#include "nickel/graphics/lowlevel/bind_group_layout.hpp"
#include "nickel/graphics/lowlevel/internal/bind_group_layout_impl.hpp"

namespace nickel::graphics {

BindGroup BindGroupLayout::RequireBindGroup(const BindGroup::Descriptor& desc) {
    return m_impl->RequireBindGroup(desc);
}

}  // namespace nickel::graphics