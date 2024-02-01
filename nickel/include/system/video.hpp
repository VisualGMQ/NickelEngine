#pragma once

#include "video/window.hpp"
#include "common/ecs.hpp"
#include "video/event.hpp"
#include "video/device.hpp"
#include "video/input.hpp"

namespace nickel {

class QuitEvent;
class EventPoller;

void VideoSystemInit(gecs::event_dispatcher<QuitEvent> quit,
                     gecs::commands cmds);

void VideoSystemUpdate(gecs::resource<EventPoller> poller,
                       gecs::resource<Window> window);

void EventPollerInit(gecs::commands cmds, gecs::registry);

}