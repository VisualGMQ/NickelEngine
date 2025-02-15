#pragma once
#include "nickel/common/math/math.hpp"
#include "nickel/common/memory/memory.hpp"
#include "nickel/fs/path.hpp"

namespace nickel::graphics {

class GLTFModelImpl;

struct GLTFModel final {
    GLTFModel() = default;
    GLTFModel(GLTFModelImpl* impl);
    GLTFModel(const GLTFModel&) noexcept;
    GLTFModel(GLTFModel&&) noexcept;
    GLTFModel& operator=(const GLTFModel&) noexcept;
    GLTFModel& operator=(GLTFModel&&) noexcept;
    ~GLTFModel();
    
    operator bool() const;

    void Move(const Vec3& offset);
    void Scale(const Vec3& delta);
    void Rotate(const Quat& delta);

    void MoveTo(const Vec3& position);
    void ScaleTo(const Vec3& scale);
    void RotateTo(const Quat& q);

    void UpdateTransform();

    const GLTFModelImpl* GetImpl() const;
    GLTFModelImpl* GetImpl();

private:
    GLTFModelImpl* m_impl{};
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
