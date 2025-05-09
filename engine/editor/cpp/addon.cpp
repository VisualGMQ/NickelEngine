#include <napi.h>
#include "nickel/nickel.hpp"
#include <Windows.h>

class HelloAddon : public Napi::Addon<HelloAddon> {
public:
    HelloAddon(Napi::Env env, Napi::Object exports) {
        DefineAddon(exports, {InstanceMethod("hello", &HelloAddon::Hello,
                                             napi_enumerable)});
        DefineAddon(exports, {InstanceMethod("initEngine", &HelloAddon::InitEngine,
                                             napi_enumerable)});
        DefineAddon(exports, {InstanceMethod("shutdownEngine", &HelloAddon::ShutdownEngine,
                                             napi_enumerable)});
        DefineAddon(exports, {InstanceMethod("updateEngine", &HelloAddon::UpdateEngine,
                                             napi_enumerable)});
    }

private:
    Napi::Value Hello(const Napi::CallbackInfo& info) {
        return Napi::String::New(info.Env(), "world");
    }

    Napi::Value InitEngine(const Napi::CallbackInfo& info) {
        auto env = info.Env();
        if (!info[0].IsBuffer()) {
            Napi::TypeError::New(env, "invalid window handle")
                .ThrowAsJavaScriptException();
            return env.Null();
        }
        auto buffer = info[0].As<Napi::Buffer<void*>>();
        HWND win = static_cast<HWND>(*static_cast<void **>(buffer.Data()));
        nickel::Context::Init();
        nickel::Context::GetInst().Initialize(win);
        return env.Null();
    }
    
    Napi::Value UpdateEngine(const Napi::CallbackInfo& info) {
        nickel::Context::GetInst().Update();
        return info.Env().Null();
    }

    Napi::Value ShutdownEngine(const Napi::CallbackInfo& info) {
        nickel::Context::Delete();
        return info.Env().Null();
    }
};

NODE_API_ADDON(HelloAddon)
