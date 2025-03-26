#pragma once
#include "nickel/common/memory/memory.hpp"
#include "nickel/physics/internal/pch.hpp"
#include "nickel/physics/rigidbody.hpp"
#include "nickel/physics/vehicle.hpp"

namespace nickel::physics {

class ContextImpl;
class SceneImpl;

physx::PxVehicleWheelsSimData* VehicleWheelSimDescriptor2PhysX(
    const VehicleWheelSimDescriptor&);

physx::PxVehicleDriveSimData4W VehicleDriveSim4WDescriptor2PhysX(
    const VehicleDriveSim4WDescriptor&);

class VehicleManagerImpl;

class VehicleQueryFilterShader : public physx::PxQueryFilterCallback {
public:
    physx::PxQueryHitType::Enum preFilter(
        const physx::PxFilterData& filterData, const physx::PxShape* shape,
        const physx::PxRigidActor* actor,
        physx::PxHitFlags& queryFlags) override {
        // TODO: remove magic number
        auto filter_data = shape->getQueryFilterData();
        if (filter_data.word0 & 0x01 || filter_data.word0 & 0x02) {
            return physx::PxQueryHitType::eNONE;
        }
        return physx::PxQueryHitType::eBLOCK;
    }

    physx::PxQueryHitType::Enum postFilter(
        const physx::PxFilterData& filterData, const physx::PxQueryHit& hit,
        const physx::PxShape* shape,
        const physx::PxRigidActor* actor) override {
        return physx::PxQueryHitType::eBLOCK;
    }
};

class VehicleDriveImpl : public RefCountable {
public:
    Vehicle::Type GetType() const { return m_type; }
    virtual void Update(float) = 0;

    physx::PxVehicleWheels* m_drive{};

protected:
    VehicleDriveImpl(Vehicle::Type type) : m_type{type} {}

private:
    Vehicle::Type m_type;
};

class Vehicle4WDriveImpl : public VehicleDriveImpl {
public:
    Vehicle4WDriveImpl();
    Vehicle4WDriveImpl(ContextImpl& ctx, VehicleManagerImpl& mgr,
                       const VehicleWheelSimDescriptor&,
                       const VehicleDriveSim4WDescriptor&, const RigidDynamic&);
    ~Vehicle4WDriveImpl();

    void DecRefcount() override;

    void SetDigitalAccel(bool);
    void SetDigitalBrake(bool);
    void SetDigitalHandbrake(bool);
    void SetDigitalSteerLeft(bool);
    void SetDigitalSteerRight(bool);
    void SetAnalogAccel(float);
    void SetAnalogBrake(float);
    void SetAnalogHandbrake(float);
    void SetAnalogSteer(float);
    void SetGearUp(bool);
    void SetGearDown(bool);

    void SetPadSmoothingConfig(const VehicleInputSmoothingConfig&);
    void SetKeySmoothingConfig(const VehicleInputSmoothingConfig&);
    void SetSteerVsForwardSpeedLookupTable(const VehicleSteerVsForwardTable&);

    void Update(float delta_time) override;

    physx::PxVehicleDrive4W* GetUnderlying();

private:
    VehicleManagerImpl* m_mgr{};
    RigidDynamic m_actor;
    physx::PxVehicleDrive4WRawInputData m_input_data;
    physx::PxVehiclePadSmoothingData m_pad_smoothing_data;
    physx::PxVehicleKeySmoothingData m_key_smoothing_data;
    physx::PxFixedSizeLookupTable<8> m_steer_vs_forward_speed;
};

class VehicleNWDriveImpl : public VehicleDriveImpl {
public:
    VehicleNWDriveImpl();
    VehicleNWDriveImpl(ContextImpl& ctx, VehicleManagerImpl& mgr,
                       const VehicleWheelSimDescriptor&,
                       const VehicleDriveSimNWDescriptor&, const RigidDynamic&);
    ~VehicleNWDriveImpl();

    void DecRefcount() override;

    void SetDigitalAccel(bool);
    void SetDigitalBrake(bool);
    void SetDigitalHandbrake(bool);
    void SetDigitalSteerLeft(bool);
    void SetDigitalSteerRight(bool);
    void SetAnalogAccel(float);
    void SetAnalogBrake(float);
    void SetAnalogHandbrake(float);
    void SetAnalogSteer(float);
    void SetGearUp(bool);
    void SetGearDown(bool);
    
    void SetPadSmoothingConfig(const VehicleInputSmoothingConfig&);
    void SetKeySmoothingConfig(const VehicleInputSmoothingConfig&);
    void SetSteerVsForwardSpeedLookupTable(const VehicleSteerVsForwardTable&);

    void Update(float delta_time) override;

