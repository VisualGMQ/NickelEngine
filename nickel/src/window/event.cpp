#pragma once

#include "window/event.hpp"

namespace nickel {

void EventPoller::PollAndDispatch() const {
    glfwPollEvents();
}

}