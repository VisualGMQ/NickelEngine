#include "nickel/main_entry/runtime.hpp"
#include "nickel/nickel.hpp"

class Application : public nickel::Application {
public:
    void OnInit() override {
        auto& ctx = nickel::Context::GetInst();
        nickel::FlyCamera& camera = (nickel::FlyCamera&)ctx.GetCamera();

        camera.MoveTo(nickel::Vec3{0, 3, 3});
        camera.SetPitch(nickel::Degrees{45});
    }
    
    void OnUpdate() override {
        updateCamera();
        drawGrid();
        
        ImGui::ShowDemoWindow();
    }

private:
    void updateCamera() {
        auto& ctx = nickel::Context::GetInst();
        nickel::FlyCamera& camera = static_cast<nickel::FlyCamera&>(ctx.GetCamera());
        auto& device_mgr = ctx.GetDeviceManager();
        auto& keyboard = device_mgr.GetKeyboard();

        constexpr float speed = 0.1f;

        if (auto btn = keyboard.GetKey(nickel::input::Key::W); btn.
            IsPressing()) {
            camera.MoveForward(speed);
        }
        if (auto btn = keyboard.GetKey(nickel::input::Key::S); btn.
            IsPressing()) {
            camera.MoveForward(-speed);
        }
        if (auto btn = keyboard.GetKey(nickel::input::Key::A); btn.
            IsPressing()) {
            camera.MoveRightLeft(-speed);
        }
        if (auto btn = keyboard.GetKey(nickel::input::Key::D); btn.
            IsPressing()) {
            camera.MoveRightLeft(speed);
        }
        if (auto btn = keyboard.GetKey(nickel::input::Key::R); btn.
            IsPressing()) {
            camera.MoveUpDown(speed);
        }
        if (auto btn = keyboard.GetKey(nickel::input::Key::F); btn.
            IsPressing()) {
            camera.MoveUpDown(-speed);
        }
        
        auto& mouse = device_mgr.GetMouse();
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
                nickel::graphics::Vertex
                {nickel::Vec3(i, 0, HalfLineNum), color},
                nickel::graphics::Vertex{nickel::Vec3(i, 0, -HalfLineNum),
                                         color}
            };
            ctx.DrawLineStrip(vertices);
        }

        for (int i = -HalfLineNum; i <= HalfLineNum; i++) {
            if (i == 0) {
                continue;
            }
            nickel::graphics::Vertex vertices[] = {
                nickel::graphics::Vertex
                {nickel::Vec3(HalfLineNum, 0, i), color},
                nickel::graphics::Vertex{nickel::Vec3(-HalfLineNum, 0, i),
                                         color}
            };
            ctx.DrawLineStrip(vertices);
        }

        // draw axis
        {
            nickel::graphics::Vertex vertices[] = {
                nickel::graphics::Vertex{nickel::Vec3(0, 0, 0), nickel::Color{1, 0, 0, 1}},
                nickel::graphics::Vertex{nickel::Vec3(HalfLineNum, 0, 0), nickel::Color{1, 0, 0, 1}}
            };
            ctx.DrawLineStrip(vertices);
        }
        {
            nickel::graphics::Vertex vertices[] = {
                nickel::graphics::Vertex{nickel::Vec3(0, 0, 0), nickel::Color{0, 1, 0, 1}},
                nickel::graphics::Vertex{nickel::Vec3(0, HalfLineNum, 0), nickel::Color{0, 1, 0, 1}}
            };
            ctx.DrawLineStrip(vertices);
        }
        {
            nickel::graphics::Vertex vertices[] = {
                nickel::graphics::Vertex{nickel::Vec3(0, 0, 0), nickel::Color{0, 0, 1, 1}},
                nickel::graphics::Vertex{nickel::Vec3(0, 0, HalfLineNum), nickel::Color{0, 0, 1, 1}}
            };
            ctx.DrawLineStrip(vertices);
        }
    }
};

NICKEL_RUN_APP(Application)

