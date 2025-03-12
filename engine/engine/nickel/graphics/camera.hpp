#pragma once
#include "nickel/common/math/math.hpp"

namespace nickel {

struct Frustum {
    float near{}, far{}, aspect = 1.0;
    Radians fov = Degrees{45};
};

class Camera {
public:
    virtual Mat44 GetProject() const = 0;
    virtual void SetProject(const SVector<uint32_t, 2>& window_size,
                            Radians fov, float aspect, float near,
                            float far) const = 0;
    virtual Frustum GetFrustum() const = 0;
    virtual Mat44 GetView() const = 0;
    virtual Vec3 GetPosition() const = 0;
    virtual void ResizeViewArea(
        const SVector<uint32_t, 2>& new_window_size) = 0;
    virtual ~Camera() = default;
};

class FlyCamera : public Camera {
public:
    FlyCamera(const SVector<uint32_t, 2>& window_size, Radians fov,
              float aspect, float near, float far);
    ~FlyCamera();
    void SetProject(const SVector<uint32_t, 2>& window_size, Radians fov,
                    float aspect, float near, float far) const override;
    Frustum GetFrustum() const override;

    void ResizeViewArea(const SVector<uint32_t, 2>& new_window_size);

    void SetYaw(Radians value);
    void SetPitch(Radians value);
    void AddYaw(Radians value);
    void AddPitch(Radians value);
    Radians GetYaw() const;
    Radians GetPitch() const;
    Vec3 GetForward() const;
    Vec3 GetUp() const;
    Quat GetRotation() const;

    void Move(const Vec3& offset);
    void MoveTo(const Vec3& pos);
    void MoveForward(float dist);
    void MoveRightLeft(float dist);
    void MoveUpDown(float dist);
    Vec3 GetPosition() const override;

    Mat44 GetProject() const override;
    Mat44 GetView() const override;

private:
    class Impl;
    std::unique_ptr<Impl> m_impl;
};
}  // namespace nickel
