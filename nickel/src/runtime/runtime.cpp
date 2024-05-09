#include "misc/argv.hpp"
#include "nickel.hpp"
#include "SDL.h"

using namespace nickel;

void BootstrapSystem(gecs::world& world, typename gecs::world::registry_type& reg);

void BootstrapCallSystem() {
    BootstrapSystem(ECS::Instance().World(), *ECS::Instance().World().cur_registry());
}

int initRuntime(int argc, char** argv) {
    // _CrtSetDbgFlag(_CRTDBG_ALLOC_MEM_DF | _CRTDBG_LEAK_CHECK_DF);
    // _CrtSetReportMode(_CRT_WARN, _CRTDBG_MODE_FILE);
    // _CrtSetReportFile(_CRT_WARN, _CRTDBG_FILE_STDERR);
    InitProfile("nickelengine.profile");

    LOGI(log_tag::Nickel, "Running dir: ", std::filesystem::current_path(),
         ". Full path: ", argv[0]);

    if (SDL_Init(SDL_INIT_VIDEO|SDL_INIT_EVENTS|SDL_INIT_TIMER) != 0) {
        LOGF(log_tag::SDL2, "SDL init failed!: ", SDL_GetError());
        return 1;
    }
    SDL_SetHint(SDL_HINT_IME_SHOW_UI, "1");

    auto& world = ECS::Instance().World();

    auto& main_reg = world.regist_registry("MainReg");
    main_reg.commands().emplace_resource<CmdLineArgs>(argc, argv);

    main_reg.regist_startup_system<BootstrapCallSystem>();

    world.start_with("MainReg");

    world.startup();
    return 0;
}

void mainloop() {
    auto& world = ECS::Instance().World();
    world.update();

    auto window = world.res<Window>();
    if (window->ShouldClose()) {
        world.shutdown();
        SDL_Quit();
#ifdef __EMSCRIPTEN__
        emscripten_cancel_main_loop();
#else
        exit(0);
#endif
    }
}

int main(int argc, char** argv) {
    if (int err = initRuntime(argc, argv); err != 0) {
        return err;
    }
#ifdef __EMSCRIPTEN__
    emscripten_set_main_loop(mainloop, 0, 1);
#else
    while (1) {
        mainloop();
    }
#endif
}