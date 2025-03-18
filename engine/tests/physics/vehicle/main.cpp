#include "nickel/common/macro.hpp"
#include "nickel/graphics/internal/gltf_model_impl.hpp"
#include "nickel/main_entry/runtime.hpp"
#include "nickel/nickel.hpp"
#include "vehicle_debug_panel.hpp"

enum {
    COLLISION_FLAG_GROUND = 1 << 0,
    COLLISION_FLAG_WHEEL = 1 << 1,
    COLLISION_FLAG_CHASSIS = 1 << 2,
    COLLISION_FLAG_OBSTACLE = 1 << 3,
    COLLISION_FLAG_DRIVABLE_OBSTACLE = 1 << 4,

    COLLISION_FLAG_GROUND_AGAINST = COLLISION_FLAG_CHASSIS |
                                    COLLISION_FLAG_OBSTACLE |
                                    COLLISION_FLAG_DRIVABLE_OBSTACLE,
    COLLISION_FLAG_WHEEL_AGAINST =
        COLLISION_FLAG_WHEEL | COLLISION_FLAG_CHASSIS | COLLISION_FLAG_OBSTACLE,
    COLLISION_FLAG_CHASSIS_AGAINST =
        COLLISION_FLAG_GROUND | COLLISION_FLAG_WHEEL | COLLISION_FLAG_CHASSIS |
        COLLISION_FLAG_OBSTACLE | COLLISION_FLAG_DRIVABLE_OBSTACLE,
    COLLISION_FLAG_OBSTACLE_AGAINST =
        COLLISION_FLAG_GROUND | COLLISION_FLAG_WHEEL | COLLISION_FLAG_CHASSIS |
        COLLISION_FLAG_OBSTACLE | COLLISION_FLAG_DRIVABLE_OBSTACLE,
    COLLISION_FLAG_DRIVABLE_OBSTACLE_AGAINST =
        COLLISION_FLAG_GROUND | COLLISION_FLAG_CHASSIS |
        COLLISION_FLAG_OBSTACLE | COLLISION_FLAG_DRIVABLE_OBSTACLE
};

