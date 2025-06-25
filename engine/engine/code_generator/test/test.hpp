#pragma once
#include <string>
#include "macro.hpp"

namespace test_space {

struct NICKEL_REFL_ATTR(refl, script) Person {
    std::string m_name;
    NICKEL_REFL_ATTR(norefl) float m_height;
    const int m_age;

    void Foo();

    enum MyEnum {
        Value1, Value2, Value3,
    };
};

}