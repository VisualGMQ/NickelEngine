#pragma once
#include "nickel/common/singleton.hpp"

// forward declare
struct JSRuntime;

namespace nickel::script {

class QJSClassFactory;

class QJSContext;

// TODO: remove singlton
class QJSRuntime : public Singlton<QJSRuntime, false> {
public:
    QJSRuntime();
    ~QJSRuntime();

    operator JSRuntime*() const;

    const QJSContext& GetContext() const;
    QJSContext& GetContext();
    QJSClassFactory& GetClassFactory();
    void DoRegister();

private:
    JSRuntime* m_runtime{};
    std::unique_ptr<QJSClassFactory> m_class_factory;
    std::unique_ptr<QJSContext> m_context;
};

}  // namespace nickel::script