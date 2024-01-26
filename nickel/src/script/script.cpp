#include "script/script.hpp"

namespace nickel {

CppScript::CppScript(const std::filesystem::path& libname) : Asset(libname) {
    load(libname);
}

CppScript::~CppScript() {
    SDL_UnloadObject(lib_);
}

void CppScript::load(const std::filesystem::path& path) {
    lib_ = SDL_LoadObject(path.string().c_str());
    if (!lib_) {
        LOGE(nickel::log_tag::Script, "load cpp lib ", path, " failed");
        return;
    }

    onInit_ = (OnInitFunc)SDL_LoadFunction(lib_, InitFuncName.data());
    onDestroy_ = (OnDestroyFunc)SDL_LoadFunction(lib_, DestroyFuncName.data());
    onUpdate_ = (OnUpdateFunc)SDL_LoadFunction(lib_, UpdateFuncName.data());
}

void CppScript::OnInit(gecs::entity e) {
    if (onInit_) {
        onInit_(e);
    }
}

void CppScript::OnUpdate(gecs::entity e) {
    if (onUpdate_) {
        onUpdate_(e);
    }
}

void CppScript::OnDestroy(gecs::entity e) {
    if (onDestroy_) {
        onDestroy_(e);
    }
}

toml::table CppScript::Save2Toml() const {
    toml::table tbl;
    tbl.emplace("path", RelativePath());
    return tbl;
}

ScriptHandle ScriptManager::Load(const std::filesystem::path& path) {
    auto data = std::make_unique<CppScript>(path);
    if (!data) {
        auto handle = ScriptHandle::Create();
        storeNewItem(handle, std::move(data));
        return handle;
    }
    return ScriptHandle::Null();
}

}  // namespace nickel