class Application : public nickel::Application {
public:
    void OnInit() override {
        auto& ctx = nickel::Context::GetInst();
        nickel::FlyCamera& camera = (nickel::FlyCamera&)ctx.GetCamera();

        camera.MoveTo(nickel::Vec3{0, 3, 3});
        camera.SetPitch(nickel::Degrees{45});

        auto& mouse = ctx.GetDeviceManager().GetMouse();
        mouse.RelativeMode(true);
        mouse.Show(false);

        auto& mgr = ctx.GetGLTFManager();
        mgr.Load("tests/physics/vehicle/assets/car/car.gltf");
        auto& root_go = ctx.GetCurrentLevel().GetRootGO();

        auto& physics_ctx = ctx.GetPhysicsContext();
        //  create plane
        {
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
            root_go.m_children.push_back(go);
        }
        // create Vehicle
        {
            nickel::GameObject go;
            go.m_name = "car";
            go.m_model = mgr.Find("tests/physics/vehicle/assets/car/car");
            auto rigid =
                physics_ctx.CreateRigidDynamic(nickel::Vec3{0, 5, -5}, {});
            rigid.SetMass(12.50f);
            go.m_rigid_actor = rigid;

            nickel::graphics::GLTFVertexDataLoader loader;
            auto meshes =
                loader.Load("tests/physics/vehicle/assets/car/car.gltf");

            uint32_t wheel_steer_left = 0;
            uint32_t wheel_steer_right = 0;
            uint32_t wheel_driving_left = 0;
            uint32_t wheel_driving_right = 0;
            for (uint32_t i = 0; i < meshes.size(); ++i) {
                if (meshes[i].m_name == "wheel.steer.front.right") {
                    wheel_steer_right = i;
                } else if (meshes[i].m_name == "wheel.steer.front.left") {
                    wheel_steer_left = i;
                } else if (meshes[i].m_name == "wheel.driving.rear.left") {
                    wheel_driving_left = i;
                } else if (meshes[i].m_name == "wheel.driving.rear.right") {
                    wheel_driving_right = i;
                }
            }

            constexpr uint32_t CollisionGroupVehicleChassis = 0x01;
            constexpr uint32_t CollisionGroupVehicleWheels = 0x02;

            auto convert_to_wheel = [](nickel::physics::Context& ctx,
                                       const nickel::graphics::GLTFVertexData&
                                           mesh) {
                nickel::physics::VehicleWheelSimDescriptor::WheelDescriptor
                    desc;
                desc.m_wheel.m_width = 0.2f;
                desc.m_wheel.m_radius = 0.3f;
                desc.m_wheel_centre_cm_offsets = mesh.m_transform.p;
                desc.m_suspension.m_spring_strength = 35000;
                desc.m_suspension.m_spring_damper_rate = 4500;
                desc.m_suspension.m_max_compression = 0.3;
                desc.m_suspension.m_max_droop = 0.1;
                desc.m_suspension.m_sprung_mass = 3.125;
                desc.m_suspension.m_camber_at_rest = 0;

                desc.m_suspension.m_camber_at_max_compression = 0;
                desc.m_suspension.m_camber_at_max_droop = 0;

                desc.m_tire.m_lat_stiff_x = 2;
                desc.m_tire.m_lat_stiff_y = 17.904932;
                desc.m_tire.m_longitudinal_stiffness_per_unit_gravity = 1000;
                desc.m_tire.m_camber_stiffness_per_unit_gravity = 1000;
                desc.m_tire.m_type = 0;
                desc.m_tire.m_friction_vs_slip_graph[0][0] = 0;
                desc.m_tire.m_friction_vs_slip_graph[0][1] = 1;
                desc.m_tire.m_friction_vs_slip_graph[1][0] = 0.1;
                desc.m_tire.m_friction_vs_slip_graph[1][1] = 1;
                desc.m_tire.m_friction_vs_slip_graph[2][0] = 1;
                desc.m_tire.m_friction_vs_slip_graph[2][1] = 1;

                desc.m_suspension_travel_directions = nickel::Vec3{0, -1, 0};

                desc.m_suspension_force_app_point_offsets = mesh.m_transform.p;
                desc.m_tire_force_app_cm_offsets = mesh.m_transform.p;

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
                    ctx.CreateMaterial(0.1, 0.1, 0.1), true);
                shape.SetQueryFilterData(
                    nickel::physics::FilterData{CollisionGroupVehicleWheels});
                shape.SetSimulateFilterData(nickel::physics::FilterData{
                    COLLISION_FLAG_WHEEL, COLLISION_FLAG_WHEEL_AGAINST, 0, 0});
                desc.m_scene_query_filter_data.m_word0 = ~(
                    CollisionGroupVehicleWheels | CollisionGroupVehicleChassis);
                return std::make_tuple(desc, shape);
            };

            auto [driving_left_desc, driving_left_shape] = convert_to_wheel(
                ctx.GetPhysicsContext(), meshes[wheel_driving_left]);
            auto [driving_right_desc, driving_right_shape] = convert_to_wheel(
                ctx.GetPhysicsContext(), meshes[wheel_driving_right]);
            auto [steer_right_desc, steer_right_shape] = convert_to_wheel(
                ctx.GetPhysicsContext(), meshes[wheel_steer_right]);
            auto [steer_left_desc, steer_left_shape] = convert_to_wheel(
                ctx.GetPhysicsContext(), meshes[wheel_steer_left]);

            driving_left_desc.m_wheel.m_max_hand_brake_torque = 4000.0f;
            driving_left_desc.m_wheel.m_max_steer = 0;
            m_wheel_sim_desc.m_rear_left_wheel =
                m_wheel_sim_desc.m_wheels.size();
            m_wheel_sim_desc.m_wheels.push_back(driving_left_desc);

            driving_right_desc.m_wheel.m_max_hand_brake_torque = 4000.0f;
            driving_right_desc.m_wheel.m_max_steer = 0;
            driving_right_desc.m_suspension.m_camber_at_rest *= -1;
            driving_right_desc.m_suspension.m_camber_at_max_compression *= -1;
            driving_right_desc.m_suspension.m_camber_at_max_droop *= -1;
            m_wheel_sim_desc.m_rear_right_wheel =
                m_wheel_sim_desc.m_wheels.size();
            m_wheel_sim_desc.m_wheels.push_back(driving_right_desc);

            steer_left_desc.m_wheel.m_max_steer = nickel::PI * 0.33333f;
            steer_left_desc.m_wheel.m_max_hand_brake_torque = 0;
            m_wheel_sim_desc.m_front_left_wheel =
                m_wheel_sim_desc.m_wheels.size();
            m_wheel_sim_desc.m_wheels.push_back(steer_left_desc);

            steer_right_desc.m_wheel.m_max_steer = nickel::PI * 0.33333f;
            steer_right_desc.m_wheel.m_max_hand_brake_torque = 0;
            steer_right_desc.m_suspension.m_camber_at_rest *= -1;
            steer_right_desc.m_suspension.m_camber_at_max_compression *= -1;
            steer_right_desc.m_suspension.m_camber_at_max_droop *= -1;
            m_wheel_sim_desc.m_front_right_wheel =
                m_wheel_sim_desc.m_wheels.size();
            m_wheel_sim_desc.m_wheels.push_back(steer_right_desc);

            go.m_rigid_actor.AttachShape(driving_left_shape);
            go.m_rigid_actor.AttachShape(driving_right_shape);
            go.m_rigid_actor.AttachShape(steer_left_shape);
            go.m_rigid_actor.AttachShape(steer_right_shape);

            m_wheel_sim_desc.m_chassis_mass = 12.500f;

            for (auto& mesh : meshes) {
                if (mesh.m_name.find("chassis") != mesh.m_name.npos) {
                    auto convex_mesh =
                        ctx.GetPhysicsContext().CreateConvexMesh(mesh.m_points);
                    auto shape = ctx.GetPhysicsContext().CreateShape(
                        nickel::physics::ConvexMeshGeometry{
                            convex_mesh, mesh.m_transform.q,
                            mesh.m_transform.scale},
                        ctx.GetPhysicsContext().CreateMaterial(0.1, 0.1, 0.1),
                        true);
                    shape.SetQueryFilterData(nickel::physics::FilterData{
                        CollisionGroupVehicleChassis});
                    shape.SetSimulateFilterData(nickel::physics::FilterData{
                        COLLISION_FLAG_CHASSIS, COLLISION_FLAG_CHASSIS_AGAINST,
                        0, 0});
                    go.m_rigid_actor.AttachShape(shape);
                }
            }

            physics_ctx.GetMainScene().AddRigidActor(go.m_rigid_actor);

            m_drive_sim_desc.m_engine.m_moi = 1;
            m_drive_sim_desc.m_engine.m_peak_torque = 500;
            m_drive_sim_desc.m_engine.m_max_omega = 600;
            m_drive_sim_desc.m_engine.m_damping_rate_full_throttle = 0.15;
            m_drive_sim_desc.m_engine
                .m_damping_rate_zero_throttle_clutch_engaged = 2;
            m_drive_sim_desc.m_engine
                .m_damping_rate_zero_throttle_clutch_disengaged = 0.35;

            m_drive_sim_desc.m_gear.m_reverse_ratio = -4;
            m_drive_sim_desc.m_gear.m_neutral_ratio = 0;
            m_drive_sim_desc.m_gear.m_first_ratio = 4;
            m_drive_sim_desc.m_gear.m_final_ratio = 4;
            m_drive_sim_desc.m_gear.m_switch_time = 0.5;

            m_drive_sim_desc.m_clutch.m_estimate_iterations = 5;
            m_drive_sim_desc.m_clutch.m_strength = 10;

            m_drive_sim_desc.m_diff.m_type = nickel::physics::
                VehicleDifferential4WDescriptor::Type::LS_Rear_WD;

            go.m_vehicle = physics_ctx.GetVehicleManager().CreateVehicle4WDrive(
                m_wheel_sim_desc, m_drive_sim_desc,
                static_cast<nickel::physics::RigidDynamic&>(go.m_rigid_actor));

            root_go.m_children.push_back(go);
        }
    }

    void OnUpdate() override {
        auto& ctx = nickel::Context::GetInst();
        auto& keyboard = ctx.GetDeviceManager().GetKeyboard();
        auto& mouse = ctx.GetDeviceManager().GetMouse();

        updateFlyCamera();
        driveVehicle();
        drawGrid();

        ShowVehicleDebugPanel(ctx.GetCurrentLevel().GetRootGO().m_children[1],
                              m_wheel_sim_desc, m_drive_sim_desc);

        if (keyboard.GetKey(nickel::input::Key::LAlt).IsPressed()) {
            mouse.RelativeMode(mouse.IsRelativeMode() ? false : true);
        }

        ImGui::ShowDemoWindow();
    }

