#include "nickel.hpp"

using namespace nickel;

std::unique_ptr<gecs::world> gWorld;

void BootstrapSystem(gecs::world& world, typename gecs::world::registry_type& reg);

void BootstrapCallSystem() {
    BootstrapSystem(*gWorld, *gWorld->cur_registry());
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

    main_reg.regist_startup_system<BootstrapCallSystem>();
    RegistEngineSystem(main_reg);

    gWorld->start_with("MainReg");

    gWorld->startup();
    auto window = main_reg.res<Window>();

    while (!window->ShouldClose()) {
        gWorld->update();
    }

    gWorld->shutdown();
    gWorld.reset();

    SDL_Quit();
    return 0;
}
