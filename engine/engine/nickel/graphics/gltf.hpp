#pragma once
#include "nickel/common/impl_wrapper.hpp"
#include "nickel/common/math/math.hpp"
#include "nickel/fs/path.hpp"

namespace nickel::graphics {

class GLTFModelImpl;

struct GLTFModel final: public ImplWrapper<GLTFModelImpl> {
    using ImplWrapper::ImplWrapper;
    
    void Move(const Vec3& offset);
    void Scale(const Vec3& delta);
    void Rotate(const Quat& delta);

    void MoveTo(const Vec3& position);
    void ScaleTo(const Vec3& scale);
    void RotateTo(const Quat& q);

    void UpdateTransform();
};

class GLTFManager {
public:
    GLTFManager();
    ~GLTFManager();

    GLTFModel Load(const Path&);
    void Update();
    void GC();

private:
    class Impl;
    std::unique_ptr<Impl> m_impl;
};

}  // namespace nickel::graphics
