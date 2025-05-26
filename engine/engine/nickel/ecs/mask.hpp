#pragma once
#include <concepts>

namespace nickel::ecs {

template <typename T>
requires std::is_integral_v<T> && std::is_unsigned_v<T>
struct Mask {
    uint8_t count;
    uint8_t offset;
    T mask;

    template <typename U>
    constexpr T operator&(U value) const {
        return value & mask;
    }

    constexpr T GenerateByValue(T value) const { return (value << offset) & mask; }
};

template <typename T, typename U>
constexpr T operator&(U value, Mask<T> mask) {
    return mask & value;
}

template <typename T>
requires std::is_integral_v<T> && std::is_unsigned_v<T>
consteval Mask<T> GenerateMask(uint32_t count, uint32_t offset) {
    Mask<T> mask;
    mask.mask = 0;

    while (count--) {
        mask.mask |= 1 << count;
    }
    mask.mask <<= offset;
    mask.count = count;
    mask.offset = offset;

    return mask;
}

}  // namespace nickel::ecs