#pragma once
#include "nickel/common/singleton.hpp"

// forward declare
struct JSRuntime;

namespace nickel::script {

class QJSContext;

class QJSRuntime : public Singlton<QJSRuntime, false> {
public:
    QJSRuntime();
    ~QJSRuntime();

    operator JSRuntime*() const;

    const QJSContext& GetContext() const;
    QJSContext& GetContext();

private:
    JSRuntime* m_runtime{};
    std::unique_ptr<QJSContext> m_context;
};

}