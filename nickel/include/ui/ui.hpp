#pragma once

#include "video/device.hpp"
#include "common/hierarchy.hpp"
#include "ui/button.hpp"
#include "ui/context.hpp"
#include "ui/event.hpp"
#include "ui/label.hpp"
#include "ui/style.hpp"


namespace nickel::ui {

void InitSystem(gecs::commands, gecs::resource<rhi::Adapter>,
                gecs::resource<gecs::mut<rhi::Device>>,
                gecs::resource<gecs::mut<RenderContext>>,
                gecs::resource<Window>);

void ShutdownSystem(gecs::commands);

void RenderUI(gecs::querier<Style, gecs::without<Parent>>,
              gecs::resource<gecs::mut<UIContext>>,
              gecs::resource<gecs::mut<RenderContext>>,
              gecs::resource<gecs::mut<Camera>>,
              gecs::resource<Material2DManager>, gecs::registry);

void UpdateGlobalPosition(
    gecs::querier<gecs::mut<Style>, Child, gecs::without<Parent>>,
    gecs::querier<gecs::mut<Style>, gecs::without<Child, Parent>>,
    gecs::registry);

void HandleEventSystem(gecs::resource<gecs::mut<UIContext>>,
                       gecs::querier<Style, gecs::without<Parent>>,
                       gecs::querier<Style, Child, gecs::without<Parent>>,
                       gecs::resource<Mouse>, gecs::registry);

}  // namespace nickel::ui