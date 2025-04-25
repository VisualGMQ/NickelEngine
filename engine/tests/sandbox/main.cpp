#include "nickel/common/macro.hpp"
#include "nickel/graphics/internal/gltf_model_impl.hpp"
#include "nickel/main_entry/runtime.hpp"
#include "nickel/nickel.hpp"
#include "nickel/physics/cct.hpp"

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

        auto engine_relative_path = ctx.GetEngineRelativePath();

        auto& mgr = ctx.GetGLTFManager();
        mgr.Load(engine_relative_path /
                 "engine/assets/models/CesiumMilkTruck/CesiumMilkTruck.gltf");
        mgr.Load(engine_relative_path /
                 "engine/assets/models/CesiumMan/CesiumMan.gltf");
        mgr.Load(engine_relative_path /
                 "engine/assets/models/unit_box/unit_box.gltf");
        mgr.Load(engine_relative_path /
                 "engine/assets/models/unit_sphere/unit_sphere.gltf");
        mgr.Load(engine_relative_path / "tests/sandbox/assets/door/door.gltf");
        auto& root_go = ctx.GetCurrentLevel().GetRootGO();

        auto& physics_ctx = ctx.GetPhysicsContext();
        // create car
        {
            nickel::GameObject go;
            go.m_name = "car";
            go.m_model = mgr.Find(
                (engine_relative_path /
                 "engine/assets/models/CesiumMilkTruck/CesiumMilkTruck")
                    .ToString());
            go.m_rigid_actor =
                nickel::physics::RigidActor{physics_ctx.CreateRigidDynamic(
                    nickel::Vec3{3, 0, 0}, nickel::Quat{})};
            auto material = physics_ctx.CreateMaterial(1.0, 1.0, 0.1);
            auto shape = physics_ctx.CreateShape(
                nickel::physics::BoxGeometry{
                    nickel::Vec3{1.2, 1.2, 2.2}
            },
                material);
            shape.SetLocalPose({0, 1.5, 0}, {});
            go.m_rigid_actor.AttachShape(shape);
            physics_ctx.GetMainScene().AddRigidActor(go.m_rigid_actor);
            root_go.m_children.emplace_back(std::move(go));
        }
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
            root_go.m_children.emplace_back(std::move(go));
        }
        // create man
        {
            nickel::GameObject go;
            go.m_name = "saw";
            go.m_model = mgr.Find((engine_relative_path /
                                   "engine/assets/models/CesiumMan/CesiumMan")
                                      .ToString());
            go.m_transform.scale = nickel::Vec3{0.7};

            nickel::physics::CapsuleController::Descriptor desc;
            desc.m_radius = 0.25;
            desc.m_height = 0.5;
            desc.m_up_dir = nickel::Vec3{0, 1, 0};
            desc.m_scale_coeff = 1;
            desc.m_contact_offset = 0.01;
            desc.m_climbing_mode = nickel::physics::CapsuleController::
                Descriptor::ClimbingMode::Easy;
            desc.m_step_offset = 0;
            desc.m_material = physics_ctx.CreateMaterial(0.01, 0.01, 0.01);
            go.m_controller =
                physics_ctx.GetMainScene().CreateCapsuleController(desc);

            root_go.m_children.emplace_back(std::move(go));
        }

        // create door
        {
            nickel::GameObject go;
            go.m_name = "wall";
            go.m_model = mgr.Find(
                (engine_relative_path / "tests/sandbox/assets/door/door")
                    .ToString());
            go.m_rigid_actor =
                physics_ctx.CreateRigidStatic(nickel::Vec3{0, 0, 5}, {});

            nickel::graphics::GLTFVertexDataLoader loader;
            auto meshes = loader.Load("tests/sandbox/assets/door/door.gltf");

            auto material = physics_ctx.CreateMaterial(1.0, 1.0, 0.1);
            for (auto& mesh : meshes) {
                auto triangle_mesh = physics_ctx.CreateTriangleMesh(
                    mesh.m_points, mesh.m_indices);
                auto shape = physics_ctx.CreateShape(
                    nickel::physics::TriangleMeshGeometry{
                        triangle_mesh, mesh.m_transform.q,
                        mesh.m_transform.scale},
                    material);
                shape.SetLocalPose(mesh.m_transform.p, mesh.m_transform.q);
                go.m_rigid_actor.AttachShape(shape);
            }
            physics_ctx.GetMainScene().AddRigidActor(go.m_rigid_actor);

            root_go.m_children.emplace_back(std::move(go));
        }
    }

    void OnUpdate(float delta_time) override {
        auto& ctx = nickel::Context::GetInst();
        auto& keyboard = ctx.GetDeviceManager().GetKeyboard();
        auto& mouse = ctx.GetDeviceManager().GetMouse();

        updateCamera();
        if (mode == Mode::Character) {
            shootBall();
            moveCharacter();
        }
        if (keyboard.GetKey(nickel::input::Key::P).IsPressed()) {
            if (mode == Mode::Character) {
                mode = Mode::Fly;
            } else {
                mode = Mode::Character;
            }
        }
        drawGrid();

        if (keyboard.GetKey(nickel::input::Key::LAlt).IsPressed()) {
            mouse.RelativeMode(mouse.IsRelativeMode() ? false : true);
        }

        ImGui::ShowDemoWindow();
    }

