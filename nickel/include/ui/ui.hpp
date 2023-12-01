#pragma once

#include "input/device.hpp"
#include "misc/hierarchy.hpp"
#include "ui/button.hpp"
#include "ui/context.hpp"
#include "ui/event.hpp"
#include "ui/label.hpp"
#include "ui/style.hpp"


namespace nickel::ui {

void InitSystem(gecs::commands cmds);
void RenderStyle(gecs::querier<Style>, gecs::resource<gecs::mut<Renderer2D>>,
                 gecs::resource<Context>);

void RenderUI(gecs::querier<Style, gecs::without<Parent>> querier,
              gecs::resource<gecs::mut<Renderer2D>> renderer,
              gecs::resource<Context> ctx, gecs::registry reg);

void UpdateGlobalPosition(
    gecs::querier<gecs::mut<Style>, Child, gecs::without<Parent>>,
    gecs::querier<gecs::mut<Style>, gecs::without<Child, Parent>>,
    gecs::registry);

void HandleEventSystem(gecs::resource<gecs::mut<Context>>,
                       gecs::querier<Style, gecs::without<Parent>>,
                       gecs::querier<Style, Child, gecs::without<Parent>>,
                       gecs::resource<Mouse>, gecs::registry);

}  // namespace nickel::ui