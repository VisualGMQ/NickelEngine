#include "nickel/common/macro.hpp"
#include "nickel/main_entry/runtime.hpp"
#include "nickel/nickel.hpp"

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
        mgr.Load(
            "tests/render/gltf/assets/CesiumMilkTruck/CesiumMilkTruck.gltf");
        mgr.Load("engine/assets/models/unit_box/unit_box.gltf");
        mgr.Load("engine/assets/models/unit_sphere/unit_sphere.gltf");
        auto& root_go = ctx.GetCurrentLevel().GetRootGO();

        {
            nickel::GameObject go;
            go.m_model = mgr.Find("engine/assets/models/unit_sphere/unit_sphere");
            auto& physics_ctx = ctx.GetPhysicsContext();
            go.m_rigid_actor = nickel::physics::RigidActor{
                physics_ctx.CreateRigidDynamic(nickel::Vec3{}, nickel::Quat{})};
            auto material = physics_ctx.CreateMaterial(1.0, 1.0, 0.1);
            auto loader = nickel::graphics::GLTFVertexDataLoader{};
            auto vertices = loader.Load("engine/assets/models/unit_box/unit_box.gltf");
            auto triangle_mesh = physics_ctx.CreateTriangleMesh(vertices[0].m_points, vertices[0].m_indices);
            auto shape = physics_ctx.CreateShape(
                nickel::physics::TriangleMeshGeometry{
                    triangle_mesh, vertices[0].m_transform.q,
                    vertices[0].m_transform.scale},
                material);
            shape.SetLocalPose({0, 1.5, 0}, {});
            ((nickel::physics::RigidDynamic*)&go.m_rigid_actor)->EnableKinematic(true);
            go.m_rigid_actor.AttachShape(shape);
            physics_ctx.GetMainScene().AddRigidActor(go.m_rigid_actor);
            root_go.m_children.push_back(go);
        }
        {
            nickel::GameObject go;
            go.m_model =
                mgr.Find("engine/assets/models/unit_sphere/unit_sphere.gltf");
            go.m_transform.p = nickel::Vec3{3, 0, 0};
            root_go.m_children.push_back(go);
        }
    }

    void OnUpdate() override {
        logicUpdate();
        updateCamera();
        drawGrid();

        auto& ctx = nickel::Context::GetInst();

        auto& keyboard = ctx.GetDeviceManager().GetKeyboard();
        auto& mouse = ctx.GetDeviceManager().GetMouse();
        if (keyboard.GetKey(nickel::input::Key::LAlt).IsPressed()) {
            mouse.RelativeMode(mouse.IsRelativeMode() ? false : true);
        }

        ImGui::ShowDemoWindow();
    }

private:
    void logicUpdate() {
        auto& ctx = nickel::Context::GetInst();
        auto& keyboard = ctx.GetDeviceManager().GetKeyboard();
        auto& mouse = ctx.GetDeviceManager().GetMouse();

        NICKEL_RETURN_IF_FALSE(!mouse.IsRelativeMode());

        nickel::GameObject& go =
            ctx.GetCurrentLevel().GetRootGO().m_children[0];
        if (keyboard.GetKey(nickel::input::Key::W).IsPressing()) {
            go.m_transform.p.z += 0.005f;
        }
        if (keyboard.GetKey(nickel::input::Key::S).IsPressing()) {
            go.m_transform.p.z -= 0.005f;
        }
        if (keyboard.GetKey(nickel::input::Key::A).IsPressing()) {
            go.m_transform.p.x += 0.005f;
        }
        if (keyboard.GetKey(nickel::input::Key::D).IsPressing()) {
            go.m_transform.p.x -= 0.005f;
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

        constexpr float speed = 0.1f;

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

        constexpr nickel::Degrees rot_radians{1};
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

