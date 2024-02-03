#pragma once

#include "common/asset.hpp"
#include "common/manager.hpp"

struct lua_State;

namespace nickel {

class LuaScript : public Asset {
public:
    friend void ScriptUpdateSystem(gecs::querier<gecs::mut<LuaScript>>);

    static LuaScript Null;

    LuaScript() = default;
    LuaScript(const LuaScript&) = delete;
    LuaScript& operator=(const LuaScript&) = delete;

    LuaScript(LuaScript&& o) : Asset(std::move(o)) { swap(o, *this); }

    LuaScript& operator=(LuaScript&& o) {
        Asset::operator=(std::move(o));
        if (&o != this) {
            swap(o, *this);
        }
        return *this;
    }

    explicit LuaScript(const std::filesystem::path& libname);
    ~LuaScript();

    void OnInit(gecs::entity) const;
    void OnUpdate(gecs::entity) const;
    void OnDestroy(gecs::entity) const;

    bool IsInited() const { return isInited_; }

    toml::table Save2Toml() const override;

    operator bool() const {
        return state_;
    }

private:
    lua_State* state_{};
    bool isInited_ = false;

    void load(const std::filesystem::path& path);

    friend void swap(LuaScript& o1, LuaScript& o2) {
        using std::swap;

        swap(o1.state_, o2.state_);
        swap(o1.isInited_, o2.isInited_);
    }
};

using ScriptHandle = Handle<LuaScript>;

template <>
std::unique_ptr<LuaScript> LoadAssetFromMeta(const toml::table&);

class ScriptManager: public Manager<LuaScript> {
public:
    ScriptHandle Load(const std::filesystem::path& path);

    auto GetFileType() const { return FileType::Script; }
};

class Script final {
public:
    ScriptHandle handle;
};

void ScriptUpdateSystem(gecs::querier<gecs::mut<LuaScript>>);
void ScriptShutdownSystem(gecs::querier<LuaScript>);

}  // namespace nickel