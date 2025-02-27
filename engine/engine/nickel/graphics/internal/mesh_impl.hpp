#pragma once
#include "nickel/common/memory/refcountable.hpp"
#include "nickel/common/transform.hpp"
#include "nickel/graphics/mesh.hpp"

namespace nickel::graphics {
class GLTFManagerImpl;

struct MeshImpl : public RefCountable {
    explicit MeshImpl(GLTFManagerImpl* mgr);

    void DecRefcount() override;

    std::string m_name;
    std::vector<Primitive> m_primitives;

private:
    GLTFManagerImpl* m_mgr{};
};

}  // namespace nickel::graphics