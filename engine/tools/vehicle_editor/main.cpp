#include "nickel/graphics/internal/gltf_model_impl.hpp"
#include "nickel/main_entry/runtime.hpp"
#include "nickel/nickel.hpp"

class Application : public nickel::Application {
public:
    void OnInit() override {
        auto& ctx = nickel::Context::GetInst();
        ctx.GetWindow().SetTitle("vehicle editor");

        auto frustum = ctx.GetCamera().GetFrustum();
        auto camera = std::make_unique<nickel::OrbitCamera>(
            frustum.fov, frustum.aspect, frustum.near, frustum.far);
        camera->SetRadius(10);
        camera->SetTheta(0);
        camera->SetPhi(nickel::Degrees{30});
        ctx.ChangeCamera(std::move(camera));

        auto& root_go = ctx.GetCurrentLevel().GetRootGO();

        // create physics plane
        {
            auto& physics_ctx = ctx.GetPhysicsContext();

            nickel::GameObject go;
            go.m_name = "plane";
            go.m_rigid_actor =
                nickel::physics::RigidActor{physics_ctx.CreateRigidStatic(
                    nickel::Vec3{}, nickel::Quat::Create(nickel::Vec3{0, 0, 1},
                                                         nickel::Degrees{90}))};
            auto material = physics_ctx.CreateMaterial(1.0, 1.0, 0.1);
            auto shape = physics_ctx.CreateShape(
                nickel::physics::PlaneGeometry{}, material);
            go.m_rigid_actor.AttachShape(shape);
            physics_ctx.GetMainScene().AddRigidActor(go.m_rigid_actor);
            root_go.m_children.emplace_back(std::move(go));
        }
    }

    void OnUpdate() override {
        ImPlot::ShowDemoWindow();

        displayTunningPanel();
        updateCamera();
        drawGrid();
    }

private:
    enum class State {
        Idle = 0,
        ChoosingVehicleType,
        ChoosingChassisMesh,
        ChoosingFrontLeftWheel,
        ChoosingFrontRightWheel,
        ChoosingRearLeftWheel,
        ChoosingRearRightWheel,
        ChoosingOtherWheels,
        MakeupVehicle,
        Tunning,
    } m_state = State::Idle;

    enum class WheelTunningMode {
        SingleWheel = 0,
        AllWheel,
    } m_tunning_mode = WheelTunningMode::AllWheel;

    std::optional<nickel::physics::Vehicle::Type> m_vehicle_type;
    int m_wheel_num{};

    std::string_view m_popup_title_name;

    struct WheelMeshData {
        nickel::graphics::GLTFModel m_render_mesh;
        uint32_t m_physics_mesh;
    };

    nickel::GameObject* m_chassis_go{};

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

    void updateCamera() {
        auto& ctx = nickel::Context::GetInst();
        nickel::OrbitCamera& camera =
            static_cast<nickel::OrbitCamera&>(ctx.GetCamera());
        auto& device_mgr = ctx.GetDeviceManager();
        auto& mouse = device_mgr.GetMouse();

        constexpr nickel::Radians rotate_speed = nickel::Degrees{0.1};
        constexpr float scale_speed = 0.5f;

        if (mouse.GetButton(nickel::input::Mouse::Button::Type::Left)
                .IsPressing()) {
            auto& offset = mouse.GetOffset();
            camera.AddTheta(offset.x * rotate_speed);
            camera.AddPhi(offset.y * rotate_speed);

            float wheel = mouse.GetWheelDelta();
            if (wheel != 0) {
                camera.AddRadius(-wheel * scale_speed);
            }
        }
    }

    void drawGrid() {
        auto& ctx = nickel::Context::GetInst().GetGraphicsContext();
        ctx.SetClearColor(nickel::Color{0.1, 0.1, 0.1, 1.0f});

        constexpr uint8_t HalfLineNum = 10;
        nickel::Color color{0.7, 0.7, 0.7, 1.0};
        for (int i = -HalfLineNum; i <= HalfLineNum; i++) {
            if (i == 0) {
                continue;
            }
            nickel::graphics::Vertex vertices[] = {
                nickel::graphics::Vertex{nickel::Vec3(i, 0,  HalfLineNum),
                                         color},
                nickel::graphics::Vertex{nickel::Vec3(i, 0, -HalfLineNum),
                                         color}
            };
            ctx.DrawLineList(vertices);
        }

        for (int i = -HalfLineNum; i <= HalfLineNum; i++) {
            if (i == 0) {
                continue;
            }
            nickel::graphics::Vertex vertices[] = {
                nickel::graphics::Vertex{ nickel::Vec3(HalfLineNum, 0, i),
                                         color},
                nickel::graphics::Vertex{nickel::Vec3(-HalfLineNum, 0, i),
                                         color}
            };
            ctx.DrawLineList(vertices);
        }

        // draw axis
        {
            nickel::graphics::Vertex vertices[] = {
                nickel::graphics::Vertex{          nickel::Vec3(0, 0, 0),
                                         nickel::Color{1, 0, 0, 1}},
                nickel::graphics::Vertex{nickel::Vec3(HalfLineNum, 0, 0),
                                         nickel::Color{1, 0, 0, 1}}
            };
            ctx.DrawLineList(vertices);
        }
        {
            nickel::graphics::Vertex vertices[] = {
                nickel::graphics::Vertex{nickel::Vec3(0,           0, 0),
                                         nickel::Color{0, 1, 0, 1}},
                nickel::graphics::Vertex{nickel::Vec3(0, HalfLineNum, 0),
                                         nickel::Color{0, 1, 0, 1}}
            };
            ctx.DrawLineList(vertices);
        }
        {
            nickel::graphics::Vertex vertices[] = {
                nickel::graphics::Vertex{nickel::Vec3(0, 0,           0),
                                         nickel::Color{0, 0, 1, 1}},
                nickel::graphics::Vertex{nickel::Vec3(0, 0, HalfLineNum),
                                         nickel::Color{0, 0, 1, 1}}
            };
            ctx.DrawLineList(vertices);
        }
    }

