#pragma once
#include <string>
#include "macro.hpp"

namespace test_space {

struct NICKEL_REFL_ATTR(refl, script) Person {
    Person(const std::string& name, float height, int age): m_name{name}, m_height{height}, m_age{age} {}

    std::string m_name;
    NICKEL_REFL_ATTR(norefl) float m_height;
    const int m_age;

    void Foo();

    enum MyEnum {
        Value1, Value2, Value3,
    };
};

enum NICKEL_REFL_ATTR(refl, script) GEnum {
    Value1, Value2,
};

}