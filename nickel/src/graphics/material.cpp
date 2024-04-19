#include "graphics/material.hpp"
#include "graphics/context.hpp"

namespace nickel {

Material2D::Material2D() {}

Material2D::Material2D(TextureHandle handle, rhi::SamplerAddressMode u,
                       rhi::SamplerAddressMode v, rhi::Filter min,
                       rhi::Filter mag)
    : texture_{handle} {
    samplerDesc_.u = u;
    samplerDesc_.v = v;
    samplerDesc_.min = min;
    samplerDesc_.mag = mag;

    auto mgr = ECS::Instance().World().res<TextureManager>();
    if (!mgr->Has(texture_)) {
        LOGE(log_tag::Asset,
             "Texture is null when create texture");
    } else {
        // TODO: maybe we need function `Update()` to update part of bindGroup
        // for more performance?
        bindGroup_.Destroy();
        bindGroup_ = createBindGroup(mgr->Get(texture_).View());
    }
}

Material2D::~Material2D() {
    bindGroup_.Destroy();
}

Material3D::~Material3D() {
    bindGroup.Destroy();
}

rhi::Sampler::Descriptor& Material2D::GetSamplerDesc() {
    return const_cast<rhi::Sampler::Descriptor&>(
        std::as_const(*this).GetSamplerDesc());
}

const rhi::Sampler::Descriptor& Material2D::GetSamplerDesc() const {
    return samplerDesc_;
}

TextureHandle Material2D::GetTexture() const {
    return texture_;
}

rhi::BindGroup Material2D::GetBindGroup() const {
    return bindGroup_;
}

void Material2D::ChangeTexture(TextureHandle handle) {
    texture_ = handle;
    auto mgr = ECS::Instance().World().res<TextureManager>();
    if (!mgr->Has(texture_)) {
        LOGE(log_tag::Asset,
             "Texture is null when change texture. Texture changed failed");
    } else {
        // TODO: maybe we need function `Update()` to update part of bindGroup
        // for more performance?
        bindGroup_.Destroy();
        createBindGroup(mgr->Get(texture_).View());
    }
}

void Material2D::ChangeSampler(rhi::SamplerAddressMode u,
                               rhi::SamplerAddressMode v, rhi::Filter min,
                               rhi::Filter mag) {
    samplerDesc_.u = u;
    samplerDesc_.v = v;
    samplerDesc_.min = min;
    samplerDesc_.mag = mag;

    auto mgr = ECS::Instance().World().res<TextureManager>();

    if (!mgr->Has(texture_)) {
        LOGE(log_tag::Asset,
             "Texture is null when change sampler. Sampler changed failed");
    } else {
        // TODO: maybe we need function `Update()` to update part of bindGroup
        // for more performance?
        bindGroup_.Destroy();
        createBindGroup(mgr->Get(texture_).View());
    }
}

rhi::BindGroup Material2D::createBindGroup(rhi::TextureView texture) {
    auto ctx = ECS::Instance().World().res<RenderContext>();
    auto device = ECS::Instance().World().res_mut<rhi::Device>();

    rhi::BindGroup::Descriptor desc;
    desc.layout = ctx->ctx2D->bindGroupLayout;

    // sampler binding
    {
        rhi::BindingPoint entry;
        entry.binding = 1;
        rhi::SamplerBinding binding;
        binding.sampler = ctx->ctx2D->GetSampler(
            samplerDesc_.u, samplerDesc_.v, samplerDesc_.min, samplerDesc_.mag);
        entry.entry = binding;

        desc.entries.emplace_back(std::move(entry));
    }

    // texture binding
    {
        rhi::BindingPoint entry;
        entry.binding = 2;
        rhi::TextureBinding binding;
        binding.view = texture;
        entry.entry = binding;

        desc.entries.emplace_back(std::move(entry));
    }

    return device->CreateBindGroup(desc);
}

TextureBundle::~TextureBundle() {
    view.Destroy();
    texture.Destroy();
}

}  // namespace nickel