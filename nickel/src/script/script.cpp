#include "script/script.hpp"
#include "script/luabind.hpp"
#include "common/util.hpp"

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

LuaScript::LuaScript(const std::filesystem::path& libname) {
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

bool LuaScript::Load(const toml::table& tbl) {
    if (auto node = tbl.get("path"); node && node->is_string()) {
        auto& filename = node->as_string()->get();
        return load(filename);
    } else {
        LOGE(log_tag::Asset, "load luau script failed: no asset path");
        return false;
    }
}

bool LuaScript::load(const std::filesystem::path& filename) {
    auto codes = ReadWholeFile<std::string>(filename);
    size_t bytecodeSize = 0;
    lua_State* newState{};
    if (codes) {
        char* bytecode =
            luau_compile(codes->c_str(), codes->size(), NULL, &bytecodeSize);
        int result = luau_load(newState, filename.string().c_str(), bytecode,
                               bytecodeSize, 0);
        free(bytecode);

        if (result != LUA_OK) {
            LOGE(log_tag::Script, "load script ", filename, "failed");
            return false;
        } else {
            lua_pcall(newState, 0, 0, 0);
            lua_close(state_);
            state_ = newState;
            isInited_ = false;
            return true;
        }
    } else {
        LOGE(log_tag::Script, "load script failed: read file ", filename,
             " failed");
        return false;
    }
}

bool LuaScript::Save(toml::table& tbl) const {
    tbl.emplace("path", GetRelativePath().string());
}

void ScriptUpdateSystem(gecs::querier<gecs::mut<Script>> scripts,
                        gecs::resource<gecs::mut<ScriptManager>> mgr) {
    for (auto&& [entity, script] : scripts) {
        if (script.handle) {
            continue;
        }

        auto& luaScript = *script.handle.GetData();
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