    void displayTunningPanel() {
        auto& ctx = nickel::Context::GetInst();

        if (ImGui::Begin("Tunning Panel")) {
            if (ImGui::Button("create new vehicle")) {
                nickel::FileDialog dialog =
                    nickel::FileDialog::CreateOpenFileDialog()
                        .AddFilter("gltf model", "gltf")
                        .SetTitle("load gltf model")
                        .Open();
                auto& selected = dialog.GetSelected();
                if (selected.empty()) {
                    ImGui::End();
                    return;
                }
                loadGLTFModel(selected.back());
                m_physics_meshes = loadPhysicsMesh(selected.back());
                m_state = State::ChoosingVehicleType;
                m_popup_title_name = "select vehicle";
                ImGui::OpenPopup(m_popup_title_name.data());
            }

            // select vehicle window
            std::string_view select_vehicle_type_window_name =
                "select vehicle type";
            {
                std::array<const char*, 4> vehicle_type_names = {
                    "four wheel", "N-wheel", "tank", "no-drive"};
                if (ImGui::BeginPopupModal(
                        select_vehicle_type_window_name.data())) {
                    if (ImGui::BeginCombo("vehicle type", nullptr)) {
                        for (int i = 0; i < vehicle_type_names.size(); i++) {
                            if (ImGui::Selectable(vehicle_type_names[i])) {
                                m_vehicle_type =
                                    static_cast<nickel::physics::Vehicle::Type>(
                                        i);
                            }
                        }
                    }
                    ImGui::EndCombo();
                    ImGui::DragInt(
                        "wheel num", &m_wheel_num, 1,
                        m_vehicle_type ==
                                nickel::physics::Vehicle::Type::FourWheel
                            ? 4
                            : 1,
                        20);
                    ImGui::EndPopup();

                    if (m_vehicle_type &&
                        m_vehicle_type.value() ==
                            nickel::physics::Vehicle::Type::FourWheel) {
                        m_state = State::ChoosingChassisMesh;
                    } else {
                        m_popup_title_name = "select wheels";
                        m_state = State::ChoosingOtherWheels;
                    }
                }
            }

            // select mesh window
            std::string selected_mesh_window_name;
            static bool is_driving_wheel = false;
            if (!m_popup_title_name.empty()) {
                if (ImGui::BeginPopupModal(m_popup_title_name.data())) {
                    auto names = ctx.GetGLTFManager().GetAllGLTFModelNames();
                    for (auto& name : names) {
                        if (ImGui::Selectable(name.c_str())) {
                            selected_mesh_window_name = name;
                        }

                        if (m_vehicle_type &&
                            m_vehicle_type.value() ==
                                nickel::physics::Vehicle::Type::N_Wheel) {
                            ImGui::Checkbox("is driving wheel",
                                            &is_driving_wheel);
                        }
                    }
                    ImGui::EndPopup();
                }
            }

            if (m_state == State::ChoosingVehicleType) {
                ImGui::OpenPopup(select_vehicle_type_window_name.data());
                m_state = State::Idle;
            } else if (m_state == State::ChoosingChassisMesh) {
                if (!selected_mesh_window_name.empty()) {
                    WheelMeshData mesh_data;
                    mesh_data.m_render_mesh =
                        ctx.GetGLTFManager().Find(selected_mesh_window_name);
                    mesh_data.m_physics_mesh =
                        findPhysicsMesh(selected_mesh_window_name);
                    m_chassis_mesh = mesh_data;

                    m_state = State::ChoosingFrontLeftWheel;
                    m_popup_title_name = "select front left wheel";
                    ImGui::OpenPopup(m_popup_title_name.data());
                }
            } else if (m_state == State::ChoosingFrontLeftWheel) {
                if (!selected_mesh_window_name.empty()) {
                    WheelMeshData mesh_data;
                    mesh_data.m_render_mesh =
                        ctx.GetGLTFManager().Find(selected_mesh_window_name);
                    mesh_data.m_physics_mesh =
                        findPhysicsMesh(selected_mesh_window_name);
                    m_front_left_wheel_idx = m_wheel_meshes.size();
                    m_wheel_meshes.emplace_back(std::move(mesh_data));

                    m_state = State::ChoosingFrontRightWheel;
                    m_popup_title_name = "select front right wheel";
                    m_wheel_num--;
                    ImGui::OpenPopup(m_popup_title_name.data());
                }
            } else if (m_state == State::ChoosingFrontRightWheel) {
                if (!selected_mesh_window_name.empty()) {
                    WheelMeshData mesh_data;
                    mesh_data.m_render_mesh =
                        ctx.GetGLTFManager().Find(selected_mesh_window_name);
                    mesh_data.m_physics_mesh =
                        findPhysicsMesh(selected_mesh_window_name);
                    m_front_right_wheel_idx = m_wheel_meshes.size();
                    m_wheel_meshes.emplace_back(std::move(mesh_data));

                    m_state = State::ChoosingRearLeftWheel;
                    m_popup_title_name = "select rear left wheel";
                    m_wheel_num--;
                    ImGui::OpenPopup(m_popup_title_name.data());
                }
            } else if (m_state == State::ChoosingRearLeftWheel) {
                if (!selected_mesh_window_name.empty()) {
                    WheelMeshData mesh_data;
                    mesh_data.m_render_mesh =
                        ctx.GetGLTFManager().Find(selected_mesh_window_name);
                    mesh_data.m_physics_mesh =
                        findPhysicsMesh(selected_mesh_window_name);
                    m_rear_left_wheel_idx = m_wheel_meshes.size();
                    m_wheel_meshes.emplace_back(std::move(mesh_data));

                    m_state = State::ChoosingRearRightWheel;
                    m_popup_title_name = "select rear right wheel";
                    m_wheel_num--;
                    ImGui::OpenPopup(m_popup_title_name.data());
                }
            } else if (m_state == State::ChoosingRearRightWheel) {
                if (!selected_mesh_window_name.empty()) {
                    WheelMeshData mesh_data;
                    mesh_data.m_render_mesh =
                        ctx.GetGLTFManager().Find(selected_mesh_window_name);
                    mesh_data.m_physics_mesh =
                        findPhysicsMesh(selected_mesh_window_name);
                    m_rear_right_wheel_idx = m_wheel_meshes.size();
                    m_wheel_meshes.emplace_back(std::move(mesh_data));

                    m_popup_title_name = "select other wheels";
                    m_wheel_num--;
                    m_state = State::ChoosingOtherWheels;
                }
            } else if (m_state == State::ChoosingOtherWheels) {
                if (m_wheel_num == 0) {
                    m_state = State::MakeupVehicle;
                } else if (!selected_mesh_window_name.empty()) {
                    WheelMeshData mesh_data;
                    mesh_data.m_render_mesh =
                        ctx.GetGLTFManager().Find(selected_mesh_window_name);
                    mesh_data.m_physics_mesh =
                        findPhysicsMesh(selected_mesh_window_name);
                    if (is_driving_wheel) {
                        m_driving_wheels.push_back(m_wheel_meshes.size());
                    }

                    m_wheel_meshes.emplace_back(std::move(mesh_data));
                    m_wheel_num--;
                }
            } else if (m_state == State::MakeupVehicle) {
                if (m_vehicle_type) {
                    initVehicleModel();
                    initDefaultPhysicsVehicle(m_vehicle_type.value());
                    m_state = State::Tunning;
                }
            } else if (m_state == State::Tunning) {
                displayParamTunning();
            }
        }
        ImGui::End();
    }