private:
    nickel::physics::VehicleWheelSim4WDescriptor m_wheel_sim_desc;
    nickel::physics::VehicleDriveSim4WDescriptor m_drive_sim_desc;

    void driveVehicle() {
        auto& ctx = nickel::Context::GetInst();
        auto& keyboard = ctx.GetDeviceManager().GetKeyboard();

        auto& go = ctx.GetCurrentLevel().GetRootGO().m_children[1];

        nickel::physics::Vehicle4W vehicle = go.m_vehicle.CastAs4W();
        vehicle.SetDigitalAccel(
            keyboard.GetKey(nickel::input::Key::Up).IsPressing());
        vehicle.SetDigitalBrake(
            keyboard.GetKey(nickel::input::Key::Down).IsPressing());
        vehicle.SetDigitalSteerLeft(
            keyboard.GetKey(nickel::input::Key::Left).IsPressing());
        vehicle.SetDigitalSteerRight(
            keyboard.GetKey(nickel::input::Key::Right).IsPressing());
        vehicle.SetGearUp(
            keyboard.GetKey(nickel::input::Key::RShift).IsPressing());
        vehicle.SetGearDown(
            keyboard.GetKey(nickel::input::Key::RCtrl).IsPressing());
    }

    void updateFlyCamera() {
        auto& ctx = nickel::Context::GetInst();
        nickel::FlyCamera& camera =
            static_cast<nickel::FlyCamera&>(ctx.GetCamera());
        auto& device_mgr = ctx.GetDeviceManager();
        auto& keyboard = device_mgr.GetKeyboard();
        auto& mouse = device_mgr.GetMouse();

        NICKEL_RETURN_IF_FALSE(mouse.IsRelativeMode());

        constexpr float speed = 0.01f;

        if (auto btn = keyboard.GetKey(nickel::input::Key::W);
            btn.IsPressing()) {
            camera.MoveForward(speed);
        }
        if (auto btn = keyboard.GetKey(nickel::input::Key::S);
            btn.IsPressing()) {
            camera.MoveForward(-speed);
        }
        if (auto btn = keyboard.GetKey(nickel::input::Key::A);
            btn.IsPressing()) {
            camera.MoveRightLeft(-speed);
        }
        if (auto btn = keyboard.GetKey(nickel::input::Key::D);
            btn.IsPressing()) {
            camera.MoveRightLeft(speed);
        }
        if (auto btn = keyboard.GetKey(nickel::input::Key::R);
            btn.IsPressing()) {
            camera.MoveUpDown(speed);
        }
        if (auto btn = keyboard.GetKey(nickel::input::Key::F);
            btn.IsPressing()) {
            camera.MoveUpDown(-speed);
        }

        constexpr nickel::Degrees rot_radians{0.1};
        auto offset = mouse.GetOffset();

        camera.AddYaw(-offset.x * rot_radians);
        camera.AddPitch(-offset.y * rot_radians);
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
};

NICKEL_RUN_APP(Application)
