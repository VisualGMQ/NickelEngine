#include "nickel/physics/internal/vehicle_impl.hpp"

#include "3rdlibs/physx/physx/source/physxvehicle/src/PxVehicleSuspWheelTire4.h"
#include "nickel/common/macro.hpp"
#include "nickel/physics/internal/context_impl.hpp"
#include "nickel/physics/internal/util.hpp"

namespace nickel::physics {
inline physx::PxVehicleWheelData WheelData2PhysX(
    const VehicleWheelSimDescriptor::Wheel& wheel) {
    physx::PxVehicleWheelData data;
    data.mMass = wheel.m_mass;
    data.mRadius = wheel.m_radius;
    data.mWidth = wheel.m_width;
    data.mDampingRate = wheel.m_damping_rate;
    data.mMaxSteer = wheel.m_max_steer.Value();
    data.mMOI = wheel.m_moi;
    data.mToeAngle = wheel.m_toe_angle.Value();
    data.mMaxBrakeTorque = wheel.m_max_brake_torque;
    data.mMaxHandBrakeTorque = wheel.m_max_hand_brake_torque;

    return data;
}

inline physx::PxVehicleTireData TireData2PhysX(
    const VehicleWheelSimDescriptor::Tire& tire) {
    physx::PxVehicleTireData data;
    data.mType = tire.m_type;
    data.mLatStiffX = tire.m_lat_stiff_x;
    data.mLatStiffY = tire.m_lat_stiff_y.Value();
    memcpy(data.mFrictionVsSlipGraph, tire.m_friction_vs_slip_graph,
           sizeof(data.mFrictionVsSlipGraph));
    data.mCamberStiffnessPerUnitGravity =
        tire.m_camber_stiffness_per_unit_gravity.Value();
    data.mLongitudinalStiffnessPerUnitGravity =
        tire.m_longitudinal_stiffness_per_unit_gravity;
    return data;
}

inline physx::PxVehicleSuspensionData SuspensionData2PhysX(
    const VehicleWheelSimDescriptor::Suspension& suspension) {
    physx::PxVehicleSuspensionData data;
    data.mMaxCompression = suspension.m_max_compression;
    data.mMaxDroop = suspension.m_max_droop;
    data.mSpringStrength = suspension.m_spring_strength;
    data.mSprungMass = suspension.m_sprung_mass;
    data.mSpringDamperRate = suspension.m_spring_damper_rate;
    data.mCamberAtRest = suspension.m_camber_at_rest.Value();
    data.mCamberAtMaxCompression =
        suspension.m_camber_at_max_compression.Value();
    data.mCamberAtMaxDroop = suspension.m_camber_at_max_droop.Value();
    return data;
}

inline physx::PxVehicleAntiRollBarData VehicleAntiRollBarData2PhysX(
    const VehicleWheelSimDescriptor::AntiRollBar& data) {
    physx::PxVehicleAntiRollBarData bar;
    bar.mWheel0 = data.m_wheel0;
    bar.mWheel1 = data.m_wheel1;
    bar.mStiffness = data.stiffness;
    return bar;
}

inline physx::PxVehicleDriveTankControlModel::Enum VehicleTankDriveMode2PhysX(
    VehicleTankDriveMode mode) {
    switch (mode) {
        case VehicleTankDriveMode::Standard:
            return physx::PxVehicleDriveTankControlModel::eSTANDARD;
        case VehicleTankDriveMode::Special:
            return physx::PxVehicleDriveTankControlModel::eSPECIAL;
    }
    NICKEL_CANT_REACH();
    return {};
}

// NOTE: don't forget call `free()` to free return value!
inline physx::PxVehicleWheelsSimData* VehicleWheelSimDescriptor2PhysX(
    const VehicleWheelSimDescriptor& desc) {
    uint32_t wheel_num = desc.GetWheelNum();
    physx::PxVehicleWheelsSimData* data =
        physx::PxVehicleWheelsSimData::allocate(wheel_num);

    std::array<VehicleWheelSimDescriptor::WheelDescriptor, PX_MAX_NB_WHEELS>
        wheel_desc;

    uint32_t other_wheel_record_num = 0;
    const VehicleWheelSim4WDescriptor* wheel_4w_desc =
        desc.GetType() == VehicleWheelSimDescriptor::Type::FourWheel
            ? static_cast<const VehicleWheelSim4WDescriptor*>(&desc)
            : nullptr;
    for (uint32_t i = 0; i < wheel_num; i++) {
        auto& wheel = desc.m_wheels[i];
        if (wheel_4w_desc) {
            if (wheel_4w_desc->m_front_left_wheel &&
                wheel_4w_desc->m_front_left_wheel.value() == i) {
                wheel_desc[physx::PxVehicleDrive4WWheelOrder::eFRONT_LEFT] =
                    wheel;
            } else if (wheel_4w_desc->m_front_right_wheel &&
                       wheel_4w_desc->m_front_right_wheel.value() == i) {
                wheel_desc[physx::PxVehicleDrive4WWheelOrder::eFRONT_RIGHT] =
                    wheel;
            } else if (wheel_4w_desc->m_rear_left_wheel &&
                       wheel_4w_desc->m_rear_left_wheel.value() == i) {
                wheel_desc[physx::PxVehicleDrive4WWheelOrder::eREAR_LEFT] =
                    wheel;
            } else if (wheel_4w_desc->m_rear_right_wheel &&
                       wheel_4w_desc->m_rear_right_wheel.value() == i) {
                wheel_desc[physx::PxVehicleDrive4WWheelOrder::eREAR_RIGHT] =
                    wheel;
            } else {
                wheel_desc[other_wheel_record_num + 4] = desc.m_wheels[i];
                other_wheel_record_num++;
            }
        } else {
            wheel_desc[i] = desc.m_wheels[i];
        }
    }

    for (uint32_t i = 0; i < desc.GetWheelNum(); i++) {
        auto& wheel = wheel_desc[i];

        data->setWheelData(i, WheelData2PhysX(wheel.m_wheel));
        data->setTireData(i, TireData2PhysX(wheel.m_tire));
        data->setSuspensionData(i, SuspensionData2PhysX(wheel.m_suspension));
        data->setSuspTravelDirection(
            i, Vec3ToPhysX(wheel.m_suspension_travel_directions));
        data->setWheelCentreOffset(
            i, Vec3ToPhysX(wheel.m_wheel_centre_cm_offsets));
        data->setSuspForceAppPointOffset(
            i, Vec3ToPhysX(wheel.m_suspension_force_app_point_offsets));
        data->setTireForceAppPointOffset(
            i, Vec3ToPhysX(wheel.m_tire_force_app_cm_offsets));
        data->setWheelShapeMapping(i, i);
        data->setSceneQueryFilterData(
            i, FilterData2PhysX(wheel.m_scene_query_filter_data));
    }

    data->setChassisMass(desc.m_chassis_mass);

    for (auto& bar : desc.m_anti_roll_bars) {
        data->addAntiRollBarData(VehicleAntiRollBarData2PhysX(bar));
    }

    return data;
}

inline physx::PxVehicleEngineData VehicleEngineData2PhysX(
    const VehicleEngineDescriptor& desc) {
    physx::PxVehicleEngineData data;
    data.mMaxOmega = desc.m_max_omega;
    data.mPeakTorque = desc.m_peak_torque;
    data.mMOI = desc.m_moi;
    data.mDampingRateFullThrottle = desc.m_damping_rate_full_throttle;
    data.mDampingRateZeroThrottleClutchDisengaged =
        desc.m_damping_rate_zero_throttle_clutch_disengaged;
    data.mDampingRateZeroThrottleClutchEngaged =
        desc.m_damping_rate_zero_throttle_clutch_engaged;
    return data;
}

inline physx::PxVehicleGearsData VehicleGearData2PhysX(
    const VehicleGearDescriptor& desc) {
    physx::PxVehicleGearsData data;
    data.mFinalRatio = desc.m_final_ratio;
    data.mNbRatios = desc.m_other_forward_ratios.size() + 3;
    data.mSwitchTime = desc.m_switch_time;
    data.setGearRatio(physx::PxVehicleGearsData::eREVERSE,
                      desc.m_reverse_ratio);
    data.setGearRatio(physx::PxVehicleGearsData::eNEUTRAL,
                      desc.m_neutral_ratio);
    data.setGearRatio(physx::PxVehicleGearsData::eFIRST, desc.m_first_ratio);
    for (uint32_t i = 0; i < desc.m_other_forward_ratios.size(); i++) {
        data.setGearRatio(static_cast<physx::PxVehicleGearsData::Enum>(
                              physx::PxVehicleGearsData::eSECOND + i),
                          desc.m_other_forward_ratios[i]);
    }
    return data;
}

inline physx::PxVehicleClutchData VehicleClutchData2PhysX(
    const VehicleClutchDescriptor& desc) {
    physx::PxVehicleClutchData data;
    data.mStrength = desc.m_strength;
    data.mEstimateIterations = desc.m_estimate_iterations;
    data.mAccuracyMode =
        desc.m_accuracy_mode == VehicleClutchDescriptor::AccuracyMode::Estimate
            ? physx::PxVehicleClutchAccuracyMode::eESTIMATE
            : physx::PxVehicleClutchAccuracyMode::eBEST_POSSIBLE;
    return data;
}

inline physx::PxVehicleDifferential4WData::Enum VehicleDifferential4WType2PhysX(
    VehicleDifferential4WDescriptor::Type type) {
    switch (type) {
        case VehicleDifferential4WDescriptor::Type::LS_4_WD:
            return physx::PxVehicleDifferential4WData::eDIFF_TYPE_LS_4WD;
        case VehicleDifferential4WDescriptor::Type::LS_Front_WD:
            return physx::PxVehicleDifferential4WData::eDIFF_TYPE_LS_FRONTWD;
        case VehicleDifferential4WDescriptor::Type::LS_Rear_WD:
            return physx::PxVehicleDifferential4WData::eDIFF_TYPE_LS_REARWD;
        case VehicleDifferential4WDescriptor::Type::Open_4_WD:
            return physx::PxVehicleDifferential4WData::eDIFF_TYPE_OPEN_4WD;
        case VehicleDifferential4WDescriptor::Type::Open_Front_WD:
            return physx::PxVehicleDifferential4WData::eDIFF_TYPE_OPEN_FRONTWD;
        case VehicleDifferential4WDescriptor::Type::Open_Rear_WD:
            return physx::PxVehicleDifferential4WData::eDIFF_TYPE_OPEN_REARWD;
    }
    NICKEL_CANT_REACH();
    return {};
}

inline physx::PxVehicleDifferentialNWData VehicleDifferentialNWData2PhysX(
    VehicleDifferentialNWDescriptor desc) {
    physx::PxVehicleDifferentialNWData data;

    for (int i = 0; i < PX_MAX_NB_WHEELS; i++) {
        if (desc.GetDrivenWheel(i)) {
            data.setDrivenWheel(i, true);
        }
    }
    return data;
}

inline physx::PxVehicleDifferential4WData VehicleDifferential4WData2PhysX(
    const VehicleDifferential4WDescriptor& desc) {
    physx::PxVehicleDifferential4WData data;
    data.mCentreBias = desc.m_centre_bias;
    data.mFrontBias = desc.m_front_bias;
    data.mRearBias = desc.m_rear_bias;
    data.mFrontRearSplit = desc.m_front_rear_split;
    data.mFrontLeftRightSplit = desc.m_front_left_right_split;
    data.mRearLeftRightSplit = desc.m_rear_left_right_split;
    data.mType = VehicleDifferential4WType2PhysX(desc.m_type);

    return data;
}

inline physx::PxVehicleAckermannGeometryData
VehicleAckermannGeometryDescriptor2PhysX(
    const VehicleAckermannGeometryDescriptor& desc) {
    physx::PxVehicleAckermannGeometryData data;
    data.mAccuracy = desc.m_accuracy;
    data.mAxleSeparation = desc.m_axle_separation;
    data.mFrontWidth = desc.m_front_width;
    data.mRearWidth = desc.m_rear_width;
    return data;
}

physx::PxVehicleDriveSimData VehicleDriveSimDescriptor2PhysX(
    const VehicleDriveSimDescriptor& desc) {
    physx::PxVehicleDriveSimData data;
    data.setEngineData(VehicleEngineData2PhysX(desc.m_engine));
    data.setClutchData(VehicleClutchData2PhysX(desc.m_clutch));
    data.setGearsData(VehicleGearData2PhysX(desc.m_gear));
    return data;
}

physx::PxVehicleDriveSimData4W VehicleDriveSim4WDescriptor2PhysX(
    const VehicleDriveSim4WDescriptor& desc) {
    physx::PxVehicleDriveSimData4W data;
    data.setDiffData(VehicleDifferential4WData2PhysX(desc.m_diff));
    data.setEngineData(VehicleEngineData2PhysX(desc.m_engine));
    data.setClutchData(VehicleClutchData2PhysX(desc.m_clutch));
    data.setGearsData(VehicleGearData2PhysX(desc.m_gear));
    data.setAckermannGeometryData(
        VehicleAckermannGeometryDescriptor2PhysX(desc.m_ackermann));
    data.setDiffData(VehicleDifferential4WData2PhysX(desc.m_diff));
    return data;
}

physx::PxVehicleDriveSimDataNW VehicleDriveSimNWDescriptor2PhysX(
    const VehicleDriveSimNWDescriptor& desc) {
    physx::PxVehicleDriveSimDataNW data;
    data.setEngineData(VehicleEngineData2PhysX(desc.m_engine));
    data.setClutchData(VehicleClutchData2PhysX(desc.m_clutch));
    data.setGearsData(VehicleGearData2PhysX(desc.m_gear));
    data.setDiffData(VehicleDifferentialNWData2PhysX(desc.m_diff));
    return data;
}

Vehicle4WDriveImpl::Vehicle4WDriveImpl() : VehicleDriveImpl{Type::FourWheel} {}

Vehicle4WDriveImpl::Vehicle4WDriveImpl(
    ContextImpl& ctx, VehicleManagerImpl& mgr,
    const VehicleWheelSimDescriptor& wheel_sim_desc,
    const VehicleDriveSim4WDescriptor& drive_sim_desc,
    const RigidDynamic& actor)
    : VehicleDriveImpl{Type::FourWheel}, m_mgr{&mgr} {
    physx::PxVehicleWheelsSimData* wheel_sim_data =
        VehicleWheelSimDescriptor2PhysX(wheel_sim_desc);

    physx::PxVehicleDriveSimData4W drive_sim_data =
        VehicleDriveSim4WDescriptor2PhysX(drive_sim_desc);

    auto wheel_num = wheel_sim_desc.GetWheelNum();
    auto drive = physx::PxVehicleDrive4W::allocate(wheel_num);

    if (!drive) {
        LOGE("vehicle create failed");
        wheel_sim_data->free();
        return;
    }

    drive->setup(ctx.m_physics,
                 static_cast<physx::PxRigidDynamic*>(actor.GetImpl()->m_actor),
                 *wheel_sim_data, drive_sim_data, wheel_num - 4);

    wheel_sim_data->free();
    m_actor = actor;
    drive->setToRestState();
    drive->mDriveDynData.forceGearChange(physx::PxVehicleGearsData::eNEUTRAL);

    m_drive = drive;
}

Vehicle4WDriveImpl::~Vehicle4WDriveImpl() {
    if (m_drive) {
        m_drive->release();
    }
}

void Vehicle4WDriveImpl::DecRefcount() {
    RefCountable::DecRefcount();

    if (Refcount() == 0) {
        m_mgr->m_4w_allocator.MarkAsGarbage(this);
        m_mgr->m_pending_delete.push_back(this);
    }
}

void Vehicle4WDriveImpl::SetDigitalAccel(bool active) {
    m_input_data.setDigitalAccel(active);
}

void Vehicle4WDriveImpl::SetDigitalBrake(bool active) {
    m_input_data.setDigitalBrake(active);
}

void Vehicle4WDriveImpl::SetDigitalHandbrake(bool active) {
    m_input_data.setDigitalHandbrake(active);
}

void Vehicle4WDriveImpl::SetDigitalSteerLeft(bool active) {
    m_input_data.setDigitalSteerLeft(active);
}

void Vehicle4WDriveImpl::SetDigitalSteerRight(bool active) {
    m_input_data.setDigitalSteerRight(active);
}

void Vehicle4WDriveImpl::SetAnalogAccel(float value) {
    m_input_data.setAnalogAccel(value);
}

void Vehicle4WDriveImpl::SetAnalogBrake(float value) {
    m_input_data.setAnalogBrake(value);
}

void Vehicle4WDriveImpl::SetAnalogHandbrake(float value) {
    m_input_data.setAnalogHandbrake(value);
}

void Vehicle4WDriveImpl::SetAnalogSteerLeft(float value) {
    m_input_data.setDigitalSteerLeft(value);
}

void Vehicle4WDriveImpl::SetAnalogSteerRight(float value) {
    m_input_data.setDigitalSteerRight(value);
}

void Vehicle4WDriveImpl::SetGearUp(bool active) {
    m_input_data.setGearUp(active);
}

void Vehicle4WDriveImpl::SetGearDown(bool active) {
    m_input_data.setGearDown(active);
}

// TODO: directly copied from PhysX example. Move to vehicle config later
// clang-format off
physx::PxVehicleKeySmoothingData gKeySmoothingData=
{
    {
        3.0f,    //rise rate eANALOG_INPUT_ACCEL
        3.0f,    //rise rate eANALOG_INPUT_BRAKE
        10.0f,    //rise rate eANALOG_INPUT_HANDBRAKE
        2.5f,    //rise rate eANALOG_INPUT_STEER_LEFT
        2.5f,    //rise rate eANALOG_INPUT_STEER_RIGHT
    },
    {
        5.0f,    //fall rate eANALOG_INPUT__ACCEL
        5.0f,    //fall rate eANALOG_INPUT__BRAKE
        10.0f,    //fall rate eANALOG_INPUT__HANDBRAKE
        5.0f,    //fall rate eANALOG_INPUT_STEER_LEFT
        5.0f    //fall rate eANALOG_INPUT_STEER_RIGHT
    }
};

physx::PxVehiclePadSmoothingData gPadSmoothingData=
{
    {
        6.0f,    //rise rate eANALOG_INPUT_ACCEL
        6.0f,    //rise rate eANALOG_INPUT_BRAKE
        12.0f,    //rise rate eANALOG_INPUT_HANDBRAKE
        2.5f,    //rise rate eANALOG_INPUT_STEER_LEFT
        2.5f,    //rise rate eANALOG_INPUT_STEER_RIGHT
    },
    {
        10.0f,    //fall rate eANALOG_INPUT_ACCEL
        10.0f,    //fall rate eANALOG_INPUT_BRAKE
        12.0f,    //fall rate eANALOG_INPUT_HANDBRAKE
        5.0f,    //fall rate eANALOG_INPUT_STEER_LEFT
        5.0f    //fall rate eANALOG_INPUT_STEER_RIGHT
    }
};

float gSteerVsForwardSpeedData[2*8]=
{
    0.0f,        0.75f,
    5.0f,        0.75f,
    30.0f,        0.125f,
    120.0f,        0.1f,
    PX_MAX_F32, PX_MAX_F32,
    PX_MAX_F32, PX_MAX_F32,
    PX_MAX_F32, PX_MAX_F32,
    PX_MAX_F32, PX_MAX_F32
};
physx::PxFixedSizeLookupTable<8> gSteerVsForwardSpeedTable(gSteerVsForwardSpeedData,4);
// clang-format on

void Vehicle4WDriveImpl::Update(float delta_time) {
    physx::PxVehicleDrive4WSmoothDigitalRawInputsAndSetAnalogInputs(
        gKeySmoothingData, gSteerVsForwardSpeedTable, m_input_data, delta_time,
        false, *GetUnderlying());
    m_input_data = physx::PxVehicleDrive4WRawInputData{};
}

physx::PxVehicleDrive4W* Vehicle4WDriveImpl::GetUnderlying() {
    return static_cast<physx::PxVehicleDrive4W*>(m_drive);
}

VehicleNWDriveImpl::VehicleNWDriveImpl() : VehicleDriveImpl{Type::N_Wheel} {}

VehicleNWDriveImpl::VehicleNWDriveImpl(
    ContextImpl& ctx, VehicleManagerImpl& mgr,
    const VehicleWheelSimDescriptor& wheel_sim_desc,
    const VehicleDriveSimNWDescriptor& drive_sim_desc,
    const RigidDynamic& actor)
    : VehicleDriveImpl{Type::N_Wheel}, m_mgr{&mgr} {
    physx::PxVehicleWheelsSimData* wheel_sim_data =
        VehicleWheelSimDescriptor2PhysX(wheel_sim_desc);

    physx::PxVehicleDriveSimDataNW drive_sim_data =
        VehicleDriveSimNWDescriptor2PhysX(drive_sim_desc);

    auto wheel_num = wheel_sim_desc.GetWheelNum();
    auto drive = physx::PxVehicleDriveNW::allocate(wheel_num);

    if (!drive) {
        LOGE("vehicle create failed");
        wheel_sim_data->free();
        return;
    }

    drive->setup(ctx.m_physics,
                 static_cast<physx::PxRigidDynamic*>(actor.GetImpl()->m_actor),
                 *wheel_sim_data, drive_sim_data, wheel_num);

    wheel_sim_data->free();
    m_actor = actor;
    drive->setToRestState();
    drive->mDriveDynData.forceGearChange(physx::PxVehicleGearsData::eNEUTRAL);

    m_drive = drive;
}

VehicleNWDriveImpl::~VehicleNWDriveImpl() {
    if (m_drive) {
        m_drive->release();
    }
}

void VehicleNWDriveImpl::DecRefcount() {
    RefCountable::DecRefcount();

    if (Refcount() == 0) {
        m_mgr->m_nw_allocator.MarkAsGarbage(this);
        m_mgr->m_pending_delete.push_back(this);
    }
}

void VehicleNWDriveImpl::SetDigitalAccel(bool active) {
    m_input_data.setDigitalAccel(active);
}

void VehicleNWDriveImpl::SetDigitalBrake(bool active) {
    m_input_data.setDigitalBrake(active);
}

void VehicleNWDriveImpl::SetDigitalHandbrake(bool active) {
    m_input_data.setDigitalHandbrake(active);
}

void VehicleNWDriveImpl::SetDigitalSteerLeft(bool active) {
    m_input_data.setDigitalSteerLeft(active);
}

void VehicleNWDriveImpl::SetDigitalSteerRight(bool active) {
    m_input_data.setDigitalSteerRight(active);
}

void VehicleNWDriveImpl::SetAnalogAccel(float value) {
    m_input_data.setAnalogAccel(value);
}

void VehicleNWDriveImpl::SetAnalogBrake(float value) {
    m_input_data.setAnalogBrake(value);
}

void VehicleNWDriveImpl::SetAnalogHandbrake(float value) {
    m_input_data.setAnalogHandbrake(value);
}

void VehicleNWDriveImpl::SetAnalogSteerLeft(float value) {
    m_input_data.setDigitalSteerLeft(value);
}

void VehicleNWDriveImpl::SetAnalogSteerRight(float value) {
    m_input_data.setDigitalSteerRight(value);
}

void VehicleNWDriveImpl::SetGearUp(bool active) {
    m_input_data.setGearUp(active);
}

void VehicleNWDriveImpl::SetGearDown(bool active) {
    m_input_data.setGearDown(active);
}

void VehicleNWDriveImpl::Update(float delta_time) {
    physx::PxVehicleDriveNWSmoothDigitalRawInputsAndSetAnalogInputs(
        gKeySmoothingData, gSteerVsForwardSpeedTable, m_input_data, delta_time,
        false, *GetUnderlying());
    m_input_data = physx::PxVehicleDriveNWRawInputData{};
}

physx::PxVehicleDriveNW* VehicleNWDriveImpl::GetUnderlying() {
    return static_cast<physx::PxVehicleDriveNW*>(m_drive);
}

VehicleTankDriveImpl::VehicleTankDriveImpl()
    : VehicleDriveImpl{Type::Tank},
      m_input_data{physx::PxVehicleDriveTankControlModel::eSTANDARD} {}

VehicleTankDriveImpl::VehicleTankDriveImpl(
    ContextImpl& ctx, VehicleManagerImpl& mgr, VehicleTankDriveMode drive_mode,
    const VehicleWheelSimDescriptor& wheel_sim_desc,
    const VehicleDriveSimDescriptor& drive_sim_desc, const RigidDynamic& actor)
    : VehicleDriveImpl{Type::Tank},
      m_mgr{&mgr},
      m_input_data{VehicleTankDriveMode2PhysX(drive_mode)} {
    physx::PxVehicleWheelsSimData* wheel_sim_data =
        VehicleWheelSimDescriptor2PhysX(wheel_sim_desc);

    physx::PxVehicleDriveSimData drive_sim_data =
        VehicleDriveSimDescriptor2PhysX(drive_sim_desc);

    auto wheel_num = wheel_sim_desc.GetWheelNum();
    auto drive = physx::PxVehicleDriveTank::allocate(wheel_num);

    if (!drive) {
        LOGE("vehicle create failed");
        wheel_sim_data->free();
        return;
    }

    drive->setup(ctx.m_physics,
                 static_cast<physx::PxRigidDynamic*>(actor.GetImpl()->m_actor),
                 *wheel_sim_data, drive_sim_data, wheel_num);

    wheel_sim_data->free();
    m_actor = actor;
    drive->setToRestState();
    drive->mDriveDynData.forceGearChange(physx::PxVehicleGearsData::eNEUTRAL);

    m_drive = drive;
}

VehicleTankDriveImpl::~VehicleTankDriveImpl() {
    if (m_drive) {
        m_drive->release();
    }
}

void VehicleTankDriveImpl::DecRefcount() {
    RefCountable::DecRefcount();

    if (Refcount() == 0) {
        m_mgr->m_tank_allocator.MarkAsGarbage(this);
        m_mgr->m_pending_delete.push_back(this);
    }
}

void VehicleTankDriveImpl::SetDigitalAccel(bool acc) {
    m_input_data.setDigitalAccel(acc);
}

void VehicleTankDriveImpl::SetDigitalLeftBrake(bool brake) {
    m_input_data.setDigitalLeftBrake(brake);
}

void VehicleTankDriveImpl::SetDigitalRightBrake(bool brake) {
    m_input_data.setDigitalRightBrake(brake);
}

void VehicleTankDriveImpl::SetDigitalLeftThrust(bool thrust) {
    m_input_data.setDigitalLeftThrust(thrust);
}

void VehicleTankDriveImpl::SetDigitalRightThrust(bool thrust) {
    m_input_data.setDigitalRightThrust(thrust);
}

void VehicleTankDriveImpl::SetGearUp(bool gear) {
    m_input_data.setGearUp(gear);
}

void VehicleTankDriveImpl::SetGearDown(bool gear) {
    m_input_data.setGearDown(gear);
}

void VehicleTankDriveImpl::Update(float delta_time) {
    physx::PxVehicleDriveTankSmoothDigitalRawInputsAndSetAnalogInputs(
        gKeySmoothingData, m_input_data, delta_time, *GetUnderlying());
    m_input_data =
        physx::PxVehicleDriveTankRawInputData{m_input_data.getDriveModel()};
}

physx::PxVehicleDriveTank* VehicleTankDriveImpl::GetUnderlying() {
    return static_cast<physx::PxVehicleDriveTank*>(m_drive);
}

VehicleNoDriveImpl::VehicleNoDriveImpl() : VehicleDriveImpl{Type::NoDrive} {}

VehicleNoDriveImpl::VehicleNoDriveImpl(
    ContextImpl& ctx, VehicleManagerImpl& mgr,
    const VehicleWheelSimDescriptor& wheel_sim_desc, const RigidDynamic& actor)
    : VehicleDriveImpl{Type::NoDrive}, m_mgr{&mgr} {
    physx::PxVehicleWheelsSimData* wheel_sim_data =
        VehicleWheelSimDescriptor2PhysX(wheel_sim_desc);

    auto wheel_num = wheel_sim_desc.GetWheelNum();
    auto drive = physx::PxVehicleNoDrive::allocate(wheel_num);

    if (!drive) {
        LOGE("vehicle create failed");
        wheel_sim_data->free();
        return;
    }

    drive->setup(ctx.m_physics,
                 static_cast<physx::PxRigidDynamic*>(actor.GetImpl()->m_actor),
                 *wheel_sim_data);

    wheel_sim_data->free();
    m_actor = actor;
    drive->setToRestState();

    m_drive = drive;
}

VehicleNoDriveImpl::~VehicleNoDriveImpl() {
    if (m_drive) {
        m_drive->release();
    }
}

void VehicleNoDriveImpl::DecRefcount() {
    RefCountable::DecRefcount();

    if (Refcount() == 0) {
        m_mgr->m_no_drive_allocator.MarkAsGarbage(this);
        m_mgr->m_pending_delete.push_back(this);
    }
}

void VehicleNoDriveImpl::SetDriveTorque(uint32_t wheel_idx, float torque) {
    GetUnderlying()->setDriveTorque(wheel_idx, torque);
}

void VehicleNoDriveImpl::SetSteerAngle(uint32_t wheel_idx, Radians angle) {
    GetUnderlying()->setSteerAngle(wheel_idx, angle.Value());
}

void VehicleNoDriveImpl::SetBrakeTorque(uint32_t wheel_idx, float torque) {
    GetUnderlying()->setBrakeTorque(wheel_idx, torque);
}

void VehicleNoDriveImpl::Update(float) {}

physx::PxVehicleNoDrive* VehicleNoDriveImpl::GetUnderlying() {
    return static_cast<physx::PxVehicleNoDrive*>(m_drive);
}

VehicleManagerImpl::VehicleManagerImpl(ContextImpl& ctx, SceneImpl& scene)
    : m_ctx{ctx}, m_scene{scene} {
    setupFrictionPairs();
}

VehicleManagerImpl::~VehicleManagerImpl() {
    m_tank_allocator.FreeAll();
    m_nw_allocator.FreeAll();
    m_4w_allocator.FreeAll();
}

Vehicle4WDriveImpl* VehicleManagerImpl::CreateVehicle4WDrive(
    const VehicleWheelSimDescriptor& wheel,
    const VehicleDriveSim4WDescriptor& drive, const RigidDynamic& actor) {
    m_wheel_num += wheel.GetWheelNum();
    auto vehicle = m_4w_allocator.Allocate(m_ctx, *this, wheel, drive, actor);
    m_vehicles.push_back(vehicle);
    return vehicle;
}

VehicleNWDriveImpl* VehicleManagerImpl::CreateVehicleNWDrive(
    const VehicleWheelSimDescriptor& wheel,
    const VehicleDriveSimNWDescriptor& drive, const RigidDynamic& actor) {
    m_wheel_num += wheel.GetWheelNum();
    auto vehicle = m_nw_allocator.Allocate(m_ctx, *this, wheel, drive, actor);
    m_vehicles.push_back(vehicle);
    return vehicle;
}

VehicleTank VehicleManagerImpl::CreateVehicleTankDrive(
    VehicleTankDriveMode drive_mode, const VehicleWheelSimDescriptor& wheel,
    const VehicleDriveSimDescriptor& drive, const RigidDynamic& actor) {
    m_wheel_num += wheel.GetWheelNum();
    auto vehicle = m_tank_allocator.Allocate(m_ctx, *this, drive_mode, wheel,
                                             drive, actor);
    m_vehicles.push_back(vehicle);
    return vehicle;
}

VehicleNoDrive VehicleManagerImpl::CreateVehicleNoDrive(
    const VehicleWheelSimDescriptor& wheel, const RigidDynamic& actor) {
    m_wheel_num += wheel.GetWheelNum();
    auto vehicle = m_no_drive_allocator.Allocate(m_ctx, *this, wheel, actor);
    m_vehicles.push_back(vehicle);
    return vehicle;
}

void VehicleManagerImpl::Update(float delta_time) {
    tryRecreateBatchQuery();

    std::vector<physx::PxVehicleWheels*> wheels;
    wheels.reserve(m_vehicles.size());
    for (auto vehicle : m_vehicles) {
        vehicle->Update(delta_time);
        wheels.push_back(vehicle->m_drive);
    }
    physx::PxVehicleSuspensionRaycasts(m_batch_query, m_vehicles.size(),
                                       wheels.data());

    physx::PxVehicleUpdates(delta_time, {0, -9.8, 0}, *m_friction_pairs,
                            wheels.size(), wheels.data(), nullptr);
}

void VehicleManagerImpl::GC() {
    deletePendingVehicles();
    m_4w_allocator.GC();
    m_nw_allocator.GC();
    m_tank_allocator.GC();
}

void VehicleManagerImpl::deletePendingVehicles() {
    for (auto vehicle : m_pending_delete) {
        auto it = std::find(m_vehicles.begin(), m_vehicles.end(), vehicle);
        if (it != m_vehicles.end()) {
            m_wheel_num -= vehicle->m_drive->mWheelsSimData.getNbWheels();
            m_vehicles.erase(it);
        }
    }
    m_pending_delete.clear();
}

void VehicleManagerImpl::tryRecreateBatchQuery() {
    if (m_wheel_num >= m_batch_result_num) {
        uint32_t batch_num =
            std::ceil(m_wheel_num / (float)BatchResultExpandStep) *
            BatchResultExpandStep;
        uint32_t touch_num =
            std::ceil(m_wheel_num / (4 * (float)BatchResultExpandStep)) * 4 *
            BatchResultExpandStep;
        recreateBatchQuery(batch_num, touch_num);
    }
}

void VehicleManagerImpl::recreateBatchQuery(uint32_t batch_result_num,
                                            uint32_t batch_touch_num) {
    if (m_batch_query) {
        m_batch_query->release();
    }

    m_batch_query = physx::PxCreateBatchQueryExt(
        *m_scene.m_scene, &m_filter_shader, batch_result_num, batch_touch_num,
        batch_result_num, batch_touch_num, batch_result_num, batch_touch_num);
    if (m_batch_query) {
        m_batch_result_num = batch_result_num;
        m_batch_touch_num = batch_touch_num;
    } else {
        m_batch_result_num = 0;
        m_batch_touch_num = 0;
    }
}

void VehicleManagerImpl::setupFrictionPairs() {
    physx::PxVehicleDrivableSurfaceType surface_types[1];
    surface_types[0].mType = 0;

    std::array<const physx::PxMaterial*, 1> surface_materials;
    surface_materials[0] = {m_ctx.m_physics->createMaterial(0.1, 0.1, 0.1)};

    m_friction_pairs =
        physx::PxVehicleDrivableSurfaceToTireFrictionPairs::allocate(1, 1);

    m_friction_pairs->setup(1, 1, surface_materials.data(), surface_types);

    for (uint32_t i = 0; i < 1; i++) {
        for (uint32_t j = 0; j < 1; j++) {
            m_friction_pairs->setTypePairFriction(i, j, 1.0);
        }
    }
}

}  // namespace nickel::physics