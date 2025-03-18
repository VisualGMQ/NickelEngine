#include "nickel/physics/vehicle.hpp"

#include "nickel/physics/context.hpp"
#include "nickel/physics/internal/pch.hpp"
#include "nickel/physics/internal/util.hpp"
#include "nickel/physics/internal/vehicle_impl.hpp"

namespace nickel::physics {

uint32_t VehicleWheelSimDescriptor::GetWheelNum() const {
    return m_wheels.size();
}

VehicleWheelSimDescriptor::Type VehicleWheelSimDescriptor::GetType() const {
    return m_type;
}

VehicleWheelSimDescriptor::VehicleWheelSimDescriptor(Type type)
    : m_type{type} {}

VehicleWheelSim4WDescriptor::VehicleWheelSim4WDescriptor()
    : VehicleWheelSimDescriptor(Type::FourWheel) {}

VehicleDriveSimDescriptor::Type VehicleDriveSimDescriptor::GetType() const {
    return m_type;
}

VehicleDriveSimDescriptor::VehicleDriveSimDescriptor(Type type)
    : m_type{type} {}

void VehicleDifferentialNWDescriptor::SetDrivenWheel(uint32_t idx) {
    NICKEL_ASSERT(idx <= VehicleMaxWheelNum);
    m_wheels[idx] = true;
}

bool VehicleDifferentialNWDescriptor::GetDrivenWheel(uint32_t idx) const {
    NICKEL_ASSERT(idx <= VehicleMaxWheelNum);
    return m_wheels[idx];
}

VehicleDriveSim4WDescriptor::VehicleDriveSim4WDescriptor()
    : VehicleDriveSimDescriptor{Type::FourWheel} {}

VehicleDriveSimNWDescriptor::VehicleDriveSimNWDescriptor()
    : VehicleDriveSimDescriptor{Type::ArbitraryWheel} {}

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

void VehicleNW::SetDigitalAccel(bool active) {
    m_impl->SetDigitalAccel(active);
}

void VehicleNW::SetDigitalBrake(bool active) {
    m_impl->SetDigitalBrake(active);
}

void VehicleNW::SetDigitalHandbrake(bool active) {
    m_impl->SetDigitalHandbrake(active);
}

void VehicleNW::SetDigitalSteerLeft(bool active) {
    m_impl->SetDigitalSteerLeft(active);
}

void VehicleNW::SetDigitalSteerRight(bool active) {
    m_impl->SetDigitalSteerRight(active);
}

void VehicleNW::SetAnalogAccel(float value) {
    m_impl->SetAnalogAccel(value);
}

void VehicleNW::SetAnalogBrake(float value) {
    m_impl->SetAnalogBrake(value);
}

void VehicleNW::SetAnalogHandbrake(float value) {
    m_impl->SetAnalogHandbrake(value);
}

void VehicleNW::SetAnalogSteerLeft(float value) {
    m_impl->SetAnalogSteerLeft(value);
}

void VehicleNW::SetAnalogSteerRight(float value) {
    m_impl->SetAnalogSteerRight(value);
}

void VehicleNW::SetGearUp(bool active) {
    m_impl->SetGearUp(active);
}

void VehicleNW::SetGearDown(bool active) {
    m_impl->SetGearDown(active);
}

void VehicleNW::Update(float delta_time) {
    m_impl->Update(delta_time);
}

Vehicle::Vehicle(Vehicle4W vehicle) {
    m_impl = vehicle.GetImpl();
    if (m_impl) {
        m_impl->IncRefcount();
    }
}

Vehicle::Vehicle(VehicleNW vehicle) {
    m_impl = vehicle.GetImpl();
    if (m_impl) {
        m_impl->IncRefcount();
    }
}

Vehicle& Vehicle::operator=(Vehicle4W vehicle) {
    if (vehicle.GetImpl() != m_impl) {
        if (m_impl) {
            m_impl->DecRefcount();
        }
        m_impl = vehicle.GetImpl();
        if (m_impl) {
            m_impl->IncRefcount();
        }
    }
    return *this;
}

Vehicle& Vehicle::operator=(VehicleNW vehicle) {
    if (vehicle.GetImpl() != m_impl) {
        if (m_impl) {
            m_impl->DecRefcount();
        }
        m_impl = vehicle.GetImpl();
        if (m_impl) {
            m_impl->IncRefcount();
        }
    }
    return *this;
}

Vehicle4W Vehicle::CastAs4W() const {
    if (m_impl && m_impl->GetType() == VehicleDriveImpl::Type::FourWheel) {
        m_impl->IncRefcount();
        return Vehicle4W{static_cast<Vehicle4WDriveImpl*>(m_impl)};
    }
    return nullptr;
}

VehicleNW Vehicle::CastAsNW() const {
    if (m_impl && m_impl->GetType() == VehicleDriveImpl::Type::N_Wheel) {
        m_impl->IncRefcount();
        return VehicleNW{static_cast<VehicleNWDriveImpl*>(m_impl)};
    }
    return nullptr;
}

std::vector<float> ComputeVehicleSprungMass(
    std::span<const Vec3> sprung_mass_coord, const Vec3& center_of_mass,
    float totle_mass) {
    std::vector<physx::PxVec3> coords;
    coords.resize(sprung_mass_coord.size());
    std::transform(sprung_mass_coord.begin(), sprung_mass_coord.end(),
                   coords.begin(), Vec3ToPhysX);
    std::vector<float> sprung_mass;
    sprung_mass.resize(sprung_mass_coord.size());
    physx::PxVehicleComputeSprungMasses(sprung_mass_coord.size(), coords.data(),
                                        Vec3ToPhysX(center_of_mass), totle_mass,
                                        1, sprung_mass.data());
    return sprung_mass;
}

VehicleManager::VehicleManager(ContextImpl& ctx, SceneImpl& scene)
    : m_impl{std::make_unique<VehicleManagerImpl>(ctx, scene)} {}

VehicleManager::~VehicleManager() {}

Vehicle4W VehicleManager::CreateVehicle4WDrive(
    const VehicleWheelSimDescriptor& wheel,
    const VehicleDriveSim4WDescriptor& drive, const RigidDynamic& actor) {
    return m_impl->CreateVehicle4WDrive(wheel, drive, actor);
}

VehicleNW VehicleManager::CreateVehicleNWDrive(
    const VehicleWheelSimDescriptor& wheel,
    const VehicleDriveSimNWDescriptor& drive, const RigidDynamic& actor) {
    return m_impl->CreateVehicleNWDrive(wheel, drive, actor);
}

void VehicleManager::Update(float delta_time) {
    m_impl->Update(delta_time);
}

void VehicleManager::GC() {
    m_impl->GC();
}

}  // namespace nickel::physics