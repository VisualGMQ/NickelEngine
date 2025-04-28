#include "nickel/graphics/internal/gltf_model_impl.hpp"

#include "nickel/graphics/internal/gltf_manager_impl.hpp"
#include "nickel/graphics/internal/material3d_impl.hpp"

namespace nickel::graphics {

GLTFModelResourceImpl::GLTFModelResourceImpl(GLTFModelManagerImpl* mgr)
    : m_mgr{mgr} {}

void GLTFModelResourceImpl::DecRefcount() {
    RefCountable::DecRefcount();

    if (Refcount() == 0) {
        m_mgr->m_model_resource_allocator.MarkAsGarbage(this);
    }
}

GLTFModelImpl::GLTFModelImpl(GLTFModelManagerImpl* mgr) : m_mgr{mgr} {}

void GLTFModelImpl::DecRefcount() {
    RefCountable::DecRefcount();

    if (Refcount() == 0) {
        m_mgr->m_model_allocator.MarkAsGarbage(this);
        m_mgr->Remove(*this);
    }
}

}  // namespace nickel::graphics