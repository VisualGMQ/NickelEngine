#define SDL_MAIN_USE_CALLBACKS
#include "nickel/main_entry/runtime.hpp"
#include "SDL3/SDL_main.h"
#include "nickel/common/log.hpp"
#include "nickel/fs/dialog.hpp"
#include "nickel/nickel.hpp"

namespace nickel::main_entry {
Runtime::Runtime() {
    LOGI("init SDL");
    if (!SDL_Init(SDL_INIT_VIDEO | SDL_INIT_EVENTS | SDL_INIT_SENSOR |
                  SDL_INIT_GAMEPAD | SDL_INIT_JOYSTICK)) {
        LOGC("SDL init failed: {}", SDL_GetError());
    }

    Context::Init();

    auto& ctx = Context::GetInst();
    ctx.RegisterCustomApplication(
        ::CreateCustomApplication(Context::GetInst()));
    auto app = ctx.GetApplication();
    if (app) {
        app->OnInit();
    } else {
        LOGI("no custom application");
    }

    LOGI("running engine...");
}

Runtime::~Runtime() {
    Context::Delete();
}

void Runtime::Run() {
    auto& ctx = Context::GetInst();
    ctx.Update();
}

void Runtime::HandleEvent(const SDL_Event& event) {
    Context::GetInst().HandleEvent(event);
}

extern "C" {
SDL_AppResult SDL_AppInit(void** appstate, int argc, char** argv) {
    Runtime::Init();
    if (Context::GetInst().ShouldExit()) {
        return SDL_APP_FAILURE;
    }
    return SDL_APP_CONTINUE;
}

SDL_AppResult SDL_AppIterate(void* appstate) {
    auto& runtime = Runtime::GetInst();
    runtime.Run();
    if (Context::GetInst().ShouldExit()) {
        return SDL_APP_SUCCESS;
    }
    return SDL_APP_CONTINUE;
}

SDL_AppResult SDL_AppEvent(void* appstate, SDL_Event* event) {
    Runtime::GetInst().HandleEvent(*event);
    return SDL_APP_CONTINUE;
}

void SDL_AppQuit(void* appstate, SDL_AppResult result) {
    Runtime::Delete();
}
}
}

