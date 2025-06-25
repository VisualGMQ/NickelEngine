#pragma once
#include "nickel/common/memory/memory.hpp"
#include "nickel/script/binding/runtime.hpp"
#include "nickel/script/internal/qjs_script_impl.hpp"
#include "nickel/script/qjs_script.hpp"

namespace nickel::script {

class ScriptManagerImpl {
public:
    ScriptManagerImpl();
    void Eval(std::string_view code);
    void EvalBinary(std::span<uint8_t> code);

    QuickJSScript Load(const Path&);
    QuickJSScript Load(std::span<const char> content);

    void GC();
    
    BlockMemoryAllocator<QuickJSScriptImpl> m_allocator;

private:
    QJSRuntime m_runtime;
};

}  // namespace nickel::script