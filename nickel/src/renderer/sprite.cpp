#include "renderer/sprite.hpp"

namespace nickel {

RenderContext::RenderInfo generateRenderInfo(const Sprite& sprite,
                                             const Transform& transform,
                                             const GlobalTransform* gTrans,
                                             const TextureManager& mgr,
                                             RenderContext& renderCtx) {
    renderCtx.depthBias += renderCtx.depthBiasStep;
    float depth = sprite.zIndex + renderCtx.depthBias;

    cgmath::Mat44 trans = gTrans ? gTrans->mat : transform.ToMat();
    trans = trans *
            cgmath::CreateScale({sprite.flip & Flip::Horizontal ? -1.f : 1.f,
                                 sprite.flip & Flip::Vertical ? -1.f : 1.f, 1});

    cgmath::Rect region = {0, 0, 100, 50};
    cgmath::Vec2 customSize =
        sprite.customSize ? sprite.customSize.value() : cgmath::Vec2{100, 50};

    const Texture* texture = nullptr;

    if (mgr.Has(sprite.texture)) {
        texture = &mgr.Get(sprite.texture);
        region = sprite.region
                     ? sprite.region.value()
                     : cgmath::Rect{0, 0, static_cast<float>(texture->Width()),
                                    static_cast<float>(texture->Height())};
        customSize =
            sprite.customSize ? sprite.customSize.value() : texture->Size();
    }

    std::array<Vertex, 4> vertices = {
        Vertex{{0, 0, depth},
               {region.position.x / customSize.w,
               region.position.y / customSize.h},
               sprite.color},
        Vertex{{1, 0, depth},
               {(region.position.x + region.size.w) / customSize.w,
               region.position.y / customSize.h},
               sprite.color},
        Vertex{{0, 1, depth},
               {region.position.x / customSize.w,
               (region.position.y + region.size.h) / customSize.h},
               sprite.color},
        Vertex{{1, 1, depth},
               {(region.position.x + region.size.w) / customSize.w,
               (region.position.y + region.size.h) / customSize.h},
               sprite.color},
    };
    auto model =
        trans * cgmath::CreateScale({customSize.x, customSize.y, 1.0}) *
        cgmath::CreateTranslation({-sprite.anchor.x, -sprite.anchor.y, 0.0});

    return {vertices, model, texture};
}

void collectSpriteRenderInfoHierarchy(const TextureManager& mgr,
                                      RenderContext& renderCtx,
                                      const Child& child, gecs::registry reg) {
    for (auto e : child.entities) {
        if (reg.has<Sprite>(e) && reg.has<Transform>(e) &&
            reg.has<GlobalTransform>(e)) {
            auto& sprite = reg.get<Sprite>(e);
            auto& trans = reg.get<Transform>(e);
            auto& gTrans = reg.get<GlobalTransform>(e);

            renderCtx.renderInfos.emplace_back(
                generateRenderInfo(sprite, trans, &gTrans, mgr, renderCtx));
        }

        if (reg.has<Child>(e)) {
            collectSpriteRenderInfoHierarchy(mgr, renderCtx, reg.get<Child>(e),
                                             reg);
        }
    }
}

void CollectSpriteRenderInfo(
    gecs::querier<Sprite, Transform, gecs::without<Parent>> sprites,
    gecs::querier<Sprite, Transform, GlobalTransform, Child,
                  gecs::without<Parent>>
        spriteInHies,
    gecs::resource<TextureManager> textureMgr,
    gecs::resource<gecs::mut<RenderContext>> renderCtx, gecs::registry reg) {
    for (auto&& [_, sprite, transform] : sprites) {
        if (!sprite.visiable) {
            continue;
        }

        renderCtx->renderInfos.emplace_back(generateRenderInfo(
            sprite, transform, nullptr, textureMgr.get(), renderCtx.get()));
    }

    for (auto&& [_, sprite, trans, gTrans, child] : spriteInHies) {
        if (!sprite.visiable) {
            continue;
        }

        collectSpriteRenderInfoHierarchy(textureMgr.get(), renderCtx.get(),
                                         child, reg);
    }
}

void RenderElements(gecs::resource<gecs::mut<Renderer2D>> renderer2d,
                    gecs::resource<gecs::mut<RenderContext>> ctx,
                    gecs::resource<Camera> camera) {
    renderer2d->BeginRenderTexture(camera.get());

    auto& elems = ctx->renderInfos;
    std::sort(elems.begin(), elems.end(),
              [](const RenderContext::RenderInfo& e1,
                 const RenderContext::RenderInfo& e2) {
                  return e1.vertices[0].position.z < e2.vertices[0].position.z;
              });

    static auto indices = std::array<uint32_t, 6>{0, 1, 2, 1, 2, 3};

    for (auto& elem : elems) {
        renderer2d->Draw(gogl::PrimitiveType::Triangles, elem.vertices, indices,
                         elem.model, elem.texture);
    }

    renderer2d->EndRender();
}

}  // namespace nickel