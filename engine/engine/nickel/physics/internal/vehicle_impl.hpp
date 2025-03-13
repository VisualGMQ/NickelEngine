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

class Vehicle4WDriveImpl : public RefCountable {
public:
    Vehicle4WDriveImpl() = default;
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
    void SetAnalogSteerLeft(float);
    void SetAnalogSteerRight(float);
    void SetGearUp(bool);
    void SetGearDown(bool);

    void Update(float delta_time);

    physx::PxVehicleDrive4W* m_drive{};

private:
    VehicleManagerImpl* m_mgr{};
    RigidDynamic m_actor;
    physx::PxVehicleDrive4WRawInputData m_input_data;
    physx::PxVehicleTelemetryData* m_telemetry{};
};

class VehicleManagerImpl {
public:
    explicit VehicleManagerImpl(ContextImpl&, SceneImpl&);
    ~VehicleManagerImpl();

    Vehicle4WDriveImpl* CreateVehicle4WDrive(const VehicleWheelSimDescriptor&,
                                             const VehicleDriveSim4WDescriptor&,
                                             const RigidDynamic&);
    void Update(float delta_time);
    void GC();

    std::vector<Vehicle4WDriveImpl*> m_pending_delete;
    std::vector<Vehicle4WDriveImpl*> m_vehicles;
    BlockMemoryAllocator<Vehicle4WDriveImpl> m_allocator;
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