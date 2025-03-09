#include "nickel/physics/cct.hpp"
#include "nickel/physics/internal/cct_impl.hpp"

namespace nickel::physics {

Controller::Controller(CCTType type) : m_type{type} {}

ControllerConst::ControllerConst(CCTType type) : Controller{type} {}

CapsuleController::CapsuleController(CapsuleControllerImpl* impl)
    : Controller{CCTType::Capsule}, ImplWrapper(impl) {}

CapsuleController::CapsuleController() : Controller(CCTType::Capsule) {}

void CapsuleController::SetRadius(float radius) {
    m_impl->SetRadius(radius);
}

float CapsuleController::GetRadius() const {
    return m_impl->GetRadius();
}

void CapsuleController::SetHeight(float height) {
    m_impl->SetHeight(height);
}

float CapsuleController::GetHeight() const {
    return m_impl->GetHeight();
}

void CapsuleController::Resize(float height) {
    m_impl->Resize(height);
}

Flags<CCTCollisionFlag> CapsuleController::MoveAndSlide(const Vec3& disp,
                                                        float min_dist, float elapsed_time,
    ControllerFilters* filter) {
    return m_impl->MoveAndSlide(disp, min_dist, elapsed_time, filter);
}

void CapsuleController::SetPosition(const Vec3& v) {
    m_impl->SetPosition(v);
}

void CapsuleController::SetFootPosition(const Vec3& v) {
    m_impl->SetFootPosition(v);
}

Vec3 CapsuleController::GetPosition() const {
    return m_impl->GetPosition();
}

Vec3 CapsuleController::GetFootPosition() const {
    return m_impl->GetFootPosition();
}

void CapsuleController::SetStepOffset(float offset) const {
    m_impl->SetStepOffset(offset);
}

float CapsuleController::GetStepOffset() const {
    return m_impl->GetStepOffset();
}

void CapsuleController::SetUpDirection(const Vec3& dir) {
    m_impl->SetUpDirection(dir);
}

Vec3 CapsuleController::GetUpDirection() const {
    return m_impl->GetUpDirection();
}

CCTDesc::NonWalkableMode CapsuleController::GetNonWalkableMode() const {
    return m_impl->GetNonWalkableMode();
}

void CapsuleController::SetNonWalkableMode(CCTDesc::NonWalkableMode mode) {
    m_impl->SetNonWalkableMode(mode);
}

CapsuleController::Descriptor::ClimbingMode CapsuleController::GetClimbingMode()
    const {
    return m_impl->GetClimbingMode();
}

void CapsuleController::SetClimbingMode(Descriptor::ClimbingMode mode) {
    m_impl->SetClimbingMode(mode);
}

CapsuleControllerConst::CapsuleControllerConst()
    : ControllerConst{CCTType::Capsule} {}

CapsuleControllerConst::CapsuleControllerConst(CapsuleControllerConstImpl* impl)
    : ControllerConst{CCTType::Capsule}, ImplWrapper(impl) {}

float CapsuleControllerConst::GetRadius() const {
    return m_impl->GetRadius();
}

float CapsuleControllerConst::GetHeight() const {
    return m_impl->GetHeight();
}

Vec3 CapsuleControllerConst::GetPosition() const {
    return m_impl->GetPosition();
}

Vec3 CapsuleControllerConst::GetFootPosition() const {
    return m_impl->GetFootPosition();
}

float CapsuleControllerConst::GetStepOffset() const {
    return m_impl->GetStepOffset();
}

Vec3 CapsuleControllerConst::GetUpDirection() const {
    return m_impl->GetUpDirection();
}

CCTDesc::NonWalkableMode CapsuleControllerConst::GetNonWalkableMode() const {
    return m_impl->GetNonWalkableMode();
}

CapsuleController::Descriptor::ClimbingMode
CapsuleControllerConst::GetClimbingMode() const {
    return m_impl->GetClimbingMode();
}

}  // namespace nickel::physics