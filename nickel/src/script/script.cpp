#include "script/script.hpp"
#include "script/luabind.hpp"
#include "nickel.hpp"

#include "lua.h"
#include "luacode.h"
#include "lualib.h"

#include "LuaBridge/Array.h"
#include "LuaBridge/LuaBridge.h"
#include "LuaBridge/Vector.h"

namespace nickel {

#define LUAU_CALL(expr)                                   \
    do {                                                  \
        auto result = (expr);                             \
        if (!result) {                                    \
            LOGE(log_tag::Script, result.errorMessage()); \
        }                                                 \
    } while (0)

LuaScript LuaScript::Null;

template <>
std::unique_ptr<LuaScript> LoadAssetFromMeta(const toml::table& path) {
    if (auto node = path.get("path"); node && node->is_string()) {
        return std::make_unique<LuaScript>(node->as_string()->get());
    }
    return nullptr;
}

LuaScript::LuaScript(const std::filesystem::path& libname) : Asset(libname) {
    state_ = luaL_newstate();
    luaL_openlibs(state_);
    load(libname);
    BindLua(state_);
}

LuaScript::~LuaScript() {
    if (state_) {
        lua_close(state_);
    }
}

void LuaScript::OnInit(gecs::entity ent) const {
    if (!state_) {
        return;
    }

    if (auto ref = luabridge::getGlobal(state_, "on_init"); ref) {
        auto entity = std::underlying_type_t<gecs::entity>(ent);
        LUAU_CALL(ref(entity));
    }
}

void LuaScript::OnUpdate(gecs::entity ent) const {
    if (!state_) {
        return;
    }
    if (auto ref = luabridge::getGlobal(state_, "on_update"); ref) {
        auto entity = std::underlying_type_t<gecs::entity>(ent);
        LUAU_CALL(ref(entity));
    }
}

void LuaScript::OnDestroy(gecs::entity) const {
    if (!state_) {
        return;
    }
    if (auto ref = luabridge::getGlobal(state_, "on_quit"); ref) {
        LUAU_CALL(ref());
    }
}

void LuaScript::load(const std::filesystem::path& path) {
    size_t bytecodeSize = 0;
    auto filename = path.string();
    auto codes = ReadWholeFile<std::string>(path);
    if (codes) {
        char* bytecode =
            luau_compile(codes->c_str(), codes->size(), NULL, &bytecodeSize);
        int result =
            luau_load(state_, filename.c_str(), bytecode, bytecodeSize, 0);
        free(bytecode);

        if (result != LUA_OK) {
            LOGE(log_tag::Script, "load script ", path, "failed");
        } else {
            lua_pcall(state_, 0, 0, 0);
        }
    }
}

toml::table LuaScript::Save2Toml() const {
    toml::table tbl;
    tbl.emplace("path", RelativePath().string());
    return tbl;
}

ScriptHandle ScriptManager::Load(const std::filesystem::path& path) {
    auto data = std::make_unique<LuaScript>(path);
    if (data) {
        auto handle = ScriptHandle::Create();
        storeNewItem(handle, std::move(data));
        return handle;
    }
    return ScriptHandle::Null();
}

void ScriptUpdateSystem(gecs::querier<gecs::mut<Script>> scripts,
                        gecs::resource<gecs::mut<ScriptManager>> mgr) {
    for (auto&& [entity, script] : scripts) {
        if (!mgr->Has(script.handle)) {
            continue;
        }

        auto& luaScript = mgr->Get(script.handle);
        if (!luaScript.isInited_) {
            luaScript.OnInit(entity);
            luaScript.isInited_ = true;
        }
        luaScript.OnUpdate(entity);
    }
}

void ScriptShutdownSystem(gecs::querier<LuaScript> scripts) {
    for (auto&& [entity, script] : scripts) {
        script.OnDestroy(entity);
    }
}

}  // namespace nickel