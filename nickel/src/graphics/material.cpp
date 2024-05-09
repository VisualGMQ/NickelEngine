#include "graphics/material.hpp"
#include "graphics/context.hpp"

namespace nickel {

Material2D Material2D::Null;

Material2D::Material2D() {
}

Material2D::Material2D(const toml::table& tbl) {
    auto elem = LoadAssetFromMetaTable<Material2D>(tbl);
    *this = std::move(*elem);
}

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
        LOGE(log_tag::Asset, "Texture is null when create texture");
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
        bindGroup_ = createBindGroup(mgr->Get(texture_).View());
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
        bindGroup_ = createBindGroup(mgr->Get(texture_).View());
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

Material2DHandle Material2DManager::Create(const std::filesystem::path& filename) {
    auto elem = std::make_unique<Material2D>();
    if (elem) {
        auto handle = Material2DHandle::Create();
        elem->AssociateFile(filename);
        storeNewItem(handle, std::move(elem));
        return handle;
    }
    return Material2DHandle::Null();
}

Material2DHandle Material2DManager::Create(
    const std::filesystem::path& filename , TextureHandle handle,
    rhi::SamplerAddressMode u, rhi::SamplerAddressMode v, rhi::Filter min,
    rhi::Filter mag) {
    auto elem = std::make_unique<Material2D>(handle, u, v, min, mag);
    if (elem) {
        elem->AssociateFile(filename);
        auto handle = Material2DHandle::Create();
        storeNewItem(handle, std::move(elem));
        return handle;
    }
    return Material2DHandle::Null();
}

Material2DHandle Material2DManager::Load(
    const std::filesystem::path& filename) {
    auto elem = ::nickel::LoadAssetFromMeta<Material2D>(filename);
    if (elem) {
        auto handle = Material2DHandle::Create();
        storeNewItem(handle, std::move(elem));
        return handle;
    }
    return Material2DHandle::Null();
}

toml::table Material2D::Save2Toml() const {
    toml::table tbl;
    auto mgr = ECS::Instance().World().res<TextureManager>();
    if (mgr->Has(texture_)) {
        tbl.emplace("texture", mgr->Get(texture_).RelativePath().string());
    }

    toml::table samplerTbl;
    samplerTbl.emplace("u", static_cast<uint32_t>(samplerDesc_.u));
    samplerTbl.emplace("v", static_cast<uint32_t>(samplerDesc_.v));
    samplerTbl.emplace("min", static_cast<uint32_t>(samplerDesc_.min));
    samplerTbl.emplace("mag", static_cast<uint32_t>(samplerDesc_.mag));

    tbl.emplace("sampler", samplerTbl);
    return tbl;
}

Material2D::operator bool() const noexcept {
    return bindGroup_;
}

template <>
std::unique_ptr<Material2D> LoadAssetFromMetaTable(const toml::table& tbl) {
    auto mgr = ECS::Instance().World().res<TextureManager>();

    TextureHandle texture;
    if (auto node = tbl.get("texture"); node && node->is_string()) {
        auto filename = node->as_string()->get();
        texture = mgr->Has(filename) ? mgr->GetHandle(filename)
                                     : TextureHandle::Null();
    }

    rhi::Sampler::Descriptor samplerDesc;
    if (auto node = tbl.get("sampler"); node->is_table()) {
        auto samplerTbl = node->as_table();
        if (auto n = samplerTbl->get("u"); n->is_integer()) {
            samplerDesc.u =
                static_cast<rhi::SamplerAddressMode>(n->as_integer()->get());
        }
        if (auto n = samplerTbl->get("v"); n->is_integer()) {
            samplerDesc.v =
                static_cast<rhi::SamplerAddressMode>(n->as_integer()->get());
        }
        if (auto n = samplerTbl->get("min"); n->is_integer()) {
            samplerDesc.min = static_cast<rhi::Filter>(n->as_integer()->get());
        }
        if (auto n = samplerTbl->get("mag"); n->is_integer()) {
            samplerDesc.mag = static_cast<rhi::Filter>(n->as_integer()->get());
        }
    }

    auto mtl =
        std::make_unique<Material2D>(texture, samplerDesc.u, samplerDesc.v,
                                     samplerDesc.min, samplerDesc.mag);
    return mtl;
}

}  // namespace nickel