#include "script/script.hpp"
#include "common/util.hpp"

#include "lua.h"
#include "luacode.h"
#include "lualib.h"

#include "LuaBridge/LuaBridge.h"

namespace nickel {

LuaScript LuaScript::Null;

void bindEngine2Lua(lua_State* L);

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
    bindEngine2Lua(state_);
}

LuaScript::~LuaScript() {
    if (state_) {
        lua_close(state_);
    }
}

void LuaScript::OnInit(gecs::entity) const {
    if (!state_) {
        return;
    }

    if (auto ref = luabridge::getGlobal(state_, "on_init"); ref) {
        ref();
    }
}

void LuaScript::OnUpdate(gecs::entity) const {
    if (!state_) {
        return;
    }
    if (auto ref = luabridge::getGlobal(state_, "on_update"); ref) {
        ref();
    }
}

void LuaScript::OnDestroy(gecs::entity) const {
    if (!state_) {
        return;
    }
    if (auto ref = luabridge::getGlobal(state_, "on_quit"); ref) {
        ref();
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
    if (!data) {
        auto handle = ScriptHandle::Create();
        storeNewItem(handle, std::move(data));
        return handle;
    }
    return ScriptHandle::Null();
}

void bindEngine2Lua(lua_State* L) {
    luabridge::getGlobalNamespace(L)
        .beginNamespace("nickel")
        .addFunction("hello_nickel",
                     []() { std::cout << "hello nickel" << std::endl; })
        .endNamespace();
}

void ScriptUpdateSystem(gecs::querier<gecs::mut<LuaScript>> scripts) {
    for (auto&& [entity, script] : scripts) {
        if (!script.isInited_) {
            script.OnInit(entity);
            script.isInited_ = true;
        }
        script.OnUpdate(entity);
    }
}

void ScriptShutdownSystem(gecs::querier<LuaScript> scripts) {
    for (auto&& [entity, script] : scripts) {
        script.OnDestroy(entity);
    }
}

}  // namespace nickel