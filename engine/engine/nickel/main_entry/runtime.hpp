#pragma once
#include "nickel/nickel.hpp"

#ifdef NICKEL_PLATFORM_ANDROID
#include "SDL3/SDL.h"
#endif


extern std::unique_ptr<nickel::Application> CreateCustomApplication(
    nickel::Context&);

union SDL_Event;

namespace nickel::main_entry {

class Runtime : public Singlton<Runtime, true> {
public:
    Runtime();
    ~Runtime();
    void Run();
    void HandleEvent(const SDL_Event&);
};

}  // namespace nickel::main_entry

#define NICKEL_GENERATE_APP_REGISTER(class_name)                  \
    std::unique_ptr<nickel::Application> CreateCustomApplication( \
        nickel::Context&) {                                       \
        return std::make_unique<class_name>();                    \
    }

#ifdef NICKEL_PLATFORM_ANDROID
#define NICKEL_RUN_APP(class_name)                              \
    NICKEL_GENERATE_APP_REGISTER(class_name);                   \
    int main(int argc, char** argv) {                           \
        nickel::main_entry::Runtime::Init();                    \
        auto& ctx = nickel::Context::GetInst();                 \
        auto& runtime = nickel::main_entry::Runtime::GetInst(); \
        SDL_Event event;                                        \
        while (!ctx.ShouldExit()) {                             \
            while (SDL_PollEvent(&event)) {                     \
                runtime.HandleEvent(event);                     \
            }                                                   \
            runtime.Run();                                      \
        }                                                       \
        nickel::main_entry::Runtime::Delete();                  \
        return EXIT_SUCCESS;                                    \
    }
#else
#define NICKEL_RUN_APP(class_name) NICKEL_GENERATE_APP_REGISTER(class_name)
#endif