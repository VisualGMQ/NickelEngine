#pragma once

#include "common/cgmath.hpp"
#include "common/ecs.hpp"

namespace nickel::ui {

enum class Event {
    None = 0,
    Hover,
    Press,
    Release,

    MaxEventCount,
};

/**
 * @brief [resource][inner]
 */
struct EventRecorder {
    gecs::entity entity = gecs::null_entity;
    std::array<bool, static_cast<size_t>(Event::MaxEventCount)> events;
    cgmath::Rect region;

    void PushEvent(Event event) {
        events[static_cast<size_t>(event)] = true;
    }

    void RemoveEvent(Event event) {
        events[static_cast<size_t>(event)] = false;
    }

    bool HasEvent(Event event) const {
        return events[static_cast<size_t>(event)];
    }
};

/**
 * @brief [component] save functions to handle events
 */
struct EventHandler {
    using OnClick = std::function<void(gecs::entity, gecs::registry)>;
    using OnHover = std::function<void(gecs::entity, gecs::registry)>;

    OnClick onClick;
    OnHover onHover;
};

}