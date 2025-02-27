#include "nickel/graphics/internal/material3d_impl.hpp"

#include "nickel/graphics/internal/gltf_manager_impl.hpp"

namespace nickel::graphics {

Material3DImpl::Material3DImpl(GLTFManagerImpl* mgr) : m_mgr{mgr} {}

void Material3DImpl::DecRefcount() {
    RefCountable::DecRefcount();

    if (Refcount() == 0) {
        m_mgr->m_mtl_allocator.MarkAsGarbage(this);
    }
}

}  // namespace nickel::graphics