    void loadGLTFModel(const nickel::Path& path) {
        auto& gltf_mgr = nickel::Context::GetInst().GetGLTFManager();
        gltf_mgr.Clear();

        nickel::graphics::GLTFLoadConfig config;
        config.m_combine_mesh = false;
        if (!gltf_mgr.Load(path, config)) {
            if (ImGui::BeginPopupModal("error")) {
                ImGui::Text("load gltf failed");
                ImGui::EndPopup();
            }
        }
    }

    std::vector<nickel::graphics::GLTFVertexData> loadPhysicsMesh(
        const nickel::Path& path) {
        nickel::graphics::GLTFVertexDataLoader loader;
        return loader.Load(path);
    }

    uint32_t findPhysicsMesh(const std::string& name) const {
        for (size_t i = 0; i < m_physics_meshes.size(); i++) {
            if (name.find(m_physics_meshes[i].m_name) != std::string::npos) {
                return i;
            }
        }
        LOGE("can't find physics mesh from {}", name);
        return 0;
    }

    void initVehicleModel() {
        nickel::GameObject chassis_go;
        chassis_go.m_name = "chassis";
        chassis_go.m_model = m_chassis_mesh.m_render_mesh;

        for (size_t i = 0; i < m_wheel_meshes.size(); i++) {
            nickel::GameObject other_wheel_go;
            other_wheel_go.m_name = "wheel " + std::to_string(i);
            other_wheel_go.m_model = m_wheel_meshes[i].m_render_mesh;

            chassis_go.m_children.emplace_back(std::move(other_wheel_go));
        }

        auto& root_go =
            nickel::Context::GetInst().GetCurrentLevel().GetRootGO();
        root_go.m_children.emplace_back(std::move(chassis_go));
        m_chassis_go = &root_go.m_children.back();
    }

