#include "nickel/graphics/camera.hpp"

namespace nickel {
class FlyCamera::Impl {
public:
    Impl(Radians fov, float aspect, float near, float far) {
        m_project = CreatePersp(fov, aspect, near, far);
    }

    void SetProject(Radians fov, float aspect, float near, float far) {
        m_project = CreatePersp(fov, aspect, near, far);
    }

    Mat44 GetProject() const { return m_project; }

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

    void MoveForward(float dist) {
        auto forward_dir = Quat::Create(Vec3::UNIT_Y, m_yaw) * Quat::Create(
                               Vec3::UNIT_X, m_pitch) * -Vec3::UNIT_Z;
        m_position += forward_dir * dist;
    }

    void MoveRightLeft(float dist) {
        auto right_dir = Quat::Create(Vec3::UNIT_Y, m_yaw) * Quat::Create(
                             Vec3::UNIT_X, m_pitch) * Vec3::UNIT_X;
        m_position += right_dir * dist;
    }

    void MoveUpDown(float dist) {
        auto up_dir = Quat::Create(Vec3::UNIT_Y, m_yaw) * Quat::Create(
                          Vec3::UNIT_X, m_pitch) * Vec3::UNIT_Y;
        m_position += up_dir * dist;
    }

private:
    Mat44 m_project;
    Radians m_pitch{0};
    Radians m_yaw{0};
    Vec3 m_position;
};

FlyCamera::FlyCamera(Radians fov, float aspect, float near, float far)
    : m_impl{std::make_unique<Impl>(fov, aspect, near, far)} {
}

void FlyCamera::SetProject(Radians fov, float aspect, float near,
                           float far) {
    m_impl->SetProject(fov, aspect, near, far);
}

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

const Vec3& FlyCamera::GetPosition() const {
    return m_impl->GetPosition();
}

Mat44 FlyCamera::GetProject() const {
    return m_impl->GetProject();
}

Mat44 FlyCamera::GetView() const {
    return m_impl->GetView();
}
}
