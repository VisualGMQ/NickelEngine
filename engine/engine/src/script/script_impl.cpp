#include "nickel/script/internal/script_impl.hpp"
#include "nickel/context.hpp"
#include "nickel/script/binding/common.hpp"
#include "nickel/script/binding/runtime.hpp"

namespace nickel::script {

ScriptManagerImpl::ScriptManagerImpl() {
    bindingCpp();
}

void ScriptManagerImpl::Eval(std::string_view code) {
    JS_Eval(QJSRuntime::GetInst().GetContext(), code.data(), code.size(),
            nullptr, JS_EVAL_FLAG_STRICT | JS_EVAL_TYPE_GLOBAL);
}

void ScriptManagerImpl::EvalBinary(std::span<uint8_t> code) {
    JSContext* ctx = QJSRuntime::GetInst().GetContext();
    JSValue obj =
        JS_ReadObject(ctx, code.data(), code.size(), JS_READ_OBJ_BYTECODE);

    if (JS_IsException(obj)) LogJSException(ctx);

    JSValue result = JS_EvalFunction(ctx, obj);
    if (JS_IsException(result)) LogJSException(ctx);

    JS_FreeValue(ctx, obj);
    JS_FreeValue(ctx, result);
}

QuickJSScript ScriptManagerImpl::Load(const Path& filename) {
    auto content =
        Context::GetInst().GetStorageManager().GetUserStorage().ReadStorageFile(
            filename);
    auto& ctx = QJSRuntime::GetInst().GetContext();
    // FIXME: remove new
    return QuickJSScript{
        new QuickJSScriptImpl(ctx, ctx.Eval(content, filename, true))};
}

QuickJSScript ScriptManagerImpl::Load(std::span<const char> content) {
    auto& ctx = QJSRuntime::GetInst().GetContext();
    // FIXME: remove new
    return QuickJSScript{
        new QuickJSScriptImpl{ctx, ctx.Eval(content, "", true)}
    };
}

JSValue jsPrint2Console(JSContext* context, JSValue self, int argc,
                        JSValue* argv) {
    std::string text;
    size_t len;

    for (int i = 0; i < argc; i++) {
        if (i != 0) fputc(' ', stdout);
        const char* str = JS_ToCStringLen(context, &len, argv[i]);
        if (!str) return JS_EXCEPTION;
        text += str;
        JS_FreeCString(context, str);
    }
    LOGI("[QuickJS]: {}", text);
    return JS_UNDEFINED;
}

void ScriptManagerImpl::bindingCpp() {
    bindingConsoleFn();

    // TODO: bind other cpp
}

void ScriptManagerImpl::bindingConsoleFn() {
    auto& ctx = QJSRuntime::GetInst().GetContext();
    JSValue global_obj = JS_GetGlobalObject(ctx);

    JSValue console_obj = JS_NewObject(ctx);
    JS_SetPropertyStr(ctx, console_obj, "log",
                      JS_NewCFunction(ctx, jsPrint2Console, "log", 1));
    JS_SetPropertyStr(ctx, global_obj, "console", console_obj);

    JS_FreeValue(ctx, global_obj);
}

ScriptManager::ScriptManager()
    : m_impl{std::make_unique<ScriptManagerImpl>()} {}

ScriptManager::~ScriptManager() {}

void ScriptManager::Eval(std::string_view code) {
    m_impl->Eval(code);
}

void ScriptManager::EvalBinary(std::span<uint8_t> code) {
    m_impl->EvalBinary(code);
}

QuickJSScript ScriptManager::Load(const Path& filename) {
    return m_impl->Load(filename);
}

QuickJSScript ScriptManager::Load(std::span<const char> content) {
    return m_impl->Load(content);
}

ScriptManagerImpl* ScriptManager::GetImpl() {
    return m_impl.get();
}

}  // namespace nickel::script