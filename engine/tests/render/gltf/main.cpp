#include "nickel/common/macro.hpp"
#include "nickel/importer/gltf_importer.hpp"
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
        auto engine_relative_path = ctx.GetEngineRelativePath();
        ImportGLTF2Engine(
            engine_relative_path /
                "engine/assets/models/CesiumMilkTruck/CesiumMilkTruck.gltf",
            {});
        m_model = mgr.Find("engine/assets/models/CesiumMilkTruck/CesiumMilkTruck");
    }

    void OnUpdate(float delta_time) override {
        updateCamera();
        drawGrid();
        
        auto& ctx = nickel::Context::GetInst();
        ctx.GetGraphicsContext().DrawModel({}, m_model);

        
        auto& keyboard = ctx.GetDeviceManager().GetKeyboard();
        auto& mouse = ctx.GetDeviceManager().GetMouse();
        if (keyboard.GetKey(nickel::input::Key::LAlt).IsPressed()) {
            mouse.RelativeMode(mouse.IsRelativeMode() ? false : true);
        }

        // ImGui::ShowDemoWindow();
    }

private:
    nickel::graphics::GLTFModel m_model;
    float m_move_speed = 0.1f;
    
    void updateCamera() {
        auto& ctx = nickel::Context::GetInst();
        nickel::FlyCamera& camera =
            static_cast<nickel::FlyCamera&>(ctx.GetCamera());
        auto& device_mgr = ctx.GetDeviceManager();
        auto& keyboard = device_mgr.GetKeyboard();
        auto& mouse = device_mgr.GetMouse();

        NICKEL_RETURN_IF_FALSE(mouse.IsRelativeMode());

        m_move_speed = std::max(m_move_speed + mouse.GetWheelDelta() * 0.01, 0.001);

        if (auto btn = keyboard.GetKey(nickel::input::Key::W);
            btn.IsPressing()) {
            camera.MoveForward(m_move_speed);
        }
        if (auto btn = keyboard.GetKey(nickel::input::Key::S);
            btn.IsPressing()) {
            camera.MoveForward(-m_move_speed);
        }
        if (auto btn = keyboard.GetKey(nickel::input::Key::A);
            btn.IsPressing()) {
            camera.MoveRightLeft(-m_move_speed);
        }
        if (auto btn = keyboard.GetKey(nickel::input::Key::D);
            btn.IsPressing()) {
            camera.MoveRightLeft(m_move_speed);
        }
        if (auto btn = keyboard.GetKey(nickel::input::Key::R);
            btn.IsPressing()) {
            camera.MoveUpDown(m_move_speed);
        }
        if (auto btn = keyboard.GetKey(nickel::input::Key::F);
            btn.IsPressing()) {
            camera.MoveUpDown(-m_move_speed);
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

NICKEL_RUN_APP(Application);