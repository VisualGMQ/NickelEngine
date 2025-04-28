#include "nickel/graphics/internal/mesh_impl.hpp"
#include "nickel/graphics/internal/gltf_manager_impl.hpp"

namespace nickel::graphics {

MeshImpl::MeshImpl(GLTFModelManagerImpl* mgr) : m_mgr{mgr} {}

void MeshImpl::DecRefcount() {
    RefCountable::DecRefcount();

    if (Refcount() == 0) {
        m_mgr->m_mesh_allocator.MarkAsGarbage(this);
    }
}
}  // namespace nickel::graphics