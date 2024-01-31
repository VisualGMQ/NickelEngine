#pragma once

#include "common/cgmath.hpp"
#include "graphics/renderer2d.hpp"
#include "ui/context.hpp"
#include "common/hierarchy.hpp"

namespace nickel::ui {

/**
 * @brief [component] UI style, reference to CSS-Box-Model
 */
struct Style {
    cgmath::Vec2 center;
    cgmath::Vec2 size = {50, 25};
    cgmath::Vec2 padding;
    cgmath::Vec2 margin;

    cgmath::Color borderColor = {0, 0, 0, 1};
    cgmath::Color backgroundColor = {1, 1, 1, 1};
    int borderSize = 1;

    auto& GlobalCenter() const noexcept { return globalCenter_; }

private:
    cgmath::Vec2 globalCenter_;

    friend void doUpdateGlobalPosition(const Style& parentStyle, gecs::entity entity, gecs::registry reg);
    friend void UpdateGlobalPosition(gecs::querier<gecs::mut<Style>, Child, gecs::without<Parent>> root,
                          gecs::querier<gecs::mut<Style>, gecs::without<Child, Parent>> q,
                          gecs::registry reg);
};

}