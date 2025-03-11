#include "nickel/physics/vehicle.hpp"

#include "nickel/physics/context.hpp"
#include "nickel/physics/internal/pch.hpp"
#include "nickel/physics/internal/vehicle_impl.hpp"

namespace nickel::physics {

uint32_t VehicleWheelSimDescriptor::GetWheelNum() const {
    return 4 + m_other_wheels.size();
}

VehicleDriveSimDescriptor::VehicleDriveSimDescriptor(Type type)
    : m_type{type} {}

VehicleDriveSimDescriptor::Type VehicleDriveSimDescriptor::GetType() const {
    return m_type;
}

VehicleDriveSim4WDescriptor::VehicleDriveSim4WDescriptor()
    : VehicleDriveSimDescriptor{Type::FourWheel} {}

void Vehicle4W::SetDigitalAccel(bool active) {
    m_impl->SetDigitalAccel(active);
}

void Vehicle4W::SetDigitalBrake(bool active) {
    m_impl->SetDigitalBrake(active);
}

void Vehicle4W::SetDigitalHandbrake(bool active) {
    m_impl->SetDigitalHandbrake(active);
}

void Vehicle4W::SetDigitalSteerLeft(bool active) {
    m_impl->SetDigitalSteerLeft(active);
}

void Vehicle4W::SetDigitalSteerRight(bool active) {
    m_impl->SetDigitalSteerRight(active);
}

void Vehicle4W::SetAnalogAccel(float value) {
    m_impl->SetAnalogAccel(value);
}

void Vehicle4W::SetAnalogBrake(float value) {
    m_impl->SetAnalogBrake(value);
}

void Vehicle4W::SetAnalogHandbrake(float value) {
    m_impl->SetAnalogHandbrake(value);
}

void Vehicle4W::SetAnalogSteerLeft(float value) {
    m_impl->SetAnalogSteerLeft(value);
}

void Vehicle4W::SetAnalogSteerRight(float value) {
    m_impl->SetAnalogSteerRight(value);
}

void Vehicle4W::SetGearUp(bool active) {
    m_impl->SetGearUp(active);
}

void Vehicle4W::SetGearDown(bool active) {
    m_impl->SetGearDown(active);
}

void Vehicle4W::Update(float delta_time) {
    m_impl->Update(delta_time);
}

VehicleManager::VehicleManager(ContextImpl& ctx, SceneImpl& scene)
    : m_impl{std::make_unique<VehicleManagerImpl>(ctx, scene)} {}

VehicleManager::~VehicleManager() {}

Vehicle4W VehicleManager::CreateVehicle4WDrive(
    const VehicleWheelSimDescriptor& wheel,
    const VehicleDriveSim4WDescriptor& drive, const RigidDynamic& actor) {
    return m_impl->CreateVehicle4WDrive(wheel, drive, actor);
}

void VehicleManager::Update(float delta_time) {
    m_impl->Update(delta_time);
}

void VehicleManager::GC() {
    m_impl->GC();
}

}  // namespace nickel::physics