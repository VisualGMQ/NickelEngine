#include "config/config.hpp"
#include "core/log.hpp"
#include "core/log_tag.hpp"
#include "input/device.hpp"
#include "input/input.hpp"
#include "misc/timer.hpp"
#include "pch.hpp"
#include "renderer/sprite.hpp"
#include "window/event.hpp"
#include "window/window.hpp"
#include "renderer/camera.hpp"
#include "misc/dllopen.hpp"
#include "refl/drefl.hpp"
#include "anim/anim.hpp"
#include "nickel.hpp"

using namespace nickel;

std::unique_ptr<gecs::world> world;

void BootstrapSystem(gecs::world& world, typename gecs::world::registry_type& reg);

void ErrorCallback(int error, const char* description) {
    LOGE(log_tag::Glfw, description);
}

void BootstrapCallSystem() {
    BootstrapSystem(*world, *world->cur_registry());
}

void VideoSystemUpdate(gecs::resource<EventPoller> poller,
                       gecs::resource<Window> window,
                       gecs::resource<gecs::mut<Renderer2D>> renderer2d) {
    poller->Poll();
    window->SwapBuffer();
    renderer2d->Clear();
}

void VideoSystemShutdown(gecs::commands cmds) {}

void InputSystemInit(
    gecs::commands cmds,
    gecs::event_dispatcher<MouseButtonEvent> mouseBtnDispatcher,
    gecs::event_dispatcher<MouseMotionEvent> mouseMotionDispatcher,
    gecs::event_dispatcher<KeyboardEvent> keyboardDispatcher) {
    auto& keyboard = cmds.emplace_resource<Keyboard>();
    cmds.emplace_resource<Mouse>();
    ConnectInput2Event(mouseBtnDispatcher, mouseMotionDispatcher,
                       keyboardDispatcher);

    std::unordered_map<std::string, Key> actions = {
        {    "up", Key::W},
        {  "down", Key::S},
        {  "left", Key::A},
        { "right", Key::D},
        {"attack", Key::J},
        {  "jump", Key::K}
    };
    // read actions from config file
    // IMPROVE: use serd to auto-parse(now serd don't support serialize
    // unordered_map)
    auto parseResult = toml::parse_file("./nickel-config.toml");
    if (!parseResult) {
        LOGW(log_tag::Config, "parse ",
             "nickel-config.toml"
             " failed, use default actions.\nError: ",
             parseResult.error());
    } else {
        const auto& tbl = parseResult.table();
        if (!tbl["input-action"].is_table()) {
            LOGW(log_tag::Config,
                 "\"input-action\" table not exists, use default actions");
        } else {
            for (auto& [key, value] : tbl) {
                if (!value.is_string()) {
                    LOGW(log_tag::Config, "value of input action ", key,
                         " is not string!");
                } else {
                    actions[std::string(key.str())] =
                        GetKeyFromName(value.as_string()->get());
                }
            }
        }
    }

    cmds.emplace_resource<Input>(
        std::make_unique<KeyboardInput>(keyboard, actions));
}

int main(int argc, char** argv) {
    LOGI(log_tag::Nickel, "Running dir: ", std::filesystem::current_path(),
         ". Full path: ", argv[0]);
    glfwSetErrorCallback(ErrorCallback);

    if (!glfwInit()) {
        LOGE(log_tag::Glfw, "init glfw failed");
        return -1;
    }

    world = std::make_unique<gecs::world>();

    auto& main_reg = world->regist_registry("MainReg");
    main_reg
        // startup systems
        .regist_startup_system<BootstrapCallSystem>()
        .regist_startup_system<InputSystemInit>()
        // shutdown systems
        .regist_shutdown_system<VideoSystemShutdown>()
        // update systems
        .regist_update_system<VideoSystemUpdate>()
        // other input handle event must put here(after mouse/keyboard update)
        .regist_update_system<Mouse::Update>()
        .regist_update_system<Keyboard::Update>()
        .regist_update_system<SpriteBundle::RenderSprite>()
        .regist_update_system<Time::Update>();
    world->start_with("MainReg");

    world->startup();
    auto window = main_reg.res<Window>();

    while (!window->ShouldClose()) {
        world->update();
    }

    world->shutdown();
    world.reset();
    glfwTerminate();
    return 0;
}
