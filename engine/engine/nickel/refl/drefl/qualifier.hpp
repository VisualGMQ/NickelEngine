#pragma once

namespace nickel::refl {

enum class Qualifier {
    None = 0x00,
    Const = 0x01,   // const vairble: const int, const int&, int* const
    Ref = 0x02,
};


}