#pragma once

#include "nickel/refl/drefl/type.hpp"

namespace nickel::refl {

class Any;

class Boolean final: public Type {
public:
    Boolean();

    void SetValue(Any&, bool) const;
    bool GetValue(const Any&) const;
};

}