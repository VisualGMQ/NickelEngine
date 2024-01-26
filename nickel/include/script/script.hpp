#pragma once

#include "core/asset.hpp"
#include "core/manager.hpp"
#include "pch.hpp"


namespace nickel {

class CppScript : public Asset {
public:
    using OnInitFunc = void (*)(gecs::entity);
    using OnDestroyFunc = void (*)(gecs::entity);
    using OnUpdateFunc = void (*)(gecs::entity);

    static constexpr std::string_view InitFuncName = "OnInit";
    static constexpr std::string_view DestroyFuncName = "OnDestroy";
    static constexpr std::string_view UpdateFuncName = "OnUpdate";

    CppScript() = default;
    CppScript(const CppScript&) = delete;
    CppScript& operator=(const CppScript&) = delete;

    CppScript(CppScript&& o) : Asset(std::move(o)) { swap(o, *this); }

    CppScript& operator=(CppScript&& o) {
        Asset::operator=(std::move(o));
        if (&o != this) {
            swap(o, *this);
        }
        return *this;
    }

    explicit CppScript(const std::filesystem::path& libname);
    ~CppScript();

    void Reload();

    void OnInit(gecs::entity);
    void OnUpdate(gecs::entity);
    void OnDestroy(gecs::entity);

    toml::table Save2Toml() const override;

private:
    OnInitFunc onInit_{};
    OnDestroyFunc onDestroy_{};
    OnUpdateFunc onUpdate_{};
    void* lib_{};

    void load(const std::filesystem::path& path);

    friend void swap(CppScript& o1, CppScript& o2) {
        using std::swap;

        swap(o1.onDestroy_, o2.onDestroy_);
        swap(o1.onInit_, o2.onInit_);
        swap(o1.onUpdate_, o2.onUpdate_);
        swap(o1.lib_, o2.lib_);
    }
};

using ScriptHandle = Handle<CppScript>;

class ScriptManager: public Manager<CppScript> {
public:
    ScriptHandle Load(const std::filesystem::path& path);

    auto GetFileType() const { return FileType::Script; }
};

}  // namespace nickel