#pragma once
#include "nickel/common/memory/memory.hpp"
#include "nickel/script/internal/qjs_script_impl.hpp"
#include "nickel/script/qjs_script.hpp"

namespace nickel::script {

class ScriptManagerImpl {
public:
    ScriptManagerImpl();
    QuickJSScript Load(const Path&);
    QuickJSScript Load(std::span<const char> content);
    
private:
    BlockMemoryAllocator<QuickJSScriptImpl> m_allocator;

    void bindingCpp();
    void bindingConsoleFn();
};

}