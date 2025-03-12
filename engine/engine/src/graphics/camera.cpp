#include "nickel/graphics/camera.hpp"

#include "nickel/graphics/lowlevel/buffer.hpp"

namespace nickel {

class FlyCamera::Impl {
public:
    Impl(const SVector<uint32_t, 2>& window_size, Radians fov, float aspect,
         float near, float far) {
        SetProject(window_size, fov, aspect, near, far);
    }

    void SetProject(const SVector<uint32_t, 2>& window_size, Radians fov,
                    float aspect, float near, float far) {
        m_window_size = window_size;
        m_project = CreatePersp(fov, aspect, near, far);
        m_frustum.near = near;
        m_frustum.far = far;
        m_frustum.fov = fov;
        m_frustum.aspect = aspect;
    }

    void ResizeViewArea(const SVector<uint32_t, 2>& new_window_size) {
        float new_aspect = new_window_size.w / (float)new_window_size.h;
        float fov_height =
            m_frustum.near * std::tan((m_frustum.fov * 0.5f).Value());
        float new_fov_height = fov_height * new_window_size.h / m_window_size.h;
        float new_fov = std::atan(new_fov_height / m_frustum.near) * 2;
        SetProject(new_window_size, new_fov, new_aspect, m_frustum.near,
                   m_frustum.far);
    }

    Mat44 GetProject() const { return m_project; }

    Frustum GetFrustum() const { return m_frustum; }

    Mat44 GetView() const {
        return CreateXRotation(-m_pitch) * CreateYRotation(-m_yaw) *
               CreateTranslation(-m_position);
    }

    void SetYaw(Radians value) { m_yaw = value; }

    void SetPitch(Radians value) {
        m_pitch =
            Clamp(value, Radians{-PI * 0.5 + 0.001}, Radians{PI * 0.5 - 0.001});
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

    Vec3 GetForward() const { return GetRotation() * -Vec3::UNIT_Z; }

    Vec3 GetUp() const { return GetRotation() * Vec3::UNIT_Y; }

    Quat GetRotation() const {
        return Quat::Create(Vec3::UNIT_Y, m_yaw) *
               Quat::Create(Vec3::UNIT_X, m_pitch);
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
    SVector<uint32_t, 2> m_window_size;
};

FlyCamera::FlyCamera(const SVector<uint32_t, 2>& window_size, Radians fov,
                     float aspect, float near, float far)
    : m_impl{std::make_unique<Impl>(window_size, fov, aspect, near, far)} {}

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

Quat FlyCamera::GetRotation() const {
    return m_impl->GetRotation();
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

void FlyCamera::SetProject(const SVector<uint32_t, 2>& window_size, Radians fov,
                           float aspect, float near, float far) const {
    m_impl->SetProject(window_size, fov, aspect, near, far);
}

Frustum FlyCamera::GetFrustum() const {
    return m_impl->GetFrustum();
}

void FlyCamera::ResizeViewArea(const SVector<uint32_t, 2>& new_window_size) {
    return m_impl->ResizeViewArea(new_window_size);
}

}  // namespace nickel
