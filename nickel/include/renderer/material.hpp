#pragma once

#include "pch.hpp"
#include "renderer/texture.hpp"
#include "misc/asset_manager.hpp"

namespace nickel {

class Material {
public:
    virtual ~Material() = default;

    virtual void Bind2GPU() = 0;
};

class Material2D : public Material {
public:
    TextureHandle texture;

    void Bind2GPU() override {
        auto mgr = gWorld->res<AssetManager>();
        if (mgr->Has(texture)) {
            auto& image = mgr->Get(texture);
            image.texture_->Bind();
        }
    }
};

}  // namespace nickel