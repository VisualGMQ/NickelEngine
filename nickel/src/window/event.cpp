#pragma once

#include "window/event.hpp"

namespace nickel {

typename gecs::world::registry_type* EventPoller::reg_ = nullptr;

void EventPoller::Poll() const {
    glfwPollEvents();
}

void EventPoller::ConnectPoller2Events(Window& window) {
    glfwSetKeyCallback((GLFWwindow*)window.Raw(), EventPoller::keyCallback);
    glfwSetCursorPosCallback((GLFWwindow*)window.Raw(),
                             EventPoller::mouseMotionCallback);
    glfwSetMouseButtonCallback((GLFWwindow*)window.Raw(),
                               EventPoller::mouseBtnCallback);
}

void EventPoller::keyCallback(GLFWwindow* window, int key, int scancode,
                              int action, int mods) {
    KeyboardEvent event;

    event.action = static_cast<Action>(action);
    event.key = static_cast<Key>(key);
    event.mod = static_cast<Modifier>(mods);

    EventPoller::getRegistry().event_dispatcher<KeyboardEvent>().enqueue(event);
}

void EventPoller::mouseBtnCallback(GLFWwindow* window, int button, int action,
                                   int mods) {
    MouseButtonEvent event;

    event.action = static_cast<Action>(action);
    event.btn = static_cast<MouseButtonType>(button);
    event.mod = static_cast<Modifier>(mods);

    EventPoller::getRegistry().event_dispatcher<MouseButtonEvent>().enqueue(
        event);
}

void EventPoller::mouseMotionCallback(GLFWwindow* window, double xpos,
                                      double ypos) {
    MouseMotionEvent event;
    event.position.x = static_cast<float>(xpos);
    event.position.y = static_cast<float>(ypos);

    EventPoller::getRegistry().event_dispatcher<MouseMotionEvent>().enqueue(
        event);
}

}  // namespace nickel