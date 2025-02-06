#pragma once
#include "nickel/common/math/math.hpp"

namespace nickel {
class Camera {
public:
    virtual Mat44 GetProject() const = 0;
    virtual Mat44 GetView() const = 0;
    virtual ~Camera() = default;
};

class FlyCamera : public Camera {
public:
    FlyCamera(Radians fov, float aspect, float near, float far);
    ~FlyCamera();
    void SetProject(Radians fov, float aspect, float near, float far);

    void SetYaw(Radians value);
    void SetPitch(Radians value);
    void AddYaw(Radians value);
    void AddPitch(Radians value);
    Radians GetYaw() const;
    Radians GetPitch() const;

    void Move(const Vec3& offset);
    void MoveTo(const Vec3& pos);
    void MoveForward(float dist);
    void MoveRightLeft(float dist);
    void MoveUpDown(float dist);
    const Vec3& GetPosition() const;

    Mat44 GetProject() const override;
    Mat44 GetView() const override;

private:
    class Impl;
    std::unique_ptr<Impl> m_impl;
};
}
