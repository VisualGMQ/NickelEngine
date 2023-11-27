#include "pch.hpp"
#include "config/config.hpp"
#include "core/log.hpp"
#include "core/log_tag.hpp"
#include "gecs/entity/fwd.hpp"
#include "input/device.hpp"
#include "input/input.hpp"
#include "misc/hierarchy.hpp"
#include "misc/timer.hpp"
#include "renderer/sprite.hpp"
#include "window/event.hpp"
#include "window/window.hpp"
#include "renderer/camera.hpp"
#include "misc/dllopen.hpp"
#include "refl/drefl.hpp"
#include "anim/anim.hpp"
#include "nickel.hpp"

using namespace nickel;

std::unique_ptr<gecs::world> gWorld;
bool shouldExit = false;    // should app exit

void DetectAppShouldExit(const QuitEvent& event) {
    shouldExit = true;
}

void BootstrapSystem(gecs::world& world, typename gecs::world::registry_type& reg);

void BootstrapCallSystem() {
    BootstrapSystem(*gWorld, *gWorld->cur_registry());
}

void VideoSystemInit(gecs::event_dispatcher<QuitEvent> quit) {
    quit.sink().add<DetectAppShouldExit>();
}

void VideoSystemUpdate(gecs::resource<EventPoller> poller,
                       gecs::resource<Window> window,
                       gecs::resource<gecs::mut<Renderer2D>> renderer2d) {
    poller->Poll();
    window->SwapBuffer();
}

void VideoSystemShutdown(gecs::commands cmds) {}

void BeginRender(gecs::resource<gecs::mut<Renderer2D>> renderer, gecs::resource<Camera> camera) {
    renderer->BeginRender(camera.get());
}

void EndRender(gecs::resource<gecs::mut<Renderer2D>> renderer) {
    renderer->EndRender();
}

void EventPollerInit(gecs::commands cmds) {
    cmds.emplace_resource<EventPoller>(EventPoller{});
    EventPoller::AssociatePollerAndECS(*gWorld->cur_registry());
}

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
    // _CrtSetDbgFlag(_CRTDBG_ALLOC_MEM_DF | _CRTDBG_LEAK_CHECK_DF);
    // _CrtSetReportMode(_CRT_WARN, _CRTDBG_MODE_FILE);
    // _CrtSetReportFile(_CRT_WARN, _CRTDBG_FILE_STDERR);

    LOGI(log_tag::Nickel, "Running dir: ", std::filesystem::current_path(),
         ". Full path: ", argv[0]);

    if (SDL_Init(SDL_INIT_EVERYTHING) != 0) {
        LOGF(log_tag::SDL2, "SDL init failed!");
        return 1;
    }
    SDL_SetHint(SDL_HINT_IME_SHOW_UI, "1");

    gWorld = std::make_unique<gecs::world>();

    auto& main_reg = gWorld->regist_registry("MainReg");
    main_reg
        // startup systems
        .regist_startup_system<BootstrapCallSystem>()
        .regist_startup_system<VideoSystemInit>()
        .regist_startup_system<EventPollerInit>()
        .regist_startup_system<InputSystemInit>()
        // shutdown systems
        .regist_shutdown_system<VideoSystemShutdown>()
        // update systems
        .regist_update_system<VideoSystemUpdate>()
        // other input handle event must put here(after mouse/keyboard update)
        .regist_update_system<Mouse::Update>()
        .regist_update_system<Keyboard::Update>()
        .regist_update_system<HandleInputEvents>()
        .regist_update_system<UpdateGlobalTransform>()
        .regist_update_system<BeginRender>()
        .regist_update_system<RenderSprite>()
        .regist_update_system<EndRender>()
        .regist_update_system<Time::Update>();
    gWorld->start_with("MainReg");

    gWorld->startup();
    auto window = main_reg.res<Window>();

    while (!shouldExit) {
        gWorld->update();
    }

    gWorld->shutdown();
    gWorld.reset();

    SDL_Quit();
    return 0;
}
