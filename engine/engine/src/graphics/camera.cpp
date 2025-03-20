#include "nickel/graphics/camera.hpp"

#include "nickel/graphics/lowlevel/buffer.hpp"

namespace nickel {

Frustum ResizeFrustumInNewWindowSize(
    const SVector<uint32_t, 2>& old_window_size,
    const SVector<uint32_t, 2>& new_window_size, const Frustum& frustum) {
    Frustum new_frustum;
    float new_aspect = new_window_size.w / (float)new_window_size.h;
    float fov_height = frustum.near * std::tan((frustum.fov * 0.5f).Value());
    float new_fov_height = fov_height * new_window_size.h / old_window_size.h;
    float new_fov = std::atan(new_fov_height / frustum.near) * 2;

    new_frustum.fov = new_fov;
    new_frustum.aspect = new_aspect;
    new_frustum.near = frustum.near;
    new_frustum.far = frustum.far;
    return new_frustum;
}

Frustum Camera::GetFrustum() const {
    return m_frustum;
}

const Mat44& Camera::GetProject() const {
    return m_project;
}

void Camera::SetProject(Radians fov, float aspect, float near, float far) {
    m_project = CreatePersp(fov, aspect, near, far);
    m_frustum.near = near;
    m_frustum.far = far;
    m_frustum.fov = fov;
    m_frustum.aspect = aspect;
}

FlyCamera::FlyCamera(Radians fov, float aspect, float near, float far) {
    SetProject(fov, aspect, near, far);
}

Mat44 FlyCamera::GetView() const {
    return CreateXRotation(-m_pitch) * CreateYRotation(-m_yaw) *
           CreateTranslation(-m_position);
}

void FlyCamera::SetYaw(Radians value) {
    m_yaw = value;
}

void FlyCamera::SetPitch(Radians value) {
    m_pitch =
        Clamp(value, Radians{-PI * 0.5 + 0.001}, Radians{PI * 0.5 - 0.001});
}

void FlyCamera::AddYaw(Radians value) {
    m_yaw += value;
}

void FlyCamera::AddPitch(Radians value) {
    m_pitch = Clamp(m_pitch + value, Radians{-PI * 0.5 + 0.001},
                    Radians{PI * 0.5 - 0.001});
}

Radians FlyCamera::GetYaw() const {
    return m_yaw;
}

Radians FlyCamera::GetPitch() const {
    return m_pitch;
}

void FlyCamera::Move(const Vec3& offset) {
    m_position += offset;
}

void FlyCamera::MoveTo(const Vec3& pos) {
    m_position = pos;
}

const Vec3& FlyCamera::GetPosition() const {
    return m_position;
}

Vec3 FlyCamera::GetForward() const {
    return GetRotation() * -Vec3::UNIT_Z;
}

Vec3 FlyCamera::GetUp() const {
    return GetRotation() * Vec3::UNIT_Y;
}

Quat FlyCamera::GetRotation() const {
    return Quat::Create(Vec3::UNIT_Y, m_yaw) *
           Quat::Create(Vec3::UNIT_X, m_pitch);
}

void FlyCamera::MoveForward(float dist) {
    m_position += GetForward() * dist;
}

void FlyCamera::MoveRightLeft(float dist) {
    auto right_dir = Quat::Create(Vec3::UNIT_Y, m_yaw) *
                     Quat::Create(Vec3::UNIT_X, m_pitch) * Vec3::UNIT_X;
    m_position += right_dir * dist;
}

void FlyCamera::MoveUpDown(float dist) {
    m_position += GetUp() * dist;
}

OrbitCamera::OrbitCamera(Radians fov, float aspect, float near, float far) {
    SetProject(fov, aspect, near, far);
}

void OrbitCamera::SetTheta(Radians theta) {
    m_theta = theta;
    float l = std::sin(m_phi.Value()) * m_radius;
    m_position.x = l * std::cos(m_theta.Value());
    m_position.z = l * std::sin(m_theta.Value());
}

void OrbitCamera::SetPhi(Radians phi) {
    m_phi = Clamp<float>(phi.Value(), -PI + 0.0001, PI - 0.0001);
    m_position.y = std::cos(m_phi.Value()) * m_radius;
    float l = std::sin(m_phi.Value()) * m_radius;
    m_position.x = l * std::cos(m_theta.Value());
    m_position.z = l * std::sin(m_theta.Value());
}

void OrbitCamera::AddTheta(Radians theta) {
    SetTheta(m_theta + theta);
}

void OrbitCamera::AddPhi(Radians phi) {
    SetPhi(m_phi + phi);
}

void OrbitCamera::SetRadius(float radius) {
    constexpr float MinDist = 0.00001;
    m_radius = radius < MinDist ? MinDist : radius;
}

void OrbitCamera::AddRadius(float radius) {
    SetRadius(m_radius + radius);
}

float OrbitCamera::GetRadius() const {
    return m_radius;
}

Radians OrbitCamera::GetTheta() const {
    return m_theta;
}

Radians OrbitCamera::GetPhi() const {
    return m_phi;
}

const Vec3& OrbitCamera::GetPosition() const {
    return m_position;
}

Mat44 OrbitCamera::GetView() const {
    return LookAt(m_target, m_position, Vec3::UNIT_Y);
}

}  // namespace nickel