    physx::PxVehicleDriveNW* GetUnderlying();

private:
    VehicleManagerImpl* m_mgr{};
    RigidDynamic m_actor;
    physx::PxVehicleDriveNWRawInputData m_input_data;
    physx::PxVehiclePadSmoothingData m_pad_smoothing_data;
    physx::PxVehicleKeySmoothingData m_key_smoothing_data;
    physx::PxFixedSizeLookupTable<8> m_steer_vs_forward_speed;
};

class VehicleTankDriveImpl : public VehicleDriveImpl {
public:
    VehicleTankDriveImpl();
    VehicleTankDriveImpl(ContextImpl& ctx, VehicleManagerImpl& mgr,
                         VehicleTankDriveMode drive_mode,
                         const VehicleWheelSimDescriptor&,
                         const VehicleDriveSimDescriptor&, const RigidDynamic&);
    ~VehicleTankDriveImpl();

    void DecRefcount() override;

    void SetDigitalAccel(bool);
    void SetDigitalLeftBrake(bool);
    void SetDigitalRightBrake(bool);
    void SetDigitalLeftThrust(bool);
    void SetDigitalRightThrust(bool);
    void SetAnalogAccel(float);
    void SetAnalogLeftBrake(float);
    void SetAnalogRightBrake(float);
    void SetAnalogLeftThrust(float);
    void SetAnalogRightThrust(float);
    void SetGearUp(bool);
    void SetGearDown(bool);

    void SetPadSmoothingConfig(const VehicleInputSmoothingConfig&);
    void SetKeySmoothingConfig(const VehicleInputSmoothingConfig&);

    void Update(float delta_time) override;

    physx::PxVehicleDriveTank* GetUnderlying();

private:
    VehicleManagerImpl* m_mgr{};
    RigidDynamic m_actor;
    physx::PxVehicleDriveTankRawInputData m_input_data;
    physx::PxVehiclePadSmoothingData m_pad_smoothing_data;
    physx::PxVehicleKeySmoothingData m_key_smoothing_data;
};

class VehicleNoDriveImpl : public VehicleDriveImpl {
public:
    VehicleNoDriveImpl();
    VehicleNoDriveImpl(ContextImpl& ctx, VehicleManagerImpl& mgr,
                       const VehicleWheelSimDescriptor&, const RigidDynamic&);
    ~VehicleNoDriveImpl();

    void DecRefcount();

    void SetDriveTorque(uint32_t wheel_idx, float);
    void SetSteerAngle(uint32_t wheel_idx, Radians);
    void SetBrakeTorque(uint32_t wheel_idx, float);

    void Update(float) override;

    physx::PxVehicleNoDrive* GetUnderlying();

private:
    VehicleManagerImpl* m_mgr{};
    RigidDynamic m_actor;
};

class VehicleManagerImpl {
public:
    explicit VehicleManagerImpl(ContextImpl&, SceneImpl&);
    ~VehicleManagerImpl();

    Vehicle4WDriveImpl* CreateVehicle4WDrive(const VehicleWheelSimDescriptor&,
                                             const VehicleDriveSim4WDescriptor&,
                                             const RigidDynamic&);
    VehicleNWDriveImpl* CreateVehicleNWDrive(const VehicleWheelSimDescriptor&,
                                             const VehicleDriveSimNWDescriptor&,
                                             const RigidDynamic&);

    VehicleTank CreateVehicleTankDrive(VehicleTankDriveMode drive_mode,
                                       const VehicleWheelSimDescriptor& wheel,
                                       const VehicleDriveSimDescriptor& drive,
                                       const RigidDynamic& actor);

    VehicleNoDrive CreateVehicleNoDrive(const VehicleWheelSimDescriptor& wheel,
                                        const RigidDynamic& actor);

    void Update(float delta_time);
    void GC();

    std::vector<VehicleDriveImpl*> m_pending_delete;
    std::vector<VehicleDriveImpl*> m_vehicles;
    BlockMemoryAllocator<Vehicle4WDriveImpl> m_4w_allocator;
    BlockMemoryAllocator<VehicleNWDriveImpl> m_nw_allocator;
    BlockMemoryAllocator<VehicleTankDriveImpl> m_tank_allocator;
    BlockMemoryAllocator<VehicleNoDriveImpl> m_no_drive_allocator;
    physx::PxVehicleDrivableSurfaceToTireFrictionPairs* m_friction_pairs;

private:
    static constexpr uint32_t BatchTouchExpandStep = PX_MAX_NB_WHEELS * 4;
    static constexpr uint32_t BatchResultExpandStep = PX_MAX_NB_WHEELS;

    physx::PxBatchQueryExt* m_batch_query{};
    SceneImpl& m_scene;
    ContextImpl& m_ctx;
    uint32_t m_wheel_num{};
    uint32_t m_batch_result_num{};
    uint32_t m_batch_touch_num{};
    VehicleQueryFilterShader m_filter_shader;

    void deletePendingVehicles();
    void tryRecreateBatchQuery();
    void recreateBatchQuery(uint32_t batch_result_num,
                            uint32_t batch_touch_num);
    void setupFrictionPairs();
};

}  // namespace nickel::physics