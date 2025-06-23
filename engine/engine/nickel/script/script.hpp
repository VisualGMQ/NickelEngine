#pragma once
#include "nickel/script/qjs_script.hpp"

namespace nickel::script {

class ScriptManagerImpl;

class ScriptManager {
public:
    ScriptManager();
    ~ScriptManager();

    void Eval(std::string_view code);
    void EvalBinary(std::span<uint8_t> code);
    QuickJSScript Load(const Path&);
    QuickJSScript Load(std::span<const char> content);

    ScriptManagerImpl* GetImpl();

private:
    std::unique_ptr<ScriptManagerImpl> m_impl;
};

}  // namespace nickel