private:
    enum class Mode {
        Fly,
        Character,
    } mode = Mode::Fly;

    void moveCharacter() {
        auto& ctx = nickel::Context::GetInst();
        auto& keyboard = ctx.GetDeviceManager().GetKeyboard();
        auto& camera = (nickel::FlyCamera&)ctx.GetCamera();

        constexpr float speed = 0.01f;
        nickel::GameObject& go =
            ctx.GetCurrentLevel().GetRootGO().m_children[2];
        auto forward = camera.GetForward();
        forward.y = 0;
        Normalize(forward);
        nickel::Vec3 up = nickel::Vec3{0, 1, 0};
        nickel::Vec3 left = Cross(up, forward);

        nickel::Vec3 disp;

        if (keyboard.GetKey(nickel::input::Key::W).IsPressing()) {
            disp += forward * speed;
        }
        if (keyboard.GetKey(nickel::input::Key::S).IsPressing()) {
            disp -= forward * speed;
        }
        if (keyboard.GetKey(nickel::input::Key::A).IsPressing()) {
            disp += left * speed;
        }
        if (keyboard.GetKey(nickel::input::Key::D).IsPressing()) {
            disp -= left * speed;
        }

        disp += nickel::Vec3{0, -9.8, 0} * 0.008;

        go.m_controller.MoveAndSlide(disp, 0.00001f, 0.008);
        go.m_transform.q =
            nickel::Quat::Create(nickel::Vec3{0, 1, 0}, camera.GetYaw());

        camera.MoveTo(go.m_controller.GetPosition());
    }

    void shootBall() {
        auto& ctx = nickel::Context::GetInst();
        auto& keyboard = ctx.GetDeviceManager().GetKeyboard();
        auto& camera = (nickel::FlyCamera&)ctx.GetCamera();

        constexpr float force = 20;

        // create ball
        if (keyboard.GetKey(nickel::input::Key::Space).IsPressed()) {
            auto& mgr = ctx.GetGLTFManager();
            nickel::GameObject ball_go;
            ball_go.m_transform.scale = nickel::Vec3{0.3};
            ball_go.m_name = "ball";
            ball_go.m_model =
                mgr.Find((ctx.GetEngineRelativePath() /
                          "engine/assets/models/unit_sphere/unit_sphere")
                             .ToString());
            auto& physics_ctx = ctx.GetPhysicsContext();
            ball_go.m_rigid_actor =
                nickel::physics::RigidActor{physics_ctx.CreateRigidDynamic(
                    camera.GetPosition() + camera.GetForward() * 1,
                    nickel::Quat{})};
            auto material = physics_ctx.CreateMaterial(1.0, 1.0, 0.1);
            auto shape = physics_ctx.CreateShape(
                nickel::physics::SphereGeometry{0.3}, material);
            ball_go.m_rigid_actor.AttachShape(shape);
            physics_ctx.GetMainScene().AddRigidActor(ball_go.m_rigid_actor);
            auto& root_go = ctx.GetCurrentLevel().GetRootGO();
            static_cast<nickel::physics::RigidDynamic&>(ball_go.m_rigid_actor)
                .AddForce(camera.GetForward() * force,
                          nickel::physics::ForceMode::Impulse);
            root_go.m_children.emplace_back(std::move(ball_go));
        }
    }

    void updateCamera() {
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
