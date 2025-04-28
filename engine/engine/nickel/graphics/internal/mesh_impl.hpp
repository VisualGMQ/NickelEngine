#pragma once
#include "nickel/common/memory/refcountable.hpp"
#include "nickel/common/transform.hpp"
#include "nickel/graphics/mesh.hpp"

namespace nickel::graphics {
class GLTFModelManagerImpl;

struct MeshImpl : public RefCountable {
    explicit MeshImpl(GLTFModelManagerImpl* mgr);

    void DecRefcount() override;

    std::string m_name;
    std::vector<Primitive> m_primitives;

private:
    GLTFModelManagerImpl* m_mgr{};
};

}  // namespace nickel::graphics