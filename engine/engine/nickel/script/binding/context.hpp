#pragma once
#include "nickel/fs/path.hpp"

// quickjs forward declare
struct JSValue;
struct JSContext;

namespace nickel::script {

class QJSRuntime;
class QJSModule;

class QJSContext {
public:
    explicit QJSContext(QJSRuntime& runtime);
    ~QJSContext();

    JSValue Eval(std::span<const char> content, const Path& filename,
                 bool strict_mode) const;

    QJSModule& NewModule(const std::string& name);

    auto& GetModules() const { return m_modules; }

    QJSRuntime& GetRuntime() const;

    operator JSContext*() const;
    operator bool() const;

private:
    JSContext* m_context{};
    QJSRuntime& m_runtime;

    std::vector<std::unique_ptr<QJSModule>> m_modules;
};

}