    void initDefaultPhysicsVehicle(nickel::physics::Vehicle::Type type) {
        auto& ctx = nickel::Context::GetInst();
        auto& physics_ctx = ctx.GetPhysicsContext();

        nickel::physics::VehicleWheelSim4WDescriptor wheel_sim_desc;

        // create chassis
        {
            auto rigid =
                physics_ctx.CreateRigidDynamic(nickel::Vec3{0, 5, 0}, {});
            rigid.SetMass(1500.f);
            rigid.SetMassSpaceInertiaTensor({3625, 3125, 1281});
            m_chassis_go->m_rigid_actor = rigid;

            auto& mesh = m_physics_meshes[m_chassis_mesh.m_physics_mesh];

            auto convex_mesh =
                ctx.GetPhysicsContext().CreateConvexMesh(mesh.m_points);
            auto shape = ctx.GetPhysicsContext().CreateShape(
                nickel::physics::ConvexMeshGeometry{
                    convex_mesh, mesh.m_transform.q, mesh.m_transform.scale},
                ctx.GetPhysicsContext().CreateMaterial(0.8, 0.8, 0.1), true);
            shape.SetCollisionGroup(
                nickel::physics::CollisionGroup::VehicleChassis);
            m_chassis_go->m_rigid_actor.AttachShape(shape);

            physics_ctx.GetMainScene().AddRigidActor(
                m_chassis_go->m_rigid_actor);
        }

        // setup wheel centre offset
        std::vector<nickel::Vec3> wheel_centre_offset;
        wheel_centre_offset.resize(m_wheel_num);
        for (size_t i = 0; i < m_wheel_meshes.size(); i++) {
            wheel_centre_offset[i] =
                m_physics_meshes[m_wheel_meshes[i].m_physics_mesh]
                    .m_transform.p;
        }

        auto sprung_masses = nickel::physics::ComputeVehicleSprungMass(
            wheel_centre_offset, {}, 1500);

        auto convert_to_wheel = [=](nickel::physics::Context& ctx,
                                    const nickel::graphics::GLTFVertexData&
                                        mesh,
                                    uint32_t shape_idx,
                                    uint32_t sprung_mass_idx) {
            nickel::physics::VehicleWheelSimDescriptor::WheelDescriptor desc;
            desc.m_wheel.m_width = 0.4f;
            desc.m_wheel.m_radius = 0.5f;
            desc.m_wheel.m_mass = 20.0f;
            desc.m_wheel.m_moi = 2.5;
            desc.m_wheel_centre_cm_offsets = mesh.m_transform.p;
            desc.m_shape = shape_idx;

            desc.m_suspension.m_max_compression = 0.3;
            desc.m_suspension.m_max_droop = 0.1;
            desc.m_suspension.m_spring_strength = 35000;
            desc.m_suspension.m_spring_damper_rate = 4500;
            desc.m_suspension.m_sprung_mass = sprung_masses[sprung_mass_idx];

            desc.m_suspension_force_app_point_offsets = {0, -0.3, 0};
            desc.m_tire_force_app_cm_offsets = {0, -0.3, 0};

            desc.m_suspension_travel_directions = nickel::Vec3{0, -1, 0};

            std::vector<nickel::Vec3> points;
            points.reserve(mesh.m_points.size());

            nickel::Transform transform = mesh.m_transform;
            transform.p = {0, 0, 0};
            for (auto& p : mesh.m_points) {
                points.emplace_back(transform * p);
            }

            auto convex_mesh = ctx.CreateConvexMesh(points);
            auto shape = ctx.CreateShape(
                nickel::physics::ConvexMeshGeometry{convex_mesh},
                ctx.CreateMaterial(0.2, 0.2, 0.6), true);
            shape.SetCollisionGroup(
                nickel::physics::CollisionGroup::VehicleWheel);
            return std::make_tuple(desc, shape);
        };

        for (size_t i = 0; i < m_wheel_meshes.size(); i++) {
            auto [wheel_desc, wheel_shape] = convert_to_wheel(
                ctx.GetPhysicsContext(),
                m_physics_meshes[m_wheel_meshes[i].m_physics_mesh], i + 1, i);
            wheel_sim_desc.m_wheels.push_back(wheel_desc);
            m_chassis_go->m_rigid_actor.AttachShape(wheel_shape);
        }

        if (m_front_left_wheel_idx) {
            auto& desc = wheel_sim_desc.m_wheels[*m_front_left_wheel_idx];

            desc.m_wheel.m_max_steer = nickel::PI * 0.33333f;
            desc.m_wheel.m_max_hand_brake_torque = 0;
            desc.m_wheel.m_max_brake_torque = 2500.0f;
            wheel_sim_desc.m_front_left_wheel = *m_front_left_wheel_idx;
        }
        if (m_front_right_wheel_idx) {
            auto& desc = wheel_sim_desc.m_wheels[*m_front_right_wheel_idx];

            desc.m_wheel.m_max_steer = nickel::PI * 0.33333f;
            desc.m_wheel.m_max_hand_brake_torque = 0;
            desc.m_wheel.m_max_brake_torque = 2500.0f;
            wheel_sim_desc.m_front_right_wheel = *m_front_right_wheel_idx;
        }
        if (m_rear_left_wheel_idx) {
            auto& desc = wheel_sim_desc.m_wheels[*m_rear_left_wheel_idx];

            desc.m_wheel.m_max_hand_brake_torque = 4000.0f;
            desc.m_wheel.m_max_steer = 0;
            wheel_sim_desc.m_rear_left_wheel = *m_rear_left_wheel_idx;
        }
        if (m_rear_right_wheel_idx) {
            auto& desc = wheel_sim_desc.m_wheels[*m_rear_right_wheel_idx];

            desc.m_wheel.m_max_hand_brake_torque = 4000.0f;
            desc.m_wheel.m_max_steer = 0;
            wheel_sim_desc.m_rear_right_wheel = *m_rear_right_wheel_idx;
        }

        wheel_sim_desc.m_chassis_mass = 1500;

        nickel::physics::VehicleDriveSimDescriptor* drive_desc =
            &m_drive_4w_sim_desc;

        if (type == nickel::physics::Vehicle::Type::N_Wheel) {
            drive_desc = &m_drive_nw_sim_desc;
        }

        drive_desc->m_engine.m_peak_torque = 500;
        drive_desc->m_engine.m_max_omega = 600;

        drive_desc->m_gear.m_reverse_ratio = -4;
        drive_desc->m_gear.m_neutral_ratio = 0;
        drive_desc->m_gear.m_first_ratio = 4;
        drive_desc->m_gear.m_final_ratio = 4;
        drive_desc->m_gear.m_switch_time = 0.5;

        drive_desc->m_clutch.m_strength = 10;

        if (type == nickel::physics::Vehicle::Type::N_Wheel) {
            for (auto idx : m_driving_wheels) {
                m_drive_nw_sim_desc.m_diff.SetDrivenWheel(idx);
            }
        } else {
            m_drive_4w_sim_desc.m_diff.m_type = nickel::physics::
                VehicleDifferential4WDescriptor::Type::LS_Rear_WD;
        }
        m_wheel_sim_desc = wheel_sim_desc;

        m_steer_vs_forward_table.Add(0.0, 0.75);
        m_steer_vs_forward_table.Add(5.0, 0.75);
        m_steer_vs_forward_table.Add(30.0, 0.125);
        m_steer_vs_forward_table.Add(120.0, 0.1);

        m_chassis_go->m_vehicle = createVehicle(type);
    }

