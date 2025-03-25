#pragma once

namespace nickel {

consteval uint32_t GenFullBitMast(uint8_t len) {
    uint32_t bit = 0;
    while (len > 0) {
        bit |= 1 << (len - 1);
        len --;
    }
    return bit;
}

}