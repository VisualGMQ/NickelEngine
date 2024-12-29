#include "nickel/common/math/math.hpp"

namespace nickel {

Radians operator ""_rad(long double value) {
    return Radians{value};
}

Degrees operator "" _deg(long double value) {
    return Degrees{value};
}

Radians operator "" _rad(unsigned long long value) {
    return Radians{value};
}

Degrees operator "" _deg(unsigned long long value) {
    return Degrees{value};
}

}