    void displayParamTunning() {
        if (ImGui::Button("apply")) {
            if (m_vehicle_type) {
                m_chassis_go->m_vehicle = createVehicle(m_vehicle_type.value());
            }
        }
        if (ImGui::TreeNode("drive")) {
            tunningEngine(m_drive_4w_sim_desc.m_engine);
            tunningDiff(m_drive_4w_sim_desc.m_diff);
            tunningCluth(m_drive_4w_sim_desc.m_clutch);
            tunningGear(m_drive_4w_sim_desc.m_gear);
            tunningAckerMann(m_drive_4w_sim_desc.m_ackermann);
            ImGui::TreePop();
        }

        if (ImGui::TreeNode("wheel")) {
            tunningWheel4WSimData(m_wheel_sim_desc);
            ImGui::TreePop();
        }

        if (ImGui::TreeNode("steer vs forward table")) {
            tunningSteerVsForwardTable(m_steer_vs_forward_table);
        }
    }

    void tunningEngine(nickel::physics::VehicleEngineDescriptor& engine) {
        if (ImGui::TreeNode("engine")) {
            ImGui::DragFloat("damping rate full throttle",
                             &engine.m_damping_rate_full_throttle, 0.1, 0,
                             FLT_MAX);
            ImGui::DragFloat(
                "damping rate zero throttle clutch disengaged",
                &engine.m_damping_rate_zero_throttle_clutch_disengaged, 0.1, 0,
                FLT_MAX);
            ImGui::DragFloat(
                "damping rate zero throttle clutch engaged",
                &engine.m_damping_rate_zero_throttle_clutch_engaged, 0.1, 0,
                FLT_MAX);
            ImGui::DragFloat("max omega", &engine.m_max_omega, 0.1, 0, FLT_MAX);
            ImGui::DragFloat("moi", &engine.m_moi, 0.1, 0, FLT_MAX);
            ImGui::DragFloat("peak torque", &engine.m_peak_torque, 0.1, 0,
                             FLT_MAX);
            ImGui::TreePop();
        }
    }

    void tunningDiff(nickel::physics::VehicleDifferential4WDescriptor& diff) {
        if (ImGui::TreeNode("diff")) {
            ImGui::DragFloat("centre bias", &diff.m_centre_bias, 0.1, 0,
                             FLT_MAX);
            ImGui::DragFloat("front bias", &diff.m_front_bias, 0.1, 0, FLT_MAX);
            ImGui::DragFloat("rear bias", &diff.m_rear_bias, 0.1, 0, FLT_MAX);
            ImGui::DragFloat("front rear split", &diff.m_front_rear_split, 0.1,
                             0, FLT_MAX);
            ImGui::DragFloat("front left right split",
                             &diff.m_front_left_right_split, 0.1, 0, FLT_MAX);
            ImGui::DragFloat("rear left right split",
                             &diff.m_rear_left_right_split, 0.1, 0, FLT_MAX);
            ImGui::TreePop();
        }
    }

    void tunningCluth(nickel::physics::VehicleClutchDescriptor& clutch) {
        if (ImGui::TreeNode("clutch")) {
            ImGui::DragFloat("strength", &clutch.m_strength, 0.1, 0, FLT_MAX);
            ImGui::DragScalar("estimate iterations", ImGuiDataType_U32,
                              &clutch.m_estimate_iterations, 0.1, 0,
                              (const void*)UINT32_MAX);
            ImGui::TreePop();
        }
    }

    void tunningGear(nickel::physics::VehicleGearDescriptor& gear) {
        if (ImGui::TreeNode("gear")) {
            ImGui::DragFloat("reverse ratio", &gear.m_reverse_ratio, 0.1, 0,
                             FLT_MAX);
            ImGui::DragFloat("neutral ratio", &gear.m_neutral_ratio, 0.1, 0,
                             FLT_MAX);
            ImGui::DragFloat("first ratio", &gear.m_first_ratio, 0.1, 0,
                             FLT_MAX);
            ImGui::DragFloat("switch time", &gear.m_switch_time, 0.1, 0,
                             FLT_MAX);

            if (ImGui::Button("add new gear")) {
                gear.m_other_forward_ratios.emplace_back();
            }

            std::optional<size_t> pending_delete;
            for (size_t i = 0; i < gear.m_other_forward_ratios.size(); i++) {
                auto& other_gear = gear.m_other_forward_ratios[i];
                if (ImGui::Button(("delete##" + std::to_string(i)).c_str())) {
                    pending_delete = i;
                }
                ImGui::SameLine();
                ImGui::DragFloat((std::to_string(i + 2) + "th gear").c_str(),
                                 &other_gear, 0.1, 0, FLT_MAX);
            }
            ImGui::TreePop();

            if (pending_delete) {
                gear.m_other_forward_ratios.erase(
                    gear.m_other_forward_ratios.begin() +
                    pending_delete.value());
            }
        }
    }

