#pragma once

#include "nickel/refl/drefl/factory.hpp"

namespace nickel::refl {

class ClassVisitor {
public:
    virtual ~ClassVisitor() = default;

    virtual void operator()(NumericProperty&) = 0;
    virtual void operator()(EnumProperty&) = 0;
    virtual void operator()(ClassProperty&) = 0;
    virtual void operator()(StringProperty&) = 0;
    virtual void operator()(BooleanProperty&) = 0;
    virtual void operator()(PointerProperty&) = 0;
    virtual void operator()(ArrayProperty&) = 0;
    virtual void operator()(OptionalProperty&) = 0;
};

}  // namespace nickel::refl