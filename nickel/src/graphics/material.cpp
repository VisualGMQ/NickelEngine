#include "graphics/material.hpp"
#include "graphics/context.hpp"
#include "common/asset_manager.hpp"

namespace nickel {

Material2D Material2D::Null;

Material2D::Material2D(TextureHandle handle, rhi::SamplerAddressMode u,
                       rhi::SamplerAddressMode v, rhi::Filter min,
                       rhi::Filter mag)
    : texture_{handle} {
    samplerDesc_.u = u;
    samplerDesc_.v = v;
    samplerDesc_.min = min;
    samplerDesc_.mag = mag;

    if (!texture_) {
        texture_ = {};
        LOGE(log_tag::Asset, "Texture is null when create texture");
    } else {
        // TODO: maybe we need function `Update()` to update part of bindGroup
        // for more performance?
        auto supportSeparateSampler =
            RenderContext::Instance().adapter.Limits().supportSeparateSampler;

        bindGroup_.Destroy();
        bindGroup_ =
            createBindGroup(supportSeparateSampler, texture_.GetData()->View());
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

bool Material2D::Load(const toml::table& tbl) {
    TextureHandle texture;
    if (auto node = tbl.get("texture"); node && node->is_string()) {
        auto filename = node->as_string()->get();
        if (auto handle = AssetManager::Instance().Find<Texture>(filename); handle) {
            texture = handle;
        } else {
            return false;
        }
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

    *this = Material2D{texture, samplerDesc.u, samplerDesc.v, samplerDesc.min,
                       samplerDesc.mag};
    return true;
}

bool Material2D::Save(toml::table& tbl) const {
    if (texture_) {
        NICKEL_TOML_EMPLACE_NODE(
            tbl, "texture",
            texture_.GetDataConst()->GetRelativePath().string());
    }

    toml::table samplerTbl;
    NICKEL_TOML_EMPLACE_NODE(samplerTbl, "u",
                             static_cast<uint32_t>(samplerDesc_.u));
    NICKEL_TOML_EMPLACE_NODE(samplerTbl, "v",
                             static_cast<uint32_t>(samplerDesc_.v));
    NICKEL_TOML_EMPLACE_NODE(samplerTbl, "min",
                             static_cast<uint32_t>(samplerDesc_.min));
    NICKEL_TOML_EMPLACE_NODE(samplerTbl, "mag",
                             static_cast<uint32_t>(samplerDesc_.mag));

    NICKEL_TOML_EMPLACE_NODE(tbl, "sampler", samplerTbl);
    return true;
}

bool Material2D::ChangeTexture(TextureHandle handle) {
    texture_ = handle;
    if (!handle) {
        LOGE(log_tag::Asset, "Texture changed failed: texture invalid");
        return false;
    }
    // TODO: maybe we need function `Update()` to update part of bindGroup
    // to improve performance?
    bindGroup_.Destroy();

    auto supportSeparateSampler =
        RenderContext::Instance().adapter.Limits().supportSeparateSampler;
    bindGroup_ =
        createBindGroup(supportSeparateSampler, texture_.GetData()->View());
    return true;
}

bool Material2D::ChangeSampler(rhi::SamplerAddressMode u,
                               rhi::SamplerAddressMode v, rhi::Filter min,
                               rhi::Filter mag) {
    samplerDesc_.u = u;
    samplerDesc_.v = v;
    samplerDesc_.min = min;
    samplerDesc_.mag = mag;

    if (!texture_) {
        LOGE(log_tag::Asset, "Sampler changed failed: texture invalid");
        return false;
    } else {
        // TODO: maybe we need function `Update()` to update part of bindGroup
        // for more performance?
        bindGroup_.Destroy();
        auto supportSeparateSampler =
            RenderContext::Instance().adapter.Limits().supportSeparateSampler;
        bindGroup_ =
            createBindGroup(supportSeparateSampler, texture_.GetData()->View());
    }
    return true;
}

rhi::BindGroup Material2D::createBindGroup(bool supportSeparateSampler,
                                           rhi::TextureView texture) {
    auto& ctx = RenderContext::Instance();

    rhi::BindGroup::Descriptor desc;
    desc.layout = ctx.ctx2D->bindGroupLayout;

    // sampler binding
    {
        rhi::BindingPoint entry;
        entry.binding = 1;
        rhi::SamplerBinding binding;
        binding.sampler = ctx.ctx2D->GetSampler(
            samplerDesc_.u, samplerDesc_.v, samplerDesc_.min, samplerDesc_.mag);
        binding.name = "mySampler";
        if (!supportSeparateSampler) {
            binding.view = texture;
        }
        entry.entry = binding;

        desc.entries.emplace_back(std::move(entry));
    }

    if (supportSeparateSampler) {
        // texture binding
        {
            rhi::BindingPoint entry;
            entry.binding = 2;
            rhi::TextureBinding binding;
            binding.view = texture;
            entry.entry = binding;

            desc.entries.emplace_back(std::move(entry));
        }
    }

    return ctx.device.CreateBindGroup(desc);
}

TextureBundle::~TextureBundle() {
    view.Destroy();
    texture.Destroy();
}

Material2D::operator bool() const noexcept {
    return bindGroup_;
}

}  // namespace nickel