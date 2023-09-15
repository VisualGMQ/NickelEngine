#include "config/config.hpp"
#include "core/log.hpp"
#include "core/log_tag.hpp"
#include "input/device.hpp"
#include "input/input.hpp"
#include "pch.hpp"
#include "window/event.hpp"
#include "window/window.hpp"

using namespace nickel;

gecs::world world;

void ErrorCallback(int error, const char* description) {
    LOGE(log_tag::Glfw, description);
}

void VideoSystemInit(gecs::commands cmds) {
    auto& window = cmds.emplace_resource<Window>(
        WindowBuilder::FromConfig("./nickel-config.toml").Build());
    cmds.emplace_resource<EventPoller>(EventPoller{});
    EventPoller::AssociatePollerAndECS(*world.cur_registry());
    EventPoller::ConnectPoller2Events(window);
}

void VideoSystemUpdate(gecs::resource<EventPoller> poller,
                       gecs::resource<Window> window) {
    poller->Poll();
    window->SwapBuffer();
}

void VideoSystemShutdown(gecs::commands cmds) {
    cmds.remove_resource<Window>();
}

void InputSystemInit(gecs::commands cmds,
                     gecs::event_dispatcher<MouseButtonEvent> mouseBtnDispatcher,
                     gecs::event_dispatcher<MouseMotionEvent> mouseMotionDispatcher,
                     gecs::event_dispatcher<KeyboardEvent> keyboardDispatcher) {
    auto& keyboard = cmds.emplace_resource<Keyboard>();
    cmds.emplace_resource<Mouse>();
    ConnectInput2Event(mouseBtnDispatcher, mouseMotionDispatcher, keyboardDispatcher);

    std::unordered_map<std::string, Key> actions = {
        {    "up", Key::W},
        {  "down", Key::S},
        {  "left", Key::A},
        { "right", Key::D},
        {"attack", Key::J},
        {  "jump", Key::K}
    };
    // read actions from config file
    // IMPROVE: use serd to auto-parse
    auto parseResult = toml::parse_file("./nickel-config.toml");
    if (!parseResult) {
        LOGW(log_tag::Config, "parse ", "nickel-config.toml" " failed, use default actions.\nError: ", parseResult.error());
    } else {
        const auto& tbl = parseResult.table(); 
        if (!tbl["input-action"].is_table()) {
            LOGW(log_tag::Config, "\"input-action\" table not exists, use default actions");
        } else {
            for (auto& [key, value] : tbl) {
                if (!value.is_string()) {
                    LOGW(log_tag::Config, "value of input action ", key, " is not string!");
                } else {
                    actions[std::string(key.str())] = GetKeyFromName(value.as_string()->get());
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

    auto& main_reg = world.regist_registry("MainReg");
    main_reg.regist_startup_system<VideoSystemInit>()
        .regist_startup_system<InputSystemInit>()
        .regist_shutdown_system<VideoSystemShutdown>()
        .regist_update_system<VideoSystemUpdate>()
        // other input handle event must put here(after mouse/keyboard update)
        .regist_update_system<Mouse::Update>()
        .regist_update_system<Keyboard::Update>();
    world.start_with("MainReg");

    world.startup();
    auto window = main_reg.res<Window>();

    while (!window->ShouldClose()) {
        world.update();
    }

    world.shutdown();
    glfwTerminate();
    return 0;
}
