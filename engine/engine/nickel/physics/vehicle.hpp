#pragma once
#include "nickel/common/impl_wrapper.hpp"
#include "nickel/common/math/math.hpp"
#include "nickel/physics/filter.hpp"
#include <span>

namespace nickel::physics {

class RigidDynamic;

constexpr uint32_t VehicleMaxWheelNum = 20;

struct VehicleWheelSimDescriptor {
    enum class Type {
        FourWheel,
        ArbitraryWheel,
    };

    struct Tire {
        float m_lat_stiff_x = 2.0f;
        Radians m_lat_stiff_y{0.3125f};
        float m_longitudinal_stiffness_per_unit_gravity = 1000.0f;
        Radians m_camber_stiffness_per_unit_gravity{0.1};  // in kg/rad
        float m_friction_vs_slip_graph[3][2] = {0.0, 1.0, 0.1, 1.0, 1.0, 1.0};
        uint32_t m_type{};
    };

    struct Wheel {
        float m_radius = 0.5f;
        float m_width = 0.2;
        float m_mass = 20;
        float m_moi = 1.0f;
        float m_damping_rate = 0.25f;
        float m_max_hand_brake_torque = 0.0f;
        float m_max_brake_torque = 1500.0f;
        Radians m_max_steer = PI * 0.3333f;  // in [0, FLT_MAX)
        Radians m_toe_angle = 0.0f;          // in [0, PI/2]
    };

    struct Suspension {
        float m_spring_strength = 0.2f;
        float m_spring_damper_rate = 0.0f;
        float m_max_compression = 0.3f;
        float m_max_droop = 0.1f;
        float m_sprung_mass = 1.0f;
        Radians m_camber_at_rest = 0.0f;             // in [-PI/2, PI/2]
        Radians m_camber_at_max_compression = 0.0f;  // @brief in [-PI/2, PI/2]
        Radians m_camber_at_max_droop = 0.0f;        // in [-PI/2, PI/2]
    };

    struct WheelDescriptor {
        Wheel m_wheel;
        Tire m_tire;
        Suspension m_suspension;
        FilterData m_scene_query_filter_data;
        Vec3 m_suspension_travel_directions = {0, -1, 0};
        Vec3 m_wheel_centre_cm_offsets;
        Vec3 m_suspension_force_app_point_offsets = {0, 0, 0};
        Vec3 m_tire_force_app_cm_offsets = {0, 0, 0};
    };

    float m_chassis_mass = 1.0f;
    std::vector<WheelDescriptor> m_wheels;
    // AntiRollBar m_anti_roll_bar;

    uint32_t GetWheelNum() const;
    Type GetType() const;

protected:
    VehicleWheelSimDescriptor(Type);

private:
    Type m_type;
};

struct VehicleWheelSim4WDescriptor : public VehicleWheelSimDescriptor {
    VehicleWheelSim4WDescriptor();

    std::optional<uint32_t> m_rear_left_wheel;
    std::optional<uint32_t> m_rear_right_wheel;
    std::optional<uint32_t> m_front_left_wheel;
    std::optional<uint32_t> m_front_right_wheel;
};

struct VehicleEngineDescriptor {
    float m_moi = 1.0f;
    float m_peak_torque = 500.0f;
    float m_max_omega = 600.0f;
    float m_damping_rate_full_throttle = 0.15f;
    float m_damping_rate_zero_throttle_clutch_engaged = 2.0f;
    float m_damping_rate_zero_throttle_clutch_disengaged = 0.35f;
};

struct VehicleGearDescriptor {
    float m_reverse_ratio = -4.0f;
    float m_neutral_ratio = 0.0f;
    float m_first_ratio = 4.0f;
    std::vector<float> m_other_forward_ratios;
    float m_final_ratio = 4.0f;
    float m_switch_time = 0.5f;
};

struct VehicleClutchDescriptor {
    enum class AccuracyMode { Estimate = 0, BestPossible };

    float m_strength = 10.0f;
    AccuracyMode m_accuracy_mode = AccuracyMode::BestPossible;
    uint32_t m_estimate_iterations = 5;
};

struct VehicleDriveSimDescriptor {
    enum class Type {
        FourWheel,
        ArbitraryWheel,
    };

    Type GetType() const;

    VehicleEngineDescriptor m_engine;
    VehicleGearDescriptor m_gear;
    VehicleClutchDescriptor m_clutch;

protected:
    explicit VehicleDriveSimDescriptor(Type type);

private:
    Type m_type;
};

struct VehicleDifferential4WDescriptor {
    enum class Type {
        LS_4_WD,
        LS_Front_WD,
        LS_Rear_WD,
        Open_4_WD,
        Open_Front_WD,
        Open_Rear_WD,
    } m_type = Type::LS_4_WD;
    float m_front_rear_split = 0.5f;
    float m_front_left_right_split = 0.5f;
    float m_rear_left_right_split = 0.5f;
    float m_centre_bias = 1.0f;
    float m_front_bias = 1.0f;
    float m_rear_bias = 1.0f;
};

struct VehicleAckermannGeometryDescriptor {
    float m_accuracy = 0;
    float m_front_width = 0.1f;
    float m_rear_width = 0.1f;
    float m_axle_separation = 0.1f;
};

struct VehicleDifferentialNWDescriptor {
    void SetDrivenWheel(uint32_t idx);
    bool GetDrivenWheel(uint32_t idx) const;

private:
    std::array<bool, VehicleMaxWheelNum> m_wheels{};
};

struct VehicleDriveSim4WDescriptor : public VehicleDriveSimDescriptor {
    VehicleDriveSim4WDescriptor();

    VehicleAckermannGeometryDescriptor m_ackermann;
    VehicleDifferential4WDescriptor m_diff;
};

struct VehicleDriveSimNWDescriptor : public VehicleDriveSimDescriptor {
    VehicleDriveSimNWDescriptor();

    VehicleDifferentialNWDescriptor m_diff;
};

class Vehicle4WDriveImpl;
class VehicleDriveImpl;

class Vehicle4W: public ImplWrapper<Vehicle4WDriveImpl> {
public:
    using ImplWrapper::ImplWrapper;
    
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
};

class VehicleNWDriveImpl;

class VehicleNW : public ImplWrapper<VehicleNWDriveImpl> {
public:
    using ImplWrapper::ImplWrapper;

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
};

class Vehicle: public ImplWrapper<VehicleDriveImpl> {
public:
    using ImplWrapper::ImplWrapper;

    Vehicle(Vehicle4W vehicle);
    Vehicle(VehicleNW vehicle);
    
    Vehicle& operator=(Vehicle4W vehicle);
    Vehicle& operator=(VehicleNW vehicle);

    Vehicle4W CastAs4W() const;
    VehicleNW CastAsNW() const;
};

class VehicleManagerImpl;
class ContextImpl;
class SceneImpl;

std::vector<float> ComputeVehicleSprungMass(
    std::span<const Vec3> sprung_mass_coord, const Vec3& center_of_mass,
    float totle_mass);

class VehicleManager {
public:
    VehicleManager(ContextImpl&, SceneImpl&);
    ~VehicleManager();

    Vehicle4W CreateVehicle4WDrive(const VehicleWheelSimDescriptor&,
                                   const VehicleDriveSim4WDescriptor&,
                                   const RigidDynamic&);
    VehicleNW CreateVehicleNWDrive(const VehicleWheelSimDescriptor&,
                                   const VehicleDriveSimNWDescriptor&,
                                   const RigidDynamic&);
    void Update(float delta_time);
    void GC();

private:
    std::unique_ptr<VehicleManagerImpl> m_impl;
};

}  // namespace nickel::physics