    void tunningAckerMann(
        nickel::physics::VehicleAckermannGeometryDescriptor& ackermann) {
        if (ImGui::TreeNode("ackermann")) {
            ImGui::DragFloat("accuracy", &ackermann.m_accuracy, 0.1, 0.0001,
                             FLT_MAX);
            ImGui::DragFloat("axle separation", &ackermann.m_axle_separation,
                             0.1, 0.1, FLT_MAX);
            ImGui::DragFloat("front width", &ackermann.m_front_width, 0.1, 0,
                             FLT_MAX);
            ImGui::DragFloat("rear width", &ackermann.m_rear_width, 0.1, 0,
                             FLT_MAX);
            ImGui::TreePop();
        }
    }

    void tunningWheel4WSimData(
        nickel::physics::VehicleWheelSim4WDescriptor& wheel) {
        if (ImGui::TreeNode("wheel sim data")) {
            ImGui::DragFloat("chassis mass", &wheel.m_chassis_mass, 0.1, 0.01,
                             FLT_MAX);

            std::array<const char*, 2> tunning_mode_name = {"single", "all"};
            uint32_t tunning_mode_idx = static_cast<uint32_t>(m_tunning_mode);
            if (ImGui::BeginCombo("tunning mode",
                                  tunning_mode_name[tunning_mode_idx])) {
                for (int i = 0; i < tunning_mode_name.size(); i++) {
                    if (ImGui::Selectable(tunning_mode_name[i],
                                          tunning_mode_idx == i)) {
                        m_tunning_mode = static_cast<WheelTunningMode>(i);
                    }
                }
                ImGui::EndCombo();
            }

            if (m_tunning_mode == WheelTunningMode::SingleWheel) {
                auto wheel_shape_backup = wheel.m_wheels;

                if (ImGui::TreeNode("wheel common config")) {
                    tunningWheelCommonConfig(wheel.m_wheels[0]);
                    ImGui::TreePop();
                }

                if (ImGui::TreeNode("wheel unique config")) {
                    for (int i = 0; i < wheel.m_wheels.size(); i++) {
                        std::string name = "wheel" + std::to_string(i);
                        if (wheel.m_front_left_wheel &&
                            i == wheel.m_front_left_wheel) {
                            name = "front left wheel";
                        } else if (wheel.m_front_right_wheel &&
                                   i == wheel.m_front_right_wheel) {
                            name = "front right wheel";
                        } else if (wheel.m_rear_left_wheel &&
                                   i == wheel.m_rear_left_wheel) {
                            name = "rear left wheel";
                        } else if (wheel.m_rear_right_wheel &&
                                   i == wheel.m_rear_right_wheel) {
                            name = "rear right wheel";
                        }
                        if (ImGui::TreeNode(name.c_str())) {
                            tunningWheelUniqueConfig(wheel.m_wheels[i]);
                            ImGui::TreePop();
                        }
                    }
                    for (int i = 1; i < wheel.m_wheels.size(); i++) {
                        copyCommonConfig2Wheel(wheel.m_wheels[0],
                                               wheel.m_wheels[i]);
                    }
                }
            } else if (m_tunning_mode == WheelTunningMode::AllWheel) {
                for (int i = 0; i < wheel.m_wheels.size(); i++) {
                    std::string name = "wheel" + std::to_string(i);
                    if (wheel.m_front_left_wheel &&
                        i == wheel.m_front_left_wheel) {
                        name = "front left wheel";
                    } else if (wheel.m_front_right_wheel &&
                               i == wheel.m_front_right_wheel) {
                        name = "front right wheel";
                    } else if (wheel.m_rear_left_wheel &&
                               i == wheel.m_rear_left_wheel) {
                        name = "rear left wheel";
                    } else if (wheel.m_rear_right_wheel &&
                               i == wheel.m_rear_right_wheel) {
                        name = "rear right wheel";
                    }
                    auto& wheel_i = wheel.m_wheels[i];
                    tunningWheel(name.c_str(), wheel_i);
                }
            }
            ImGui::TreePop();
        }
    }

    void tunningWheelSimData(
        nickel::physics::VehicleWheelSimDescriptor& wheel) {
        if (ImGui::TreeNode("wheel sim data")) {
            ImGui::DragFloat("chassis mass", &wheel.m_chassis_mass, 0.1, 0.01,
                             FLT_MAX);

            std::array<const char*, 2> tunning_mode_name = {"single", "all"};
            uint32_t tunning_mode_idx = static_cast<uint32_t>(m_tunning_mode);
            if (ImGui::BeginCombo("tunning mode",
                                  tunning_mode_name[tunning_mode_idx])) {
                for (int i = 0; i < 3; i++) {
                    if (ImGui::Selectable(tunning_mode_name[i],
                                          tunning_mode_idx == i)) {
                        m_tunning_mode = static_cast<WheelTunningMode>(i);
                    }
                }
                ImGui::EndCombo();
            }

            if (m_tunning_mode == WheelTunningMode::SingleWheel) {
                auto wheel_shape_backup = wheel.m_wheels;

                if (ImGui::TreeNode("wheel common config")) {
                    tunningWheelCommonConfig(wheel.m_wheels[0]);
                    ImGui::TreePop();
                }

                if (ImGui::TreeNode("wheel unique config")) {
                    for (int i = 0; i < wheel.m_wheels.size(); i++) {
                        if (ImGui::TreeNode(
                                ("wheel " + std::to_string(i)).c_str())) {
                            tunningWheelUniqueConfig(wheel.m_wheels[i]);
                            ImGui::TreePop();
                        }
                    }
                    for (int i = 1; i < wheel.m_wheels.size(); i++) {
                        copyCommonConfig2Wheel(wheel.m_wheels[0],
                                               wheel.m_wheels[i]);
                    }
                }
            } else if (m_tunning_mode == WheelTunningMode::AllWheel) {
                for (int i = 0; i < wheel.m_wheels.size(); i++) {
                    auto& wheel_i = wheel.m_wheels[i];
                    tunningWheel(("wheel" + std::to_string(i)).c_str(),
                                 wheel_i);
                }
            }
            ImGui::TreePop();
        }
    }

