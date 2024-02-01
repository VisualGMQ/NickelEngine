#include "system/video.hpp"
#include "common/profile.hpp"

namespace nickel {


void detectAppShouldExit(const QuitEvent& event,
                         gecs::resource<gecs::mut<Window>> win) {
    win->Close();
}

void VideoSystemInit(gecs::event_dispatcher<QuitEvent> quit,
                     gecs::commands cmds) {
    PROFILE_BEGIN();

    quit.sink().add<detectAppShouldExit>();
}

void VideoSystemUpdate(gecs::resource<EventPoller> poller,
                       gecs::resource<Window> window) {
    PROFILE_BEGIN();

    poller->Poll();
    window->SwapBuffer();
}


void EventPollerInit(gecs::commands cmds, gecs::registry reg) {
    PROFILE_BEGIN();

    cmds.emplace_resource<EventPoller>(EventPoller{});
}

}