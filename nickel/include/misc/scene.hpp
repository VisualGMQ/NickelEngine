#pragma once

#include "core/asset.hpp"
#include "core/manager.hpp"

namespace nickel {

struct Scene final : public Asset {
    toml::table Save2Toml() const override {
        toml::table tbl;
        tbl.emplace("path", RelativePath());
        return tbl;
    }

    explicit Scene(const std::string& name) : name_(name) {}

    auto& Name() const { return name_; }

private:
    std::string name_;
};

using SceneHandle = Handle<Scene>;

class SceneManager : public Manager<Scene> {
public:
    SceneHandle AssociateFile(std::string_view name,
                              std::unique_ptr<Scene>&& scene) {
        auto handle = SceneHandle::Create();
        storeNewItem(handle, std::move(scene));
        return handle;
    }
};

}  // namespace nickel