    void tunningWheel(
        const char* title,
        nickel::physics::VehicleWheelSimDescriptor::WheelDescriptor& wheel) {
        if (ImGui::TreeNode(title)) {
            tunningWheelCommonConfig(wheel);
            tunningWheelUniqueConfig(wheel);
            ImGui::TreePop();
        }
    }

    void tunningWheelCommonConfig(
        nickel::physics::VehicleWheelSimDescriptor::WheelDescriptor& wheel) {
        // wheel
        ImGui::SeparatorText("wheel");
        {
            ImGui::SeparatorText("wheel");
            ImGui::DragFloat("damping rate", &wheel.m_wheel.m_damping_rate, 0.1,
                             0, FLT_MAX);
            ImGui::DragFloat("brake torque", &wheel.m_wheel.m_max_brake_torque,
                             0.1, 0, FLT_MAX);
            ImGui::DragFloat("hand brake torque",
                             &wheel.m_wheel.m_max_hand_brake_torque, 0.1, 0,
                             FLT_MAX);
            ImGui::DragFloat("moi", &wheel.m_wheel.m_moi, 0.1, 0, FLT_MAX);
            ImGui::DragFloat("radius", &wheel.m_wheel.m_radius, 0.1, 0.001,
                             FLT_MAX);
            ImGui::DragFloat("width", &wheel.m_wheel.m_width, 0.1, 0.001,
                             FLT_MAX);

            float max_steer =
                nickel::Degrees{wheel.m_wheel.m_max_steer}.Value();
            if (ImGui::DragFloat("max steer", &max_steer, 0.1, 0.001,
                                 FLT_MAX)) {
                wheel.m_wheel.m_max_steer = nickel::Degrees{max_steer};
            }

            float toe_angle =
                nickel::Degrees{wheel.m_wheel.m_toe_angle}.Value();
            if (ImGui::DragFloat("toe angle", &toe_angle, 0.1, 0.001,
                                 FLT_MAX)) {
                wheel.m_wheel.m_toe_angle = nickel::Degrees{toe_angle};
            }
        }

        // tire
        ImGui::SeparatorText("tire");
        {
            ImGui::SeparatorText("tire");
            ImGui::DragFloat("lat stiff x", &wheel.m_tire.m_lat_stiff_x, 0.1, 0,
                             FLT_MAX);

            float lat_stiff_y =
                nickel::Degrees{wheel.m_tire.m_lat_stiff_y}.Value();
            if (ImGui::DragFloat("lat stiff y", &lat_stiff_y, 0.1, 0.001,
                                 FLT_MAX)) {
                wheel.m_tire.m_lat_stiff_y = nickel::Degrees{lat_stiff_y};
            }

            ImGui::DragFloat(
                "longitudinal stiffness per unit gravity",
                &wheel.m_tire.m_longitudinal_stiffness_per_unit_gravity, 0.1, 0,
                FLT_MAX);

            float camber =
                nickel::Degrees{
                    wheel.m_tire.m_camber_stiffness_per_unit_gravity}
                    .Value();
            if (ImGui::DragFloat("camber stiffness per unit gravity", &camber,
                                 0.1, 0.001, FLT_MAX)) {
                wheel.m_tire.m_camber_stiffness_per_unit_gravity =
                    nickel::Degrees{camber};
            }

            auto& config = wheel.m_tire.m_friction_vs_slip_config;
            std::array<ImPlotPoint, 3> points;
            points[0].x = 0;
            points[0].y = config.m_friction_at_zero_slip;
            points[1].x = config.m_slip_at_maximum_firction;
            points[1].y = config.m_max_firction;
            points[2].x = config.m_max_slip;
            points[2].y = config.m_friction_at_max_slip;
            if (ImPlot::BeginPlot("friction-slip config")) {
                ImPlot::SetupAxis(ImAxis_X1, "slip", ImPlotAxisFlags_Lock);
                ImPlot::SetupAxisLimits(ImAxis_X1, 0, 1);
                for (int i = 0; i < points.size(); i++) {
                    if (ImPlot::DragPoint(i, &points[i].x, &points[i].y,
                                          ImVec4(0, 0.9f, 0, 1), 10, 0)) {
                        points[i].x = nickel::Clamp(points[i].x, 0.0, 1.0);
                        points[i].y = std::max(points[i].y, 0.0);
                    }
                    if (ImGui::IsItemHovered()) {
                        ImGui::SetTooltip("%lf, %lf", points[i].x, points[i].y);
                    }
                    points[2].y = nickel::Clamp(points[2].y, 0.0, points[1].y);
                }
                ImPlot::PlotLine("##xx", &points[0].x, &points[0].y,
                                 points.size(), 0, 0, sizeof(ImPlotPoint));
                ImPlot::EndPlot();
            }

            config.m_friction_at_zero_slip = points[0].y;
            config.m_slip_at_maximum_firction = points[1].x;
            config.m_max_firction = points[1].y;
            config.m_max_slip = points[2].x;
            config.m_friction_at_max_slip = points[2].y;
        }

        // suspension
        ImGui::SeparatorText("suspension");
        {
            ImGui::DragFloat("max compression",
                             &wheel.m_suspension.m_max_compression, 0.1, 0,
                             FLT_MAX);
            ImGui::DragFloat("max droop", &wheel.m_suspension.m_max_droop, 0.1,
                             0, FLT_MAX);
            ImGui::DragFloat("spring damping rate",
                             &wheel.m_suspension.m_spring_damper_rate, 0.1, 0,
                             FLT_MAX);
            ImGui::DragFloat("spring strength",
                             &wheel.m_suspension.m_spring_strength, 0.1, 0,
                             FLT_MAX);
            ImGui::DragFloat("sprung mass", &wheel.m_suspension.m_sprung_mass,
                             0.1, 0, FLT_MAX);
        }
        ImGui::DragFloat3("suspension force apply point offset",
                          wheel.m_suspension_force_app_point_offsets.Ptr());
        ImGui::DragFloat3("suspension travel direction",
                          wheel.m_suspension_travel_directions.Ptr());
        ImGui::DragFloat3("tire force apply centre mass offset",
                          wheel.m_tire_force_app_cm_offsets.Ptr());

        ImGui::SeparatorText("misc");
        ImGui::DragFloat3("wheel centre mass offset",
                          wheel.m_wheel_centre_cm_offsets.Ptr());
    }

