#include "imgui_window.hpp"
#include "nickel/common/macro.hpp"
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

        ImGuiWindowManager::Init(m_vehicle_params);
    }

    void OnUpdate() override {
        ImGuiWindowManager::GetInst().Update();

        driveVehicle();
        updateCamera();
        drawGrid();
    }

    void OnQuit() override { ImGuiWindowManager::Delete(); }

    void driveVehicle() {
        auto& ctx = nickel::Context::GetInst();
        auto& keyboard = ctx.GetDeviceManager().GetKeyboard();

        auto& root_go = ctx.GetCurrentLevel().GetRootGO();
        NICKEL_RETURN_IF_FALSE(root_go.m_children.size() >= 2);

        auto& go = root_go.m_children[1];

        nickel::physics::Vehicle4W vehicle = go.m_vehicle.CastAs4W();
        vehicle.SetDigitalAccel(
            keyboard.GetKey(nickel::input::Key::Up).IsPressing());
        vehicle.SetDigitalBrake(
            keyboard.GetKey(nickel::input::Key::Down).IsPressing());
        vehicle.SetDigitalSteerLeft(
            keyboard.GetKey(nickel::input::Key::Right).IsPressing());
        vehicle.SetDigitalSteerRight(
            keyboard.GetKey(nickel::input::Key::Left).IsPressing());
        vehicle.SetGearUp(
            keyboard.GetKey(nickel::input::Key::RShift).IsPressing());
        vehicle.SetGearDown(
            keyboard.GetKey(nickel::input::Key::RCtrl).IsPressing());
    }

private:
    VehicleParams m_vehicle_params;

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
};

NICKEL_RUN_APP(Application)
