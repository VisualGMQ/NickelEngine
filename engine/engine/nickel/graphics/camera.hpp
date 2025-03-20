#pragma once
#include "nickel/common/math/math.hpp"

namespace nickel {

struct Frustum {
    float near{}, far{}, aspect = 1.0;
    Radians fov = Degrees{45};
};

Frustum ResizeFrustumInNewWindowSize(
    const SVector<uint32_t, 2>& old_window_size,
    const SVector<uint32_t, 2>& new_window_size, const Frustum& frustum);

class Camera {
public:
    void SetProject(Radians fov, float aspect, float near, float far);
    virtual Mat44 GetView() const = 0;
    virtual ~Camera() = default;
    Frustum GetFrustum() const;
    const Mat44& GetProject() const;
    virtual const Vec3& GetPosition() const = 0;

protected:
    Frustum m_frustum;
    Mat44 m_project;
};

class FlyCamera : public Camera {
public:
    FlyCamera(Radians fov, float aspect, float near, float far);

    Mat44 GetView() const override;

    void SetYaw(Radians value);

    void SetPitch(Radians value);

    void AddYaw(Radians value);

    void AddPitch(Radians value);

    Radians GetYaw() const;

    Radians GetPitch() const;

    void Move(const Vec3& offset);

    void MoveTo(const Vec3& pos);

    const Vec3& GetPosition() const override;

    Vec3 GetForward() const;

    Vec3 GetUp() const;

    Quat GetRotation() const;

    void MoveForward(float dist);

    void MoveRightLeft(float dist);

    void MoveUpDown(float dist);

private:
    Radians m_pitch{0};
    Radians m_yaw{0};
    Vec3 m_position;
};

class OrbitCamera : public Camera {
public:
    OrbitCamera(Radians fov, float aspect, float near, float far);

    void SetTheta(Radians);
    void SetPhi(Radians);
    void AddTheta(Radians);
    void AddPhi(Radians);
    void SetRadius(float);
    void AddRadius(float);
    float GetRadius() const;
    Radians GetTheta() const;
    Radians GetPhi() const;
    const Vec3& GetPosition() const override;

    Mat44 GetView() const override;

private:
    Mat44 m_project;
    Vec3 m_target;
    Vec3 m_position;
    float m_radius{10};
    Radians m_theta;  // angle in XoZ
    Radians m_phi;    // angle from Y-Axis
    SVector<uint32_t, 2> m_window_size;
};

}  // namespace nickel
