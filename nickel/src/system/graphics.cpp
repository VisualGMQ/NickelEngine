#include "system/graphics.hpp"
#include "common/profile.hpp"

namespace nickel {

cgmath::Mat44 calcMatFromRenderInfo(Flip flip, const cgmath::Vec2& customSize,
                                    const cgmath::Vec2& anchor) {
    return cgmath::CreateScale(
               {customSize.x * (flip & Flip::Horizontal ? -1 : 1),
                customSize.y * (flip & Flip::Vertical ? -1 : 1), 1}) *
           cgmath::CreateTranslation({-anchor.x, -anchor.y, 0});
}

void UpdateCamera2GPU(gecs::resource<Camera> camera,
                      gecs::resource<gecs::mut<RenderContext>> ctx) {
    PROFILE_BEGIN();

    auto map = (cgmath::Mat44*)ctx->mvpBuffer.GetMappedRange();
    memcpy(map, camera->View().data, sizeof(cgmath::Mat44));
    memcpy(map + 1, camera->Project().data, sizeof(cgmath::Mat44));
    if (!ctx->mvpBuffer.IsMappingCoherence()) {
        ctx->mvpBuffer.Flush(0, sizeof(cgmath::Mat44) * 2);
    }

    PROFILE_END();
}

void BeginRender(gecs::resource<gecs::mut<rhi::Device>> device,
                 gecs::resource<gecs::mut<RenderContext>> ctx) {
    PROFILE_BEGIN();

    rhi::Texture::Descriptor textureDesc;
    textureDesc.format = rhi::TextureFormat::Presentation;
    auto [texture, view] = device->GetPresentationTexture();
    ctx->presentTexture = texture;
    ctx->presentTextureView = view;
    ctx->encoder = device->CreateCommandEncoder();

    PROFILE_END();
}

void draw(RenderContext& ctx, rhi::RenderPassEncoder renderPass,
          uint32_t count, rhi::Buffer vertexBuffer,
          uint32_t firstVertex, rhi::Buffer indexBuffer,
          uint32_t firstIndex, const Material2D& material,
          const nickel::cgmath::Mat44& model) {
    renderPass.SetPushConstant(rhi::ShaderStage::Vertex, &model.data, 0,
                               sizeof(model));
    renderPass.SetVertexBuffer(0, vertexBuffer, 0, vertexBuffer.Size());
    if (material) {
        renderPass.SetBindGroup(material.GetBindGroup());
    } else {
        renderPass.SetBindGroup(ctx.ctx2D->defaultBindGroup);
    }
    if (indexBuffer) {
        renderPass.SetIndexBuffer(indexBuffer, rhi::IndexType::Uint32, 0,
                                  indexBuffer.Size());
        renderPass.DrawIndexed(count, 1, firstIndex, firstVertex, 0);
    } else {
        renderPass.Draw(count, 1, firstVertex, 0);
    }
}

void drawMesh2D(RenderContext& ctx, rhi::RenderPassEncoder renderPass,
                const GPUMesh2D& mesh, const Material2D& material,
                const nickel::cgmath::Mat44& model) {
    draw(ctx, renderPass, mesh.elemCount, mesh.verticesBuffer, 0,
         mesh.indicesBuffer, 0, material, model);
}

void drawTexture(RenderContext& ctx, uint32_t slot,
                 rhi::RenderPassEncoder renderPass, const TextureManager& mgr,
                 const Material2D& material,
                 const std::optional<cgmath::Rect>& region,
                 const cgmath::Color& color, const cgmath::Mat44& model,
                 float z) {
    Vertex2D topLeft{
        {-0.5, 0.5, z},
        {},
        color
    };
    Vertex2D topRight{
        {0.5, 0.5, z},
        {},
        color
    };
    Vertex2D bottomLeft{
        {-0.5, -0.5, z},
        {},
        color
    };
    Vertex2D bottomRight{
        {0.5, -0.5, z},
        {},
        color
    };

    if (material) {
        auto size = mgr.Get(material.GetTexture()).Size();
        cgmath::Rect rect = region.value_or(cgmath::Rect{
            {0, 0},
            size
        });
        topLeft.texcoord.Set(rect.position.x / size.w,
                             rect.position.y / size.h);
        topRight.texcoord.Set((rect.position.x + rect.size.w) / size.w,
                              rect.position.y / size.h);
        bottomLeft.texcoord.Set(rect.position.x / size.w,
                                (rect.position.y + rect.size.h) / size.h);
        bottomRight.texcoord.Set((rect.position.x + rect.size.w) / size.w,
                                 (rect.position.y + rect.size.h) / size.h);

        std::array vertices{
            topRight, topLeft, bottomRight, bottomLeft,
        };

        auto buf = ctx.ctx2D->vertexBuffer;
        auto map = (Vertex2D*)buf.GetMappedRange();
        memcpy(map + slot * 4, vertices.data(), sizeof(vertices));

        if (!buf.IsMappingCoherence()) {
            buf.Flush();
        }

        draw(ctx, renderPass, 6, ctx.ctx2D->vertexBuffer, slot * 4,
             ctx.ctx2D->indexBuffer, 0, material, model);
    }
}

void RenderSprite2D(
    gecs::resource<gecs::mut<rhi::Device>> device,
    gecs::resource<gecs::mut<RenderContext>> ctx,
    gecs::resource<gecs::mut<Camera>> camera,
    gecs::resource<TextureManager> mgr,
    gecs::resource<Material2DManager> mtl2dMgr,
    gecs::querier<Transform, gecs::mut<Sprite>, SpriteMaterial> querier) {
    PROFILE_BEGIN();

    rhi::RenderPass::Descriptor desc;
    rhi::RenderPass::Descriptor::ColorAttachment colorAtt;
    colorAtt.loadOp = rhi::AttachmentLoadOp::Load;
    colorAtt.storeOp = rhi::AttachmentStoreOp::Store;

    auto target = camera->GetTarget();

    if (target) {
        colorAtt.view = target;
    } else {
        colorAtt.view = ctx->presentTextureView;
    }
    desc.colorAttachments.emplace_back(colorAtt);

    auto renderPass = ctx->encoder.BeginRenderPass(desc);
    renderPass.SetPipeline(ctx->ctx2D->pipeline);

    querier.sort_by<Sprite>([](const Sprite& sprite1, const Sprite& sprite2) {
        return sprite1.orderInLayer < sprite2.orderInLayer;
    });

    for (auto&& [_, transform, sprite, material] : querier) {
        if (!mtl2dMgr->Has(material.material)) {
            continue;
        }

        auto& mtl = mtl2dMgr->Get(material.material);

        if (!mgr->Has(mtl.GetTexture())) {
            continue;
        }

        if (!sprite.slot) {
            sprite.slot = ctx->ctx2D->GenVertexSlot();
        }

        auto& texture = mgr->Get(mtl.GetTexture());

        drawTexture(
            ctx.get(), sprite.slot.value(), renderPass, mgr.get(), mtl,
            sprite.region, sprite.color,
            transform.ToMat() * calcMatFromRenderInfo(
                                    sprite.flip,
                                    sprite.customSize.value_or(texture.Size()),
                                    sprite.anchor),
            sprite.orderInLayer);
    }

    renderPass.End();

    PROFILE_END();
}

void renderNodeRecursive(const GLTFModel& gltfModel, Node& node,
                         RenderContext& ctx,
                         rhi::RenderPassEncoder& renderPass) {
    auto& mesh = node.mesh;
    if (mesh) {
        for (auto& prim : node.mesh.primitives) {
            auto& material = gltfModel.materials[prim.material];

            if (prim.indicesBufView.size > 0) {
                renderPass.SetIndexBuffer(
                    mesh.indicesBuf, rhi::IndexType::Uint32,
                    prim.indicesBufView.offset, prim.indicesBufView.size);
            }

            renderPass.SetPushConstant(rhi::ShaderStage::Vertex,
                                       node.modelMat.data, 0,
                                       sizeof(nickel::cgmath::Mat44));
            renderPass.SetVertexBuffer(0, mesh.posBuf, prim.posBufView.offset,
                                       prim.posBufView.size);
            renderPass.SetVertexBuffer(1, mesh.uvBuf, prim.uvBufView.offset,
                                       prim.uvBufView.size);
            renderPass.SetVertexBuffer(2, mesh.normBuf, prim.normBufView.offset,
                                       prim.normBufView.size);
            renderPass.SetVertexBuffer(3, mesh.tanBuf, prim.tanBufView.offset,
                                       prim.tanBufView.size);

            renderPass.SetBindGroup(material->bindGroup,
                                    {material->pbrParameters.offset});

            if (prim.indicesBufView.size > 0) {
                renderPass.DrawIndexed(prim.indicesBufView.count, 1, 0, 0, 0);
            } else {
                renderPass.Draw(prim.posBufView.count, 1, 0, 0);
            }
        }
    }

    for (auto& child : node.children) {
        renderNodeRecursive(gltfModel, *child, ctx, renderPass);
    }
}

void renderScenes(const GLTFModel& model, RenderContext& ctx,
                  rhi::RenderPassEncoder& renderPass) {
    for (auto& scene : model.scenes) {
        renderNodeRecursive(model, *scene.node, ctx, renderPass);
    }
}

void RenderGLTFModel(gecs::resource<gecs::mut<RenderContext>> ctx,
                     gecs::resource<gecs::mut<Camera>> camera,
                     gecs::resource<gecs::mut<rhi::Device>> device,
                     gecs::resource<GLTFManager> mgr,
                     gecs::querier<GLTFHandle, Transform> querier) {
    PROFILE_BEGIN();

    rhi::RenderPass::Descriptor desc;
    rhi::RenderPass::Descriptor::ColorAttachment colorAtt;
    colorAtt.loadOp = rhi::AttachmentLoadOp::Clear;
    colorAtt.storeOp = rhi::AttachmentStoreOp::Store;
    auto& clearColor = camera->GetClearColor();
    colorAtt.clearValue = {clearColor.r, clearColor.g, clearColor.b,
                           clearColor.a};

    auto target = camera->GetTarget();

    if (target) {
        colorAtt.view = target;
    } else {
        colorAtt.view = ctx->presentTextureView;
    }
    desc.colorAttachments.emplace_back(colorAtt);

    desc.depthStencilAttachment =
        rhi::RenderPass::Descriptor::DepthStencilAttachment{};
    desc.depthStencilAttachment->view = ctx->depthTextureView;
    desc.depthStencilAttachment->depthLoadOp = rhi::AttachmentLoadOp::Clear;
    desc.depthStencilAttachment->depthStoreOp = rhi::AttachmentStoreOp::Discard;
    desc.depthStencilAttachment->depthReadOnly = false;
    desc.depthStencilAttachment->depthClearValue = 0.0;

    auto renderPass = ctx->encoder.BeginRenderPass(desc);
    auto viewport = camera->GetViewport();
    renderPass.SetViewport(viewport.position.x, viewport.position.y,
                           viewport.size.w, viewport.size.h);
    renderPass.SetPipeline(ctx->ctx3D->pipeline);

    for (auto&& [_, model, transform] : querier) {
        if (mgr->Has(model)) {
            renderScenes(mgr->Get(model), ctx.get(), renderPass);
        }
    }

    renderPass.End();

    PROFILE_END();
}

void EndRender(gecs::resource<gecs::mut<rhi::Device>> device,
               gecs::resource<gecs::mut<RenderContext>> ctx,
               gecs::resource<Window> window) {
    PROFILE_BEGIN();

    if (!window->IsMinimized()) {
        rhi::Queue queue = device->GetQueue();

        ctx->cmd = ctx->encoder.Finish();
        queue.Submit({ctx->cmd});
    }

    PROFILE_END();
}

void BeginFrame(gecs::resource<gecs::mut<rhi::Device>> device) {
    PROFILE_BEGIN();

    device->BeginFrame();

    PROFILE_END();
}

void EndFrame(gecs::resource<gecs::mut<rhi::Device>> device,
              gecs::resource<gecs::mut<RenderContext>> ctx) {
    PROFILE_BEGIN();

    device->EndFrame();

    ctx->encoder.Destroy();

    PROFILE_END();
}

void updateGLTFNodeTransformRecur(const cgmath::Mat44& parentMat, Node& node) {
    node.modelMat = parentMat * node.localModelMat;

    for (auto& child : node.children) {
        updateGLTFNodeTransformRecur(node.modelMat, *child);
    }
}

void UpdateGLTFModelTransform(
    gecs::querier<Transform, GLTFHandle, gecs::without<GlobalTransform>>
        querier,
    gecs::resource<gecs::mut<GLTFManager>> mgr) {
    for (auto&& [_, transform, handle] : querier) {
        if (!mgr->Has(handle)) {
            continue;
        }

        auto& model = mgr->Get(handle);
        for (auto& scene : model.scenes) {
            scene.node->localModelMat = transform.ToMat();
            updateGLTFNodeTransformRecur(transform.ToMat(), *scene.node);
        }
    }
}

}  // namespace nickel