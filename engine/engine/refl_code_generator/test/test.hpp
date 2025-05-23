#pragma once
#include <string>
#include "macro.hpp"

namespace test_space {

class NICKEL_REFL_ATTR(refl) Person {
    std::string m_name;
    NICKEL_REFL_ATTR(norefl) float m_height;
    const int m_age;

    void Foo();
};

}