#pragma once

#include "nickel/nickel.hpp"

struct WheelMeshData {
    nickel::graphics::GLTFModel m_render_mesh;
    uint32_t m_physics_mesh;
};

struct VehicleParams {
    nickel::physics::Vehicle::Type m_vehicle_type;
    nickel::physics::VehicleWheelSim4WDescriptor m_wheel_sim_desc;
    nickel::physics::VehicleDriveSim4WDescriptor m_drive_4w_sim_desc;
    nickel::physics::VehicleDriveSimNWDescriptor m_drive_nw_sim_desc;
    nickel::physics::VehicleSteerVsForwardTable m_steer_vs_forward_table;

    WheelMeshData m_chassis_mesh;
    std::optional<uint32_t> m_front_left_wheel_idx, m_front_right_wheel_idx,
        m_rear_left_wheel_idx, m_rear_right_wheel_idx;
    std::vector<uint32_t> m_driving_wheels;  // only use for NW drive
    std::vector<nickel::graphics::GLTFVertexData> m_physics_meshes;
    std::vector<WheelMeshData> m_wheel_meshes;
};

namespace imgui_window {
    class ImGuiIDGenerator {
    public:
        static uint32_t GenID() {
            static uint32_t m_id{};
            return m_id++;
        }
    };

    class ImGuiWindow {
    public:
        ImGuiWindow(const std::string& title);

        virtual ~ImGuiWindow();

        virtual void Update();

        void SetTitle(const std::string& title);

        const std::string& GetTitle() const;

        void SetVisible(bool show);

    protected:
        virtual void update() = 0;

        std::string m_title;
        bool m_show{false};
        uint32_t m_id;
    };

    class ImGuiPopupWindow : public ImGuiWindow {
    public:
        ImGuiPopupWindow(const std::string& title);

        void Update() override;

        void Close();

        void Open();

    private:
        bool m_should_close{false};
        bool m_should_open{false};
    };

}  // namespace imgui_window

class ChoosingVehicleTypePopupWindow : public imgui_window::ImGuiPopupWindow {
public:
    ChoosingVehicleTypePopupWindow(VehicleParams& params);

protected:
    void update() override;

private:
    std::array<const char*, 4> m_vehicle_type_names = {"four wheel", "N-wheel",
                                                       "tank", "no-drive"};
    VehicleParams& m_params;
    int m_wheel_num{};
};

nickel::physics::Vehicle createVehicle(const VehicleParams&,
                                       nickel::GameObject& chassis_go);

class ChoosingVehicleComponentPopupWindow
    : public imgui_window::ImGuiPopupWindow {
public:
    ChoosingVehicleComponentPopupWindow(VehicleParams& params);

    void SetWheelNum(uint32_t wheel_num);
    void EnabelSelectChassis(bool);

protected:
    void update() override;

    void initVehicleModel();
    void initDefaultPhysicsVehicle(nickel::physics::Vehicle::Type type);

private:
    VehicleParams& m_params;
    bool m_is_driving_wheel{false};
    uint32_t m_wheel_num{};
    bool m_is_selecting_chassis{false};

    uint32_t findPhysicsMesh(const std::string& name) const;
};


class TunningPanel : public imgui_window::ImGuiWindow {
public:
    TunningPanel(VehicleParams& params);

protected:
    enum class WheelTunningMode {
        SingleWheel = 0,
        AllWheel,
    } m_tunning_mode = WheelTunningMode::AllWheel;

    VehicleParams& m_params;

    void update() override;

    void loadGLTFModel(const nickel::Path& path);
    std::vector<nickel::graphics::GLTFVertexData> loadPhysicsMesh(
        const nickel::Path& path);

    void displayParamTunning();
    void tunningEngine(nickel::physics::VehicleEngineDescriptor& engine);
    void tunningDiff(nickel::physics::VehicleDifferential4WDescriptor& diff);
    void tunningCluth(nickel::physics::VehicleClutchDescriptor& clutch);
    void tunningGear(nickel::physics::VehicleGearDescriptor& gear);
    void tunningAckerMann(
        nickel::physics::VehicleAckermannGeometryDescriptor& ackermann);
    void tunningWheel4WSimData(
        nickel::physics::VehicleWheelSim4WDescriptor& wheel);
    void tunningWheelSimData(nickel::physics::VehicleWheelSimDescriptor& wheel);
    void tunningWheel(
        const char* title,
        nickel::physics::VehicleWheelSimDescriptor::WheelDescriptor& wheel);
    void tunningWheelCommonConfig(
        nickel::physics::VehicleWheelSimDescriptor::WheelDescriptor& wheel);
    void tunningWheelUniqueConfig(
        nickel::physics::VehicleWheelSimDescriptor::WheelDescriptor& wheel);
    void tunningSteerVsForwardTable(
        nickel::physics::VehicleSteerVsForwardTable& table);
    void copyCommonConfig2Wheel(
        nickel::physics::VehicleWheelSimDescriptor::WheelDescriptor& src,
        nickel::physics::VehicleWheelSimDescriptor::WheelDescriptor& dst);
};

struct ImGuiWindowManager : public nickel::Singlton<ImGuiWindowManager, true> {
    TunningPanel m_tunning_panel;
    ChoosingVehicleTypePopupWindow m_choosing_vehicle_type_window;
    ChoosingVehicleComponentPopupWindow m_choosing_component_window;

    ImGuiWindowManager(VehicleParams&);

    void Update();
};
