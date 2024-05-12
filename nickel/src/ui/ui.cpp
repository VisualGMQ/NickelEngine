#include "ui/ui.hpp"
#include "common/profile.hpp"
#include "video/window.hpp"

namespace nickel::ui {

void InitSystem(gecs::commands cmds, gecs::resource<rhi::Adapter> adapter,
                gecs::resource<gecs::mut<rhi::Device>> device,
                gecs::resource<gecs::mut<RenderContext>> renderCtx,
                gecs::resource<Window> window) {
    PROFILE_BEGIN();

    cmds.emplace_resource<UIContext>(adapter.get(), device.get(),
                                     renderCtx.get(), window->Size());

    PROFILE_END();
}

void ShutdownSystem(gecs::commands cmds) {
    PROFILE_BEGIN();

    cmds.remove_resource<UIContext>();

    PROFILE_END();
}

void doUpdateGlobalPosition(const Style& parentStyle, gecs::entity entity,
                            gecs::registry reg) {
    if (!reg.has<Style>(entity)) {
        return;
    }

    auto& style = reg.get_mut<Style>(entity);
    style.globalCenter_ = parentStyle.globalCenter_ + style.center;

    if (reg.has<Child>(entity)) {
        auto& children = reg.get<Child>(entity);
        for (auto e : children.entities) {
            doUpdateGlobalPosition(style, e, reg);
        }
    }
}

void UpdateGlobalPosition(
    gecs::querier<gecs::mut<Style>, Child, gecs::without<Parent>> root,
    gecs::querier<gecs::mut<Style>, gecs::without<Child, Parent>> q,
    gecs::registry reg) {
    PROFILE_BEGIN();

    for (auto&& [_, style, child] : root) {
        style.globalCenter_ = style.center;
        for (auto c : child.entities) {
            doUpdateGlobalPosition(style, c, reg);
        }
    }

    for (auto&& [_, style] : q) {
        style.globalCenter_ = style.center;
    }
}

void collectRenderElem(gecs::entity ent, const Style& style, const Button* btn,
                       const Label* label, UIContext& ctx,
                       const RenderContext& renderCtx) {
    auto rect = nickel::cgmath::Rect::FromCenter(
        style.GlobalCenter(), (style.size + style.padding) * 0.5);

    auto color = &style.backgroundColor;
    TextureClip texture;

    if (btn) {
        texture = btn->texture;

        if (ctx.eventRecorder.entity == ent) {
            if (ctx.eventRecorder.HasEvent(Event::Hover)) {
                color = &btn->hoverColor;
                texture = btn->hoverTexture;
            }
            if (ctx.eventRecorder.HasEvent(Event::Press)) {
                color = &btn->pressColor;
                texture = btn->pressTexture;
            }
        }
    }

    ctx.renderCtx.AddElement(rect, *color, texture, ctx.renderCtx.fillPipeline);

    /*
    color = &label->color;
    auto& rects = label->GetRenderRects();
    auto& boundingSize = label->GetBoundingBoxSize();
    if (label) {
        auto& textTextures = label->GetTextCache().Texts();
        auto& texts = label->GetText();
        for (int i = 0; i < texts.size(); i++) {
            auto& c = textTextures[i];
            auto& t = texts[i];

            if (t.is_white_space()) {
                continue;
            }

            if (i >= textTextures.size()) {
                continue;
            }

            cgmath::Rect region = {{}, c.size};
            region.position = rects[i].position + rect.position +
                              rect.size * 0.5 - boundingSize * 0.5;

            if (ctx.eventRecorder.entity == ent) {
                if (ctx.eventRecorder.HasEvent(Event::Hover)) {
                    color = &label->hoverColor;
                }
                if (ctx.eventRecorder.HasEvent(Event::Press)) {
                    color = &label->pressColor;
                }
            }

            ctx.renderCtx.AddElement(region, *color, texture,
                                     ctx.renderCtx.fillPipeline);
        }
    }
    */
}

void RenderUI(gecs::querier<Style, gecs::without<Parent>> querier,
              gecs::resource<gecs::mut<UIContext>> ctx,
              gecs::resource<gecs::mut<RenderContext>> renderCtx,
              gecs::resource<gecs::mut<Camera>> camera,
              gecs::resource<Material2DManager> mtl2dMgr, gecs::registry reg) {
    PROFILE_BEGIN();

    ctx->renderCtx.vertices.clear();
    ctx->renderCtx.batchBreakInfos.clear();

    for (auto&& [entity, style] : querier) {
        auto contentRect =
            cgmath::Rect::FromCenter(style.GlobalCenter(), style.size * 0.5);

        collectRenderElem(entity, style, reg.try_get<Button>(entity),
                          reg.try_get<Label>(entity), ctx.get(),
                          renderCtx.get());
    }

    Assert(ctx->renderCtx.vertices.size() < RenderUIContext::MaxRectSize * 4,
           "ui vertex size out of range");

    auto ptr = ctx->renderCtx.vertexBuffer.GetMappedRange();
    memcpy(ptr, ctx->renderCtx.vertices.data(),
           sizeof(UIVertex) * ctx->renderCtx.vertices.size());

    if (!ctx->renderCtx.vertexBuffer.IsMappingCoherence()) {
        ctx->renderCtx.vertexBuffer.Flush();
    }

    rhi::RenderPass::Descriptor desc;
    rhi::RenderPass::Descriptor::ColorAttachment colorAtt;
    colorAtt.loadOp = rhi::AttachmentLoadOp::Load;
    colorAtt.storeOp = rhi::AttachmentStoreOp::Store;

    auto target = ctx->renderCtx.camera.GetTarget();

    if (target) {
        colorAtt.view = target;
    } else {
        colorAtt.view = renderCtx->presentTextureView;
    }
    desc.colorAttachments.emplace_back(colorAtt);

    auto renderPass = renderCtx->encoder.BeginRenderPass(desc);
    for (auto& batch : ctx->renderCtx.batchBreakInfos) {
        renderPass.SetPipeline(batch.pipeline);
        std::array<cgmath::Mat44, 2> mats{ctx->renderCtx.camera.Project(),
                                          ctx->renderCtx.camera.View()};
        renderPass.SetPushConstant(rhi::ShaderStage::Vertex, mats.data(), 0,
                                   sizeof(cgmath::Mat44) * 2);
        auto& viewport = camera->GetViewport();
        renderPass.SetViewport(viewport.position.x, viewport.position.y,
                               viewport.size.w, viewport.size.h);
        renderPass.SetVertexBuffer(0, ctx->renderCtx.vertexBuffer, 0,
                                   sizeof(UIVertex) * batch.count);
        renderPass.SetIndexBuffer(
            ctx->renderCtx.indexBuffer, rhi::IndexType::Uint32, 0,
            sizeof(uint32_t) * ctx->renderCtx.vertices.size());

        auto bindGroup = ctx->renderCtx.FindBindGroup(batch.texture);
        renderPass.SetBindGroup(bindGroup);
        renderPass.DrawIndexed(6 * batch.count, 1, batch.start * 4, 0, 0);
    }
    renderPass.End();
}

inline cgmath::Rect getResponseRect(const Style& s) {
    return cgmath::Rect::FromCenter(s.GlobalCenter(),
                                    (s.size + s.padding) * 0.5);
}

void doDetermineEventEntity(EventRecorder& recorder, const Child& children,
                            const Mouse& mouse, gecs::registry reg) {
    for (auto e : children.entities) {
        if (!reg.has<Style>(e)) continue;

        auto& style = reg.get<Style>(e);
        auto rect = getResponseRect(style);

        if (rect.IsPtIn(mouse.Position())) {
            recorder.entity = e;
            recorder.region = rect;
        }

        if (reg.has<Child>(e)) {
            doDetermineEventEntity(recorder, reg.get<Child>(e), mouse, reg);
        }
    }
}

void HandleEventSystem(gecs::resource<gecs::mut<UIContext>> ctx,
                       gecs::querier<Style, gecs::without<Parent>> querier,
                       gecs::querier<Style, Child, gecs::without<Parent>> roots,
                       gecs::resource<Mouse> mouse, gecs::registry reg) {
    PROFILE_BEGIN();

    EventRecorder recorder;

    // check event trigger entity
    for (auto&& [entity, style] : querier) {
        cgmath::Rect rect = getResponseRect(style);
        auto pos = mouse->Position();
        if (rect.IsPtIn(pos)) {
            recorder.entity = entity;
            recorder.region = rect;
        }
    }

    for (auto&& [_, style, child] : roots) {
        doDetermineEventEntity(recorder, child, mouse.get(), reg);
    }

    if (recorder.entity == gecs::null_entity) {
        ctx->eventRecorder = std::move(recorder);
        return;
    }

    // check event type
    recorder.PushEvent(Event::Hover);

    if (mouse->LeftBtn().IsPress()) {
        recorder.PushEvent(Event::Press);
    }

    if (mouse->LeftBtn().IsRelease()) {
        recorder.PushEvent(Event::Release);
    }

    // deal event
    if (reg.has<EventHandler>(recorder.entity)) {
        auto& handler = reg.get<EventHandler>(recorder.entity);

        if (recorder.HasEvent(Event::Hover)) {
            handler.onHover(recorder.entity, reg);
        }
        if (recorder.HasEvent(Event::Press)) {
            handler.onClick(recorder.entity, reg);
        }
    }

    ctx->eventRecorder = std::move(recorder);
}

}  // namespace nickel::ui
