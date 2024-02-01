#include "common/assert.hpp"
#include "debugbreak.hpp"

namespace nickel {

void doAssert(bool b, std::string_view msg) {
    if (!b) {
        LOGF("Assert", msg);
        debug_break();
    }
}

}  // namespace nickel