    void tunningWheelUniqueConfig(
        nickel::physics::VehicleWheelSimDescriptor::WheelDescriptor& wheel) {
        ImGui::SeparatorText("camber");
        {
            float camber =
                nickel::Degrees{wheel.m_suspension.m_camber_at_max_compression}
                    .Value();
            if (ImGui::DragFloat("camber at max compression", &camber, 0.1, 0,
                                 FLT_MAX)) {
                wheel.m_suspension.m_camber_at_max_compression =
                    nickel::Radians{nickel::Degrees{camber}};
            }
        }

        {
            float camber =
                nickel::Degrees{wheel.m_suspension.m_camber_at_max_droop}
                    .Value();
            if (ImGui::DragFloat("camber at max droop", &camber, 0.1, 0,
                                 FLT_MAX)) {
                wheel.m_suspension.m_camber_at_max_droop =
                    nickel::Radians{nickel::Degrees{camber}};
            }
        }

        {
            float camber =
                nickel::Degrees{wheel.m_suspension.m_camber_at_rest}.Value();
            if (ImGui::DragFloat("camber at max rest", &camber, 0.1, 0,
                                 FLT_MAX)) {
                wheel.m_suspension.m_camber_at_rest =
                    nickel::Radians{nickel::Degrees{camber}};
            }
        }
    }

    void tunningSteerVsForwardTable(
        nickel::physics::VehicleSteerVsForwardTable& table) {
        std::optional<size_t> pending_delete;
        for (int i = 0; i < table.Size(); i++) {
            auto& pair = table.GetPair(i);
            if (ImGui::Button("delete")) {
                pending_delete = i;
            }
            ImGui::SameLine();
            ImGui::DragFloat2(("value " + std::to_string(i)).c_str(),
                              (float*)&pair, 0.01, 0, FLT_MAX);
        }
        if (pending_delete) {
            table.Remove(pending_delete.value());
        }
    }

    void copyCommonConfig2Wheel(
        nickel::physics::VehicleWheelSimDescriptor::WheelDescriptor& src,
        nickel::physics::VehicleWheelSimDescriptor::WheelDescriptor& dst) {
        auto backup = dst;
        dst = src;
        dst.m_suspension.m_camber_at_max_compression =
            backup.m_suspension.m_camber_at_max_compression;
        dst.m_suspension.m_camber_at_max_droop =
            backup.m_suspension.m_camber_at_max_droop;
        dst.m_suspension.m_camber_at_rest =
            backup.m_suspension.m_camber_at_rest;
    }

    nickel::physics::Vehicle createVehicle(
        nickel::physics::Vehicle::Type type) {
        auto& physics_ctx = nickel::Context::GetInst().GetPhysicsContext();

        nickel::physics::Vehicle result;
        if (type == nickel::physics::Vehicle::Type::FourWheel) {
            auto vehicle = physics_ctx.GetVehicleManager().CreateVehicle4WDrive(
                m_wheel_sim_desc, m_drive_4w_sim_desc,
                static_cast<nickel::physics::RigidDynamic&>(
                    m_chassis_go->m_rigid_actor));
            vehicle.SetSteerVsForwardSpeedLookupTable(m_steer_vs_forward_table);
            result = vehicle;
        } else if (type == nickel::physics::Vehicle::Type::N_Wheel) {
            auto vehicle = physics_ctx.GetVehicleManager().CreateVehicleNWDrive(
                m_wheel_sim_desc, m_drive_nw_sim_desc,
                static_cast<nickel::physics::RigidDynamic&>(
                    m_chassis_go->m_rigid_actor));
            vehicle.SetSteerVsForwardSpeedLookupTable(m_steer_vs_forward_table);
            result = vehicle;
        } else if (type == nickel::physics::Vehicle::Type::Tank) {
            auto vehicle =
                physics_ctx.GetVehicleManager().CreateVehicleTankDrive(
                    nickel::physics::VehicleTankDriveMode::Standard,
                    m_wheel_sim_desc, m_drive_4w_sim_desc,
                    static_cast<nickel::physics::RigidDynamic&>(
                        m_chassis_go->m_rigid_actor));
            result = vehicle;
        } else if (type == nickel::physics::Vehicle::Type::NoDrive) {
            auto vehicle = physics_ctx.GetVehicleManager().CreateVehicleNoDrive(
                m_wheel_sim_desc, static_cast<nickel::physics::RigidDynamic&>(
                                      m_chassis_go->m_rigid_actor));
            result = vehicle;
        }
        return result;
    }
};

NICKEL_RUN_APP(Application)
