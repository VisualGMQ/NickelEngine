#include "vehicle_debug_panel.hpp"
#include "nickel/nickel.hpp"
#include "nickel/physics/vehicle.hpp"

template <typename T>
void DisplayFiled(std::string_view name, T& value);

template <>
void DisplayFiled<float>(std::string_view name, float& value) {
    ImGui::DragFloat(name.data(), &value);
}

template <>
void DisplayFiled<nickel::Vec3>(std::string_view name, nickel::Vec3& value) {
    ImGui::DragFloat3(name.data(), value.Ptr());
}

template <>
void DisplayFiled<nickel::Radians>(std::string_view name,
                                   nickel::Radians& value) {
    float degrees = nickel::Degrees{value}.Value();
    ImGui::DragFloat(name.data(), &degrees);
    value = nickel::Degrees{degrees};
}

void ShowWheel(nickel::physics::VehicleWheelSimDescriptor::Wheel& wheel) {
    ImGui::SeparatorText("wheel");
    DisplayFiled("damping rate", wheel.m_damping_rate);
    DisplayFiled("mass", wheel.m_mass);
    DisplayFiled("max brake torque", wheel.m_max_brake_torque);
    DisplayFiled("max hand brake torque", wheel.m_max_hand_brake_torque);
    DisplayFiled("max steer", wheel.m_max_steer);
    DisplayFiled("MOI", wheel.m_moi);
    DisplayFiled("toe angle", wheel.m_toe_angle);
    DisplayFiled("radius", wheel.m_radius);
    DisplayFiled("width", wheel.m_width);
}

void ShowTire(nickel::physics::VehicleWheelSimDescriptor::Tire& tire) {
    ImGui::SeparatorText("tire");
    DisplayFiled("last stiff x", tire.m_last_stiff_x);
    DisplayFiled("last stiff y", tire.m_last_stiff_y);
    DisplayFiled("longitudinal stiffness per unit gravity",
                 tire.m_longitudinal_stiffness_per_unit_gravity);
    DisplayFiled("camber stiffness per unity gravity",
                 tire.m_camber_stiffness_per_unit_gravity);
}

void ShowSuspension(
    nickel::physics::VehicleWheelSimDescriptor::Suspension& susp) {
    ImGui::SeparatorText("suspension");
    DisplayFiled("sprung mass", susp.m_sprung_mass);
    DisplayFiled("spring strength", susp.m_spring_strength);
    DisplayFiled("spring damper rate", susp.m_spring_damper_rate);
    DisplayFiled("max droop", susp.m_max_droop);
    DisplayFiled("max compression", susp.m_max_compression);
    DisplayFiled("camber at rest", susp.m_camber_at_rest);
    DisplayFiled("camber at max droop", susp.m_camber_at_max_droop);
    DisplayFiled("camber at max compression", susp.m_camber_at_max_compression);
}

void ShowWheel(
    std::string_view name,
    nickel::physics::VehicleWheelSimDescriptor::WheelDescriptor& wheel) {
    if (ImGui::TreeNode(name.data())) {
        ShowWheel(wheel.m_wheel);
        DisplayFiled("wheel centre CM offset", wheel.m_wheel_centre_cm_offsets);
        ShowTire(wheel.m_tire);
        DisplayFiled("tire force app cm offset",
                     wheel.m_tire_force_app_cm_offsets);
        ShowSuspension(wheel.m_suspension);
        DisplayFiled("suspension force app point offset",
                     wheel.m_suspension_force_app_point_offsets);
        ImGui::TreePop();
    }
}

void ShowVehicleDebugPanel(
    nickel::GameObject& go, nickel::physics::VehicleWheelSimDescriptor& wheel,
    nickel::physics::VehicleDriveSim4WDescriptor& drive) {
    if (ImGui::Begin("Vehicle Debug Panel")) {
        if (ImGui::Button("apply")) {
            auto& physics = nickel::Context::GetInst().GetPhysicsContext();
            go.m_vehicle = physics.GetVehicleManager().CreateVehicle4WDrive(
                wheel, drive,
                static_cast<const nickel::physics::RigidDynamic&>(
                    go.m_rigid_actor));
        }
        {
            ImGui::SeparatorText("wheel");
            DisplayFiled("chassis mass", wheel.m_chassis_mass);
            ShowWheel("front left wheel", wheel.m_front_left_wheel);
            ShowWheel("front right wheel", wheel.m_front_right_wheel);
            ShowWheel("rear left wheel", wheel.m_rear_left_wheel);
            ShowWheel("rear right wheel", wheel.m_rear_right_wheel);
        }

        {
            ImGui::SeparatorText("drive");
        }
    }
    ImGui::End();
}