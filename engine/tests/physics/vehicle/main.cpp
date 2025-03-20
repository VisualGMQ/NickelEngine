#include "nickel/common/macro.hpp"
#include "nickel/graphics/internal/gltf_model_impl.hpp"
#include "nickel/main_entry/runtime.hpp"
#include "nickel/nickel.hpp"

#include "nickel/physics/collision_group.hpp"

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
            nickel::Vec3 chassis_centre_offset{0, -0.35, 0.25};

            nickel::GameObject go;
            go.m_name = "car";
            go.m_model = mgr.Find("tests/physics/vehicle/assets/car/car");
            auto rigid =
                physics_ctx.CreateRigidDynamic(nickel::Vec3{0, 5, -5}, {});
            rigid.SetMass(1500.f);
            rigid.SetMassSpaceInertiaTensor({3625, 3125, 1281});
            rigid.SetCenterOfMassLocalPose(chassis_centre_offset, {});
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

            std::vector<nickel::Vec3> wheel_centre_offset;
            wheel_centre_offset.resize(meshes.size() - 1);
            for (uint32_t i = 1; i < meshes.size(); ++i) {
                wheel_centre_offset[i - 1] = meshes[i].m_transform.p;
            }

            auto sprung_masses = nickel::physics::ComputeVehicleSprungMass(
                wheel_centre_offset, chassis_centre_offset, 1500);

            auto convert_to_wheel =
                [=](nickel::physics::Context& ctx,
                    const nickel::graphics::GLTFVertexData& mesh, uint32_t i) {
                    nickel::physics::VehicleWheelSimDescriptor::WheelDescriptor
                        desc;
                    desc.m_wheel.m_width = 0.4f;
                    desc.m_wheel.m_radius = 0.5f;
                    desc.m_wheel.m_mass = 20.0f;
                    desc.m_wheel.m_moi = 2.5;
                    desc.m_wheel_centre_cm_offsets = mesh.m_transform.p;

                    desc.m_suspension.m_max_compression = 0.3;
                    desc.m_suspension.m_max_droop = 0.1;
                    desc.m_suspension.m_spring_strength = 35000;
                    desc.m_suspension.m_spring_damper_rate = 4500;
                    desc.m_suspension.m_sprung_mass = sprung_masses[i - 1];

                    desc.m_suspension_travel_directions =
                        nickel::Vec3{0, -1, 0};

                    auto wheel_centre_cmo_offset =
                        desc.m_wheel_centre_cm_offsets - chassis_centre_offset;
                    desc.m_suspension_force_app_point_offsets = {
                        wheel_centre_cmo_offset.x, -0.3,
                        wheel_centre_cmo_offset.z};
                    desc.m_tire_force_app_cm_offsets = {
                        wheel_centre_cmo_offset.x, -0.3,
                        wheel_centre_cmo_offset.z};

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
                    shape.SetSimulateBehaviorNoCollide(
                        nickel::physics::CollisionGroup::VehicleChassis);
                    return std::make_tuple(desc, shape);
                };

            auto [driving_left_desc, driving_left_shape] = convert_to_wheel(
                ctx.GetPhysicsContext(), meshes[wheel_driving_left],
                wheel_driving_left);
            auto [driving_right_desc, driving_right_shape] = convert_to_wheel(
                ctx.GetPhysicsContext(), meshes[wheel_driving_right],
                wheel_driving_right);
            auto [steer_right_desc, steer_right_shape] =
                convert_to_wheel(ctx.GetPhysicsContext(),
                                 meshes[wheel_steer_right], wheel_steer_right);
            auto [steer_left_desc, steer_left_shape] =
                convert_to_wheel(ctx.GetPhysicsContext(),
                                 meshes[wheel_steer_left], wheel_steer_left);

            driving_left_desc.m_wheel.m_max_hand_brake_torque = 4000.0f;
            driving_left_desc.m_wheel.m_max_steer = 0;
            m_wheel_sim_desc.m_rear_left_wheel =
                m_wheel_sim_desc.m_wheels.size();
            m_wheel_sim_desc.m_wheels.push_back(driving_left_desc);

            driving_right_desc.m_wheel.m_max_hand_brake_torque = 4000.0f;
            driving_right_desc.m_wheel.m_max_steer = 0;
            m_wheel_sim_desc.m_rear_right_wheel =
                m_wheel_sim_desc.m_wheels.size();
            m_wheel_sim_desc.m_wheels.push_back(driving_right_desc);

            steer_left_desc.m_wheel.m_max_steer = nickel::PI * 0.33333f;
            steer_left_desc.m_wheel.m_max_hand_brake_torque = 0;
            steer_left_desc.m_wheel.m_max_brake_torque = 2500.0f;
            m_wheel_sim_desc.m_front_left_wheel =
                m_wheel_sim_desc.m_wheels.size();
            m_wheel_sim_desc.m_wheels.push_back(steer_left_desc);

            steer_right_desc.m_wheel.m_max_steer = nickel::PI * 0.33333f;
            steer_right_desc.m_wheel.m_max_hand_brake_torque = 0;
            steer_right_desc.m_wheel.m_max_brake_torque = 2500.0f;
            m_wheel_sim_desc.m_front_right_wheel =
                m_wheel_sim_desc.m_wheels.size();
            m_wheel_sim_desc.m_wheels.push_back(steer_right_desc);

            go.m_rigid_actor.AttachShape(driving_left_shape);
            go.m_rigid_actor.AttachShape(driving_right_shape);
            go.m_rigid_actor.AttachShape(steer_left_shape);
            go.m_rigid_actor.AttachShape(steer_right_shape);

            m_wheel_sim_desc.m_chassis_mass = 1500;

            nickel::physics::VehicleWheelSimDescriptor::AntiRollBar front_bar;
            front_bar.m_wheel0 = wheel_steer_left - 1;
            front_bar.m_wheel1 = wheel_steer_right - 1;
            front_bar.stiffness = 10000;
            m_wheel_sim_desc.m_anti_roll_bars.push_back(front_bar);

            nickel::physics::VehicleWheelSimDescriptor::AntiRollBar rear_bar;
            rear_bar.m_wheel0 = wheel_driving_left - 1;
            rear_bar.m_wheel1 = wheel_driving_right - 1;
            rear_bar.stiffness = 10000;
            m_wheel_sim_desc.m_anti_roll_bars.push_back(rear_bar);

            for (auto& mesh : meshes) {
                if (mesh.m_name.find("chassis") != mesh.m_name.npos) {
                    auto convex_mesh =
                        ctx.GetPhysicsContext().CreateConvexMesh(mesh.m_points);
                    auto shape = ctx.GetPhysicsContext().CreateShape(
                        nickel::physics::ConvexMeshGeometry{
                            convex_mesh, mesh.m_transform.q,
                            mesh.m_transform.scale},
                        ctx.GetPhysicsContext().CreateMaterial(0.8, 0.8, 0.1),
                        true);
                    shape.SetCollisionGroup(
                        nickel::physics::CollisionGroup::VehicleChassis);
                    shape.SetSimulateBehaviorNoCollide(
                        nickel::physics::CollisionGroup::VehicleWheel);
                    go.m_rigid_actor.AttachShape(shape);
                }
            }

            physics_ctx.GetMainScene().AddRigidActor(go.m_rigid_actor);

            m_drive_sim_desc.m_engine.m_peak_torque = 500;
            m_drive_sim_desc.m_engine.m_max_omega = 600;

            m_drive_sim_desc.m_gear.m_reverse_ratio = -4;
            m_drive_sim_desc.m_gear.m_neutral_ratio = 0;
            m_drive_sim_desc.m_gear.m_first_ratio = 4;
            m_drive_sim_desc.m_gear.m_final_ratio = 4;
            m_drive_sim_desc.m_gear.m_switch_time = 0.5;

            m_drive_sim_desc.m_clutch.m_strength = 10;

            m_drive_sim_desc.m_diff.m_type =
                nickel::physics::VehicleDifferential4WDescriptor::Type::LS_4_WD;

            auto vehicle = physics_ctx.GetVehicleManager().CreateVehicle4WDrive(
                m_wheel_sim_desc, m_drive_sim_desc,
                static_cast<nickel::physics::RigidDynamic&>(go.m_rigid_actor));

            nickel::physics::VehicleSteerVsForwardTable table;
            table.Add(0.0, 0.75);
            table.Add(5.0, 0.75);
            table.Add(30.0, 0.125);
            table.Add(120.0, 0.1);
            vehicle.SetSteerVsForwardSpeedLookupTable(table);
            go.m_vehicle = vehicle;
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
