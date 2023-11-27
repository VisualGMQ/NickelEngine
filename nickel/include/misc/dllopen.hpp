#pragma once

#include "core/log.hpp"
#include "core/log_tag.hpp"
#include "pch.hpp"

#ifdef _WIN32
#include <windows.h>
#else
#include <dlfcn.h>  // under ubuntu
#endif

namespace nickel {

struct DynamicLibrary {
public:
    DynamicLibrary(const std::string& filename) {
#ifdef _WIN32
        dll_ = LoadLibrary(filename.c_str());
#else
        dll_ = dlopen(filename.c_str(), RTLD_LAZY);
#endif
        if (dll_ == NULL) {
            LOGE(log_tag::Nickel, "open dynamic library ", filename, " failed");
        }
    }

    template <typename FuncType>
    FuncType* GetProcAddress(const std::string& name) {
        static_assert(std::is_function_v<FuncType>);
#ifdef _WIN32
        return (FuncType*)(::GetProcAddress(dll_, name.c_str()));
#else
        return (FuncType*)(dlsym(dll_, name.c_str()));
#endif
    }

    ~DynamicLibrary() {
#ifdef _WIN32
        FreeLibrary(dll_);
#else
        dlclose(dll_);
#endif
    }

    operator bool() const { return dll_ != NULL; }

private:
#ifdef _WIN32
    HMODULE dll_;
#else
    void* dll_;
#endif
};

}  // namespace nickel