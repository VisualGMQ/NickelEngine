#include "nickel/script/internal/script_impl.hpp"
#include "nickel/context.hpp"
#include "nickel/script/binding/common.hpp"
#include "nickel/script/binding/runtime.hpp"
#include "nickel/generate/binding/script_binding.hpp"

namespace nickel::script {

ScriptManagerImpl::ScriptManagerImpl() {
    script_binding::RegisterQJSScript(m_runtime);
}

void ScriptManagerImpl::Eval(std::string_view code) {
    JS_Eval(m_runtime.GetContext(), code.data(), code.size(), nullptr,
            JS_EVAL_FLAG_STRICT | JS_EVAL_TYPE_GLOBAL);
}

void ScriptManagerImpl::EvalBinary(std::span<uint8_t> code) {
    JSContext* ctx = m_runtime.GetContext();
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
    auto& ctx = m_runtime.GetContext();
    // FIXME: remove new
    return QuickJSScript{
        new QuickJSScriptImpl(ctx, ctx.Eval(content, filename, true))};
}

QuickJSScript ScriptManagerImpl::Load(std::span<const char> content) {
    auto& ctx = m_runtime.GetContext();
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