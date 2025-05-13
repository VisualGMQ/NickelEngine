#pragma once
#include <string>

#ifdef _NICKEL_REFLECTION_
#define NICKEL_REFL_ATTR(...)  __attribute__((annotate(#__VA_ARGS__)))
#else
#define NICKEL_REFL_ATTR(...)
#endif

namespace test_space {

class NICKEL_REFL_ATTR(nickel::refl) Person {
    std::string m_name;
    float m_height;
    const int m_age;

    void Foo();
};

}