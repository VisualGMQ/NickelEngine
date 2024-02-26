#include "misc/argv.hpp"
#include "nickel.hpp"
#include "SDL.h"

using namespace nickel;

void BootstrapSystem(gecs::world& world, typename gecs::world::registry_type& reg);

void BootstrapCallSystem() {
    BootstrapSystem(ECS::Instance().World(), *ECS::Instance().World().cur_registry());
}

int main(int argc, char** argv) {
    // _CrtSetDbgFlag(_CRTDBG_ALLOC_MEM_DF | _CRTDBG_LEAK_CHECK_DF);
    // _CrtSetReportMode(_CRT_WARN, _CRTDBG_MODE_FILE);
    // _CrtSetReportFile(_CRT_WARN, _CRTDBG_FILE_STDERR);
    InitProfile("nickelengine.profile");

    LOGI(log_tag::Nickel, "Running dir: ", std::filesystem::current_path(),
         ". Full path: ", argv[0]);

    if (SDL_Init(SDL_INIT_EVERYTHING) != 0) {
        LOGF(log_tag::SDL2, "SDL init failed!");
        return 1;
    }
    SDL_SetHint(SDL_HINT_IME_SHOW_UI, "1");

    auto& world = ECS::Instance().World();

    auto& main_reg = world.regist_registry("MainReg");
    main_reg.commands().emplace_resource<CmdLineArgs>(argc, argv);

    main_reg.regist_startup_system<BootstrapCallSystem>();

    world.start_with("MainReg");

    world.startup();
    auto window = main_reg.res<Window>();

    while (!window->ShouldClose()) {
        world.update();
    }

    world.shutdown();

    SDL_Quit();
    return 0;
}
