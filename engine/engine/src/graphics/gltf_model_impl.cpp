#include "nickel/graphics/internal/gltf_model_impl.hpp"

namespace nickel::graphics {

GLTFModelImpl::~GLTFModelImpl() {
    materials.clear();
    m_mtl_allocator.FreeAll();
}

}  // namespace nickel::graphics