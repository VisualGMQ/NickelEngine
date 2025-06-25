#pragma once
#include "quickjs.h"

namespace nickel::script {

class QJSClassBase {
public:
    QJSClassBase() = default;
    QJSClassBase(const QJSClassBase&) = delete;
    QJSClassBase& operator=(const QJSClassBase&) = delete;
    virtual ~QJSClassBase() = default;

    virtual JSValue GetConstructor() const = 0;
    virtual const std::string& GetName() const = 0;
    virtual const std::string& GetConstName() const = 0;
    virtual const std::string& GetRefName() const = 0;
    virtual const std::string& GetConstRefName() const = 0;
    virtual const std::string& GetPointerName() const = 0;
    virtual const std::string& GetConstPointerName() const = 0;
};

}