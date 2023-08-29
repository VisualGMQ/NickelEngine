#include "input/input.hpp"

namespace nickel {

Keyboard::Keyboard() {
    for (int i = 0; i < static_cast<int>(Key::KEY_LAST); i++) {
        buttons_[i].btn = static_cast<::nickel::Key>(i);
    }
}

}