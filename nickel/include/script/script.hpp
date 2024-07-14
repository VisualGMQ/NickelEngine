#pragma once

#include "common/asset.hpp"

struct lua_State;

namespace nickel {

class Script;
class ScriptManager;

class LuaScript : public Asset {
public:
    friend void ScriptUpdateSystem(gecs::querier<gecs::mut<Script>> scripts,
                        gecs::resource<gecs::mut<ScriptManager>> mgr);

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

    bool Load(const toml::table&) override;
    bool Save(toml::table&) const override;

    operator bool() const {
        return state_;
    }

private:
    lua_State* state_{};
    bool isInited_ = false;

    bool load(const std::filesystem::path& path);

    friend void swap(LuaScript& o1, LuaScript& o2) {
        using std::swap;

        swap(o1.state_, o2.state_);
        swap(o1.isInited_, o2.isInited_);
    }
};

using ScriptHandle = Handle<LuaScript>;

class Script final {
public:
    ScriptHandle handle;
    gecs::entity entity;
};


void ScriptUpdateSystem(gecs::querier<gecs::mut<Script>> scripts,
                        gecs::resource<gecs::mut<ScriptManager>> mgr);
void ScriptShutdownSystem(gecs::querier<LuaScript>);

}  // namespace nickel