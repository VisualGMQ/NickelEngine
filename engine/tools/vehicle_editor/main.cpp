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
        ChoosingChassisMesh,
        ChoosingFrontLeftWheel,
        ChoosingFrontRightWheel,
        ChoosingRearLeftWheel,
        ChoosingRearRightWheel,
        MakeupVehicle,
        Tunning,
    } m_state = State::Idle;

    enum class WheelTunningMode {
        SingleWheel = 0,
        DoubleWheel,
        FourWheel,
    } m_tunning_mode = WheelTunningMode::FourWheel;

    std::string_view m_popup_title_name;

    nickel::physics::VehicleWheelSim4WDescriptor m_wheel_sim_desc;
    nickel::physics::VehicleDriveSim4WDescriptor m_drive_sim_desc;
    std::vector<nickel::graphics::GLTFModel> m_models;
    std::vector<nickel::graphics::GLTFVertexData> m_physics_meshes;
    nickel::graphics::GLTFModel m_chassis_model, m_front_left_wheel_model,
        m_front_right_wheel_model, m_rear_left_wheel_model,
        m_rear_right_wheel_model;
    uint32_t m_chassis_physics_mesh, m_front_left_physics_mesh,
        m_front_right_physics_mesh, m_rear_left_physics_mesh,
        m_rear_right_physics_mesh;
    nickel::GameObject* m_chassis_go{};
    nickel::GameObject* m_front_left_wheel_go{};
    nickel::GameObject* m_front_right_wheel_go{};
    nickel::GameObject* m_rear_left_wheel_go{};
    nickel::GameObject* m_rear_right_wheel_go{};

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
                m_state = State::ChoosingChassisMesh;
                m_popup_title_name = "select chassis";
                ImGui::OpenPopup(m_popup_title_name.data());
            }

            std::string selected_name;
            if (!m_popup_title_name.empty()) {
                if (ImGui::BeginPopupModal(m_popup_title_name.data())) {
                    auto names = ctx.GetGLTFManager().GetAllGLTFModelNames();
                    for (auto& name : names) {
                        if (ImGui::Selectable(name.c_str())) {
                            selected_name = name;
                        }
                    }
                    ImGui::EndPopup();
                }
            }

            if (m_state == State::ChoosingChassisMesh) {
                if (!selected_name.empty()) {
                    m_chassis_model = ctx.GetGLTFManager().Find(selected_name);
                    m_chassis_physics_mesh = findPhysicsMesh(selected_name);

                    m_state = State::ChoosingFrontLeftWheel;
                    m_popup_title_name = "select front left wheel";
                    ImGui::OpenPopup(m_popup_title_name.data());
                }
            } else if (m_state == State::ChoosingFrontLeftWheel) {
                if (!selected_name.empty()) {
                    m_front_left_wheel_model =
                        ctx.GetGLTFManager().Find(selected_name);
                    m_front_left_physics_mesh = findPhysicsMesh(selected_name);
                    m_state = State::ChoosingFrontRightWheel;
                    m_popup_title_name = "select front right wheel";
                    ImGui::OpenPopup(m_popup_title_name.data());
                }
            } else if (m_state == State::ChoosingFrontRightWheel) {
                if (!selected_name.empty()) {
                    m_front_right_wheel_model =
                        ctx.GetGLTFManager().Find(selected_name);
                    m_front_right_physics_mesh = findPhysicsMesh(selected_name);
                    m_state = State::ChoosingRearLeftWheel;
                    m_popup_title_name = "select rear left wheel";
                    ImGui::OpenPopup(m_popup_title_name.data());
                }
            } else if (m_state == State::ChoosingRearLeftWheel) {
                if (!selected_name.empty()) {
                    m_rear_left_wheel_model =
                        ctx.GetGLTFManager().Find(selected_name);
                    m_rear_left_physics_mesh = findPhysicsMesh(selected_name);
                    m_state = State::ChoosingRearRightWheel;
                    m_popup_title_name = "select rear right wheel";
                    ImGui::OpenPopup(m_popup_title_name.data());
                }
            } else if (m_state == State::ChoosingRearRightWheel) {
                if (!selected_name.empty()) {
                    m_rear_right_wheel_model =
                        ctx.GetGLTFManager().Find(selected_name);
                    m_rear_right_physics_mesh = findPhysicsMesh(selected_name);
                    m_state = State::MakeupVehicle;
                }
            } else if (m_state == State::MakeupVehicle) {
                initVehicleModel();
                initDefaultPhysicsVehicle();
                m_state = State::Tunning;
            } else if (m_state == State::Tunning) {
                displayParamTunning();
            }
        }
        ImGui::End();
    }

    void loadGLTFModel(const nickel::Path& path) {
        auto& gltf_mgr = nickel::Context::GetInst().GetGLTFManager();
        m_models.clear();
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
        chassis_go.m_model = m_chassis_model;

        nickel::GameObject front_left_wheel_go;
        front_left_wheel_go.m_name = "front left wheel";
        front_left_wheel_go.m_model = m_front_left_wheel_model;

        nickel::GameObject front_right_wheel_go;
        front_right_wheel_go.m_name = "front right wheel";
        front_right_wheel_go.m_model = m_front_right_wheel_model;

        nickel::GameObject rear_left_wheel_go;
        rear_left_wheel_go.m_name = "rear left wheel";
        rear_left_wheel_go.m_model = m_rear_left_wheel_model;

        nickel::GameObject rear_right_wheel_go;
        rear_right_wheel_go.m_name = "rear right wheel";
        rear_right_wheel_go.m_model = m_rear_right_wheel_model;

        chassis_go.m_children.emplace_back(std::move(front_left_wheel_go));
        chassis_go.m_children.emplace_back(std::move(front_right_wheel_go));
        chassis_go.m_children.emplace_back(std::move(rear_left_wheel_go));
        chassis_go.m_children.emplace_back(std::move(rear_right_wheel_go));

        auto& root_go =
            nickel::Context::GetInst().GetCurrentLevel().GetRootGO();
        root_go.m_children.emplace_back(std::move(chassis_go));

        m_chassis_go = &root_go.m_children.back();
        m_front_left_wheel_go = &m_chassis_go->m_children[0];
        m_front_right_wheel_go = &m_chassis_go->m_children[1];
        m_rear_left_wheel_go = &m_chassis_go->m_children[2];
        m_rear_right_wheel_go = &m_chassis_go->m_children[3];
    }

    void initDefaultPhysicsVehicle() {
        auto& ctx = nickel::Context::GetInst();
        auto& physics_ctx = ctx.GetPhysicsContext();

        nickel::physics::VehicleWheelSim4WDescriptor wheel_sim_desc;
        nickel::physics::VehicleDriveSim4WDescriptor drive_sim_desc;

        constexpr uint32_t CollisionGroupVehicleChassis = 0x01;
        constexpr uint32_t CollisionGroupVehicleWheels = 0x02;

        // create chassis
        {
            auto rigid =
                physics_ctx.CreateRigidDynamic(nickel::Vec3{0, 5, 0}, {});
            rigid.SetMass(1500.f);
            rigid.SetMassSpaceInertiaTensor({3625, 3125, 1281});
            m_chassis_go->m_rigid_actor = rigid;

            auto& mesh = m_physics_meshes[m_chassis_physics_mesh];

            auto convex_mesh =
                ctx.GetPhysicsContext().CreateConvexMesh(mesh.m_points);
            auto shape = ctx.GetPhysicsContext().CreateShape(
                nickel::physics::ConvexMeshGeometry{
                    convex_mesh, mesh.m_transform.q, mesh.m_transform.scale},
                ctx.GetPhysicsContext().CreateMaterial(0.8, 0.8, 0.1), true);
            shape.SetCollisionGroup(nickel::physics::CollisionGroup::VehicleChassis);
            m_chassis_go->m_rigid_actor.AttachShape(shape);

            physics_ctx.GetMainScene().AddRigidActor(
                m_chassis_go->m_rigid_actor);
        }

        std::vector<nickel::Vec3> wheel_centre_offset;
        wheel_centre_offset.resize(4);
        wheel_centre_offset[0] =
            m_physics_meshes[m_front_left_physics_mesh].m_transform.p;
        wheel_centre_offset[1] =
            m_physics_meshes[m_front_right_physics_mesh].m_transform.p;
        wheel_centre_offset[2] =
            m_physics_meshes[m_rear_left_physics_mesh].m_transform.p;
        wheel_centre_offset[3] =
            m_physics_meshes[m_rear_right_physics_mesh].m_transform.p;

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
            shape.SetCollisionGroup(nickel::physics::CollisionGroup::VehicleWheel);
            return std::make_tuple(desc, shape);
        };

        auto [driving_left_desc, driving_left_shape] =
            convert_to_wheel(ctx.GetPhysicsContext(),
                             m_physics_meshes[m_rear_left_physics_mesh], 1, 2);
        auto [driving_right_desc, driving_right_shape] =
            convert_to_wheel(ctx.GetPhysicsContext(),
                             m_physics_meshes[m_rear_right_physics_mesh], 2, 3);
        auto [steer_right_desc, steer_right_shape] = convert_to_wheel(
            ctx.GetPhysicsContext(),
            m_physics_meshes[m_front_right_physics_mesh], 3, 1);
        auto [steer_left_desc, steer_left_shape] =
            convert_to_wheel(ctx.GetPhysicsContext(),
                             m_physics_meshes[m_front_left_physics_mesh], 4, 0);

        driving_left_desc.m_wheel.m_max_hand_brake_torque = 4000.0f;
        driving_left_desc.m_wheel.m_max_steer = 0;
        wheel_sim_desc.m_rear_left_wheel = wheel_sim_desc.m_wheels.size();
        wheel_sim_desc.m_wheels.push_back(driving_left_desc);

        driving_right_desc.m_wheel.m_max_hand_brake_torque = 4000.0f;
        driving_right_desc.m_wheel.m_max_steer = 0;
        wheel_sim_desc.m_rear_right_wheel = wheel_sim_desc.m_wheels.size();
        wheel_sim_desc.m_wheels.push_back(driving_right_desc);

        steer_left_desc.m_wheel.m_max_steer = nickel::PI * 0.33333f;
        steer_left_desc.m_wheel.m_max_hand_brake_torque = 0;
        steer_left_desc.m_wheel.m_max_brake_torque = 2500.0f;
        wheel_sim_desc.m_front_left_wheel = wheel_sim_desc.m_wheels.size();
        wheel_sim_desc.m_wheels.push_back(steer_left_desc);

        steer_right_desc.m_wheel.m_max_steer = nickel::PI * 0.33333f;
        steer_right_desc.m_wheel.m_max_hand_brake_torque = 0;
        steer_right_desc.m_wheel.m_max_brake_torque = 2500.0f;
        wheel_sim_desc.m_front_right_wheel = wheel_sim_desc.m_wheels.size();
        wheel_sim_desc.m_wheels.push_back(steer_right_desc);

        m_chassis_go->m_rigid_actor.AttachShape(driving_left_shape);
        m_chassis_go->m_rigid_actor.AttachShape(driving_right_shape);
        m_chassis_go->m_rigid_actor.AttachShape(steer_left_shape);
        m_chassis_go->m_rigid_actor.AttachShape(steer_right_shape);

        wheel_sim_desc.m_chassis_mass = 1500;

        drive_sim_desc.m_engine.m_peak_torque = 500;
        drive_sim_desc.m_engine.m_max_omega = 600;

        drive_sim_desc.m_gear.m_reverse_ratio = -4;
        drive_sim_desc.m_gear.m_neutral_ratio = 0;
        drive_sim_desc.m_gear.m_first_ratio = 4;
        drive_sim_desc.m_gear.m_final_ratio = 4;
        drive_sim_desc.m_gear.m_switch_time = 0.5;

        drive_sim_desc.m_clutch.m_strength = 10;

        drive_sim_desc.m_diff.m_type =
            nickel::physics::VehicleDifferential4WDescriptor::Type::LS_Rear_WD;

        m_wheel_sim_desc = wheel_sim_desc;
        m_drive_sim_desc = drive_sim_desc;

        auto vehicle = physics_ctx.GetVehicleManager().CreateVehicle4WDrive(
            wheel_sim_desc, drive_sim_desc,
            static_cast<nickel::physics::RigidDynamic&>(
                m_chassis_go->m_rigid_actor));

        nickel::physics::VehicleSteerVsForwardTable table;
        table.Add(0.0, 0.75);
        table.Add(5.0, 0.75);
        table.Add(30.0, 0.125);
        table.Add(120.0, 0.1);
        vehicle.SetSteerVsForwardSpeedLookupTable(table);
        m_chassis_go->m_vehicle = vehicle;
    }

    void displayParamTunning() {
        if (ImGui::Button("apply")) {
            auto& vehicle_mgr = nickel::Context::GetInst()
                                    .GetPhysicsContext()
                                    .GetVehicleManager();
            nickel::physics::RigidDynamic& rigid =
                static_cast<nickel::physics::RigidDynamic&>(
                    m_chassis_go->m_rigid_actor);
            rigid.SetGlobalTransform({0, 5, 0}, {});
            m_chassis_go->m_vehicle = vehicle_mgr.CreateVehicle4WDrive(
                m_wheel_sim_desc, m_drive_sim_desc, rigid);
        }
        if (ImGui::TreeNode("drive")) {
            tunningEngine(m_drive_sim_desc.m_engine);
            tunningDiff(m_drive_sim_desc.m_diff);
            tunningCluth(m_drive_sim_desc.m_clutch);
            tunningGear(m_drive_sim_desc.m_gear);
            tunningAckerMann(m_drive_sim_desc.m_ackermann);
            ImGui::TreePop();
        }

        if (ImGui::TreeNode("wheel")) {
            tunningWheelSimData(m_wheel_sim_desc);
            ImGui::TreePop();
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

    void tunningWheelSimData(
        nickel::physics::VehicleWheelSim4WDescriptor& wheel) {
        if (ImGui::TreeNode("wheel sim data")) {
            ImGui::DragFloat("chassis mass", &wheel.m_chassis_mass, 0.1, 0.01,
                             FLT_MAX);

            std::array<const char*, 3> tunning_mode_name = {"single", "double",
                                                            "four"};
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

            WheelTunningMode last_tunning_mode =
                static_cast<WheelTunningMode>(tunning_mode_idx);
            if (last_tunning_mode != m_tunning_mode) {
                // TODO:
            }

            auto& front_left_wheel =
                wheel.m_wheels[wheel.m_front_left_wheel.value()];
            auto& front_right_wheel =
                wheel.m_wheels[wheel.m_front_right_wheel.value()];
            auto& rear_left_wheel =
                wheel.m_wheels[wheel.m_rear_left_wheel.value()];
            auto& rear_right_wheel =
                wheel.m_wheels[wheel.m_rear_right_wheel.value()];
            if (m_tunning_mode == WheelTunningMode::SingleWheel) {
                auto wheel_shape_backup = wheel.m_wheels;

                tunningWheel("wheel", front_left_wheel);

                front_right_wheel = front_left_wheel;
                rear_left_wheel = front_left_wheel;
                rear_right_wheel = front_left_wheel;

                front_right_wheel.m_shape =
                    wheel_shape_backup[wheel.m_front_right_wheel.value()]
                        .m_shape;
                rear_left_wheel.m_shape =
                    wheel_shape_backup[wheel.m_rear_left_wheel.value()].m_shape;
                rear_right_wheel.m_shape =
                    wheel_shape_backup[wheel.m_rear_right_wheel.value()]
                        .m_shape;

                front_right_wheel.m_suspension.m_camber_at_max_compression *=
                    -1;
                front_right_wheel.m_suspension.m_camber_at_max_droop *= -1;
                front_right_wheel.m_suspension.m_camber_at_rest *= -1;
                rear_right_wheel.m_suspension.m_camber_at_max_compression *= -1;
                rear_right_wheel.m_suspension.m_camber_at_max_droop *= -1;
                rear_right_wheel.m_suspension.m_camber_at_rest *= -1;
            } else if (m_tunning_mode == WheelTunningMode::DoubleWheel) {
                // TODO:
            } else if (m_tunning_mode == WheelTunningMode::FourWheel) {
                tunningWheel("front left", front_left_wheel);
                tunningWheel("front right", front_right_wheel);
                tunningWheel("rear left", rear_left_wheel);
                tunningWheel("rear right", rear_right_wheel);
            }
            ImGui::TreePop();
        }
    }

    void tunningWheel(
        const char* title,
        nickel::physics::VehicleWheelSim4WDescriptor::WheelDescriptor& wheel) {
        if (ImGui::TreeNode(title)) {
            // wheel
            {
                ImGui::SeparatorText("wheel");
                ImGui::DragFloat("damping rate", &wheel.m_wheel.m_damping_rate,
                                 0.1, 0, FLT_MAX);
                ImGui::DragFloat("brake torque",
                                 &wheel.m_wheel.m_max_brake_torque, 0.1, 0,
                                 FLT_MAX);
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
            {
                ImGui::SeparatorText("tire");
                ImGui::DragFloat("lat stiff x", &wheel.m_tire.m_lat_stiff_x,
                                 0.1, 0, FLT_MAX);

                float lat_stiff_y =
                    nickel::Degrees{wheel.m_tire.m_lat_stiff_y}.Value();
                if (ImGui::DragFloat("lat stiff y", &lat_stiff_y, 0.1, 0.001,
                                     FLT_MAX)) {
                    wheel.m_tire.m_lat_stiff_y = nickel::Degrees{lat_stiff_y};
                }

                ImGui::DragFloat(
                    "longitudinal stiffness per unit gravity",
                    &wheel.m_tire.m_longitudinal_stiffness_per_unit_gravity,
                    0.1, 0, FLT_MAX);

                float camber =
                    nickel::Degrees{
                        wheel.m_tire.m_camber_stiffness_per_unit_gravity}
                        .Value();
                if (ImGui::DragFloat("camber stiffness per unit gravity",
                                     &camber, 0.1, 0.001, FLT_MAX)) {
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
                            ImGui::SetTooltip("%lf, %lf", points[i].x,
                                              points[i].y);
                        }
                        points[2].y =
                            nickel::Clamp(points[2].y, 0.0, points[1].y);
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
            {
                ImGui::DragFloat("max compression",
                                 &wheel.m_suspension.m_max_compression, 0.1, 0,
                                 FLT_MAX);
                ImGui::DragFloat("max droop", &wheel.m_suspension.m_max_droop,
                                 0.1, 0, FLT_MAX);
                ImGui::DragFloat("spring damping rate",
                                 &wheel.m_suspension.m_spring_damper_rate, 0.1,
                                 0, FLT_MAX);
                ImGui::DragFloat("spring strength",
                                 &wheel.m_suspension.m_spring_strength, 0.1, 0,
                                 FLT_MAX);
                ImGui::DragFloat("sprung mass",
                                 &wheel.m_suspension.m_sprung_mass, 0.1, 0,
                                 FLT_MAX);

                {
                    float camber =
                        nickel::Degrees{
                            wheel.m_suspension.m_camber_at_max_compression}
                            .Value();
                    if (ImGui::DragFloat("camber at max compression", &camber,
                                         0.1, 0, FLT_MAX)) {
                        wheel.m_suspension.m_camber_at_max_compression =
                            nickel::Radians{nickel::Degrees{camber}};
                    }
                }

                {
                    float camber =
                        nickel::Degrees{
                            wheel.m_suspension.m_camber_at_max_droop}
                            .Value();
                    if (ImGui::DragFloat("camber at max droop", &camber, 0.1, 0,
                                         FLT_MAX)) {
                        wheel.m_suspension.m_camber_at_max_droop =
                            nickel::Radians{nickel::Degrees{camber}};
                    }
                }

                {
                    float camber =
                        nickel::Degrees{wheel.m_suspension.m_camber_at_rest}
                            .Value();
                    if (ImGui::DragFloat("camber at max rest", &camber, 0.1, 0,
                                         FLT_MAX)) {
                        wheel.m_suspension.m_camber_at_rest =
                            nickel::Radians{nickel::Degrees{camber}};
                    }
                }
            }
            ImGui::DragFloat3("suspension force apply point offset",
                              wheel.m_suspension_force_app_point_offsets.Ptr());
            ImGui::DragFloat3("suspension travel direction",
                              wheel.m_suspension_travel_directions.Ptr());
            ImGui::DragFloat3("tire force apply centre mass offset",
                              wheel.m_tire_force_app_cm_offsets.Ptr());
            ImGui::DragFloat3("wheel centre mass offset",
                              wheel.m_wheel_centre_cm_offsets.Ptr());
            ImGui::TreePop();
        }
    }
};

NICKEL_RUN_APP(Application)
