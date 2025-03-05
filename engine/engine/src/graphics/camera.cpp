#include "nickel/graphics/camera.hpp"

#include "nickel/graphics/lowlevel/buffer.hpp"

namespace nickel {

class FlyCamera::Impl {
public:
    Impl(Radians fov, float aspect, float near, float far) {
        SetProject(fov, aspect, near, far);
    }

    void SetProject(Radians fov, float aspect, float near, float far) {
        m_project = CreatePersp(fov, aspect, near, far);
        m_frustum.near = near;
        m_frustum.far = far;
        m_frustum.fov = fov;
        m_frustum.aspect = aspect;
    }

    Mat44 GetProject() const { return m_project; }

    Frustum GetFrustum() const { return m_frustum; }

    Mat44 GetView() const {
        return CreateXRotation(-m_pitch) * CreateYRotation(-m_yaw) * CreateTranslation(-m_position);
    }

    void SetYaw(Radians value) { m_yaw = value; }

    void SetPitch(Radians value) {
        m_pitch = Clamp(value, Radians{-PI * 0.5 + 0.001},
                        Radians{PI * 0.5 - 0.001});
    }
    void AddYaw(Radians value) { m_yaw += value; }

    void AddPitch(Radians value) {
        m_pitch = Clamp(m_pitch + value, Radians{-PI * 0.5 + 0.001},
                        Radians{PI * 0.5 - 0.001});
    }
    Radians GetYaw() const { return m_yaw; }
    Radians GetPitch() const { return m_pitch; }

    void Move(const Vec3& offset) { m_position += offset; }
    void MoveTo(const Vec3& pos) { m_position = pos; }
    const Vec3& GetPosition() const { return m_position; }

    Vec3 GetForward() const {
        return Quat::Create(Vec3::UNIT_Y, m_yaw) * Quat::Create(
                               Vec3::UNIT_X, m_pitch) * -Vec3::UNIT_Z;
    }

    Vec3 GetUp() const {
        return Quat::Create(Vec3::UNIT_Y, m_yaw) *
               Quat::Create(Vec3::UNIT_X, m_pitch) * Vec3::UNIT_Y;
    }

    void MoveForward(float dist) { m_position += GetForward() * dist; }

    void MoveRightLeft(float dist) {
        auto right_dir = Quat::Create(Vec3::UNIT_Y, m_yaw) *
                         Quat::Create(Vec3::UNIT_X, m_pitch) * Vec3::UNIT_X;
        m_position += right_dir * dist;
    }

    void MoveUpDown(float dist) { m_position += GetUp() * dist; }

private:
    Mat44 m_project;
    Radians m_pitch{0};
    Radians m_yaw{0};
    Vec3 m_position;
    Frustum m_frustum;
};

FlyCamera::FlyCamera(Radians fov, float aspect, float near, float far)
    : m_impl{std::make_unique<Impl>(fov, aspect, near, far)} {}

void FlyCamera::SetYaw(Radians value) {
    m_impl->SetYaw(value);
}

void FlyCamera::SetPitch(Radians value) {
    m_impl->SetPitch(value);
}

void FlyCamera::AddYaw(Radians value) {
    m_impl->AddYaw(value);
}

void FlyCamera::AddPitch(Radians value) {
    m_impl->AddPitch(value);
}

Radians FlyCamera::GetYaw() const {
    return m_impl->GetYaw();
}

Radians FlyCamera::GetPitch() const {
    return m_impl->GetPitch();
}

Vec3 FlyCamera::GetForward() const {
    return m_impl->GetForward();
}

Vec3 FlyCamera::GetUp() const {
    return m_impl->GetUp();
}

void FlyCamera::Move(const Vec3& offset) {
    m_impl->Move(offset);
}

void FlyCamera::MoveTo(const Vec3& pos) {
    m_impl->MoveTo(pos);
}

void FlyCamera::MoveForward(float dist) {
    return m_impl->MoveForward(dist);
}

void FlyCamera::MoveRightLeft(float dist) {
    return m_impl->MoveRightLeft(dist);
}

void FlyCamera::MoveUpDown(float dist) {
    return m_impl->MoveUpDown(dist);
}

Vec3 FlyCamera::GetPosition() const {
    return m_impl->GetPosition();
}

Mat44 FlyCamera::GetProject() const {
    return m_impl->GetProject();
}

Mat44 FlyCamera::GetView() const {
    return m_impl->GetView();
}

FlyCamera::~FlyCamera() {}

void FlyCamera::SetProject(Radians fov, float aspect, float near,
                           float far) const {
    m_impl->SetProject(fov, aspect, near, far);
}

Frustum FlyCamera::GetFrustum() const {
    return m_impl->GetFrustum();
}

}  // namespace nickel
