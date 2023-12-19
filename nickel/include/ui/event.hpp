#pragma once

#include "pch.hpp"
#include "core/cgmath.hpp"

namespace nickel::ui {

enum class Event {
    None = -1,
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
    std::array<bool, static_cast<size_t>(Event::MaxEventCount)> events{false};
    cgmath::Rect region;

    void Reset() {
        events.fill(false);
    }

    void PushEvent(Event event) {
        events[static_cast<size_t>(event)] = true;
    }

    void RemoveEvent(Event event) {
        events[static_cast<size_t>(event)] = false;
    }

    bool HasEvent(gecs::entity ent, Event event) const {
        return ent == entity && events[static_cast<size_t>(event)];
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