#include "nickel/graphics/lowlevel/bind_group.hpp"
#include "nickel/graphics/lowlevel/internal/bind_group_impl.hpp"

namespace nickel::graphics {

const BindGroup::Descriptor& BindGroup::GetDescriptor() const {
    return m_impl->GetDescriptor();
}

}  // namespace nickel::graphics