#pragma once

#include "nickel/common/math/constants.hpp"
#include <concepts>

namespace nickel {

template <typename T>
requires(std::is_floating_point_v<T>)
class TDegrees;

template <typename T>
requires(std::is_floating_point_v<T>)
class TRadians {
public:
    TRadians() : value_{} {}
    
    template <typename U>
    requires(std::convertible_to<T, U>)
    constexpr TRadians(U value) : value_{static_cast<T>(value)} {}

    template <typename U>
    requires(std::convertible_to<U, T>)
    constexpr TRadians(TDegrees<U> o)
        : value_{static_cast<T>(static_cast<T>(o) * GenericPI<T> /
                                static_cast<T>(180.0))} {}

    constexpr TRadians& operator+=(const TRadians& o) {
        value_ += o.value_;
        return *this;
    }

    constexpr TRadians& operator-=(const TRadians& o) {
        value_ -= o.value_;
        return *this;
    }

    template <typename U>
    constexpr TRadians& operator*=(U value) {
        value_ *= value;
        return *this;
    }

    template <typename U>
    constexpr TRadians& operator/=(U value) {
        value_ /= value;
        return *this;
    }

    template <typename U>
    constexpr TRadians& operator*=(TRadians<U> value) {
        value_ *= value.value_;
        return *this;
    }

    template <typename U>
    constexpr TRadians& operator/=(TRadians<U> value) {
        value_ /= value.value_;
        return *this;
    }

    constexpr TRadians(const TRadians&) = default;

    constexpr explicit operator T() const noexcept { return value_; }

    constexpr T Value() const noexcept { return value_; }

private:
    T value_;
};

template <typename T>
requires(std::is_floating_point_v<T>)
class TDegrees {
public:
    constexpr TDegrees() : value_{} {}

    template <typename U>
    requires(std::convertible_to<T, U>)
    constexpr TDegrees(U value) : value_{static_cast<T>(value)} {}

    template <typename U>
    requires(std::convertible_to<U, T>)
    constexpr TDegrees(TRadians<U> o)
        : value_{static_cast<T>(static_cast<T>(o) * static_cast<T>(180.0) /
                                GenericPI<T>)} {}

    constexpr TDegrees& operator+=(const TDegrees& o) {
        value_ += o.value_;
        return *this;
    }

    constexpr TDegrees& operator-=(const TDegrees& o) {
        value_ -= o.value_;
        return *this;
    }

    template <typename U>
    constexpr TDegrees& operator*=(U value) {
        value_ *= value;
        return *this;
    }

    template <typename U>
    constexpr TDegrees& operator/=(U value) {
        value_ /= value;
        return *this;
    }

    template <typename U>
    constexpr TDegrees& operator*=(TDegrees<U> value) {
        value_ *= value.value_;
        return *this;
    }

    template <typename U>
    constexpr TDegrees& operator/=(TDegrees<U> value) {
        value_ /= value.value_;
        return *this;
    }

    constexpr TDegrees(const TDegrees&) = default;

    constexpr explicit operator T() const noexcept { return value_; }

    constexpr T Value() const noexcept { return value_; }

private:
    T value_;
};

// mathematics

template <typename T>
TRadians<T> operator+(TRadians<T> r1, TRadians<T> r2) {
    return TRadians<T>(static_cast<T>(r1) + static_cast<T>(r2));
}

template <typename T>
TRadians<T> operator-(TRadians<T> r1, TRadians<T> r2) {
    return TRadians<T>(static_cast<T>(r1) - static_cast<T>(r2));
}

template <typename T>
TRadians<T> operator-(TRadians<T> r) {
    return TRadians<T>(-static_cast<T>(r));
}

template <typename T, typename U>
TRadians<T> operator*(TRadians<T> r1, U value) {
    return TRadians<T>(r1.Value() * value);
}

template <typename T, typename U>
TRadians<T> operator*(U value, TRadians<T> r1) {
    return r1 * value;
}

template <typename T, typename U>
TRadians<T> operator/(TRadians<T> r1, U value) {
    return TRadians<T>(r1.Value() / value);
}

template <typename T>
constexpr TDegrees<T> operator+(TDegrees<T> r1, TDegrees<T> r2) {
    return TDegrees<T>(static_cast<T>(r1) + static_cast<T>(r2));
}

template <typename T>
constexpr TDegrees<T> operator-(TDegrees<T> r1, TDegrees<T> r2) {
    return TDegrees<T>(static_cast<T>(r1) - static_cast<T>(r2));
}

template <typename T>
constexpr TDegrees<T> operator-(TDegrees<T> r) {
    return TDegrees<T>(-static_cast<T>(r));
}

template <typename T, typename U>
constexpr TDegrees<T> operator*(TDegrees<T> r1, U value) {
    return TDegrees<T>(r1.Value() * value);
}

template <typename T, typename U>
constexpr TDegrees<T> operator*(U value, TDegrees<T> r1) {
    return r1 * value;
}

template <typename T, typename U>
constexpr TDegrees<T> operator/(TDegrees<T> r1, U value) {
    return TDegrees<T>(r1.Value() / value);
}

// common type comparison

template <typename T, typename U>
bool operator==(TRadians<T> r1, TRadians<U> r2) {
    return static_cast<T>(r1) == static_cast<U>(r2);
}

template <typename T, typename U>
bool operator!=(TRadians<T> r1, TRadians<U> r2) {
    return !(r1 == r2);
}

template <typename T, typename U>
bool operator>(TRadians<T> r1, TRadians<U> r2) {
    return static_cast<T>(r1) > static_cast<U>(r2);
}

template <typename T, typename U>
bool operator<(TRadians<T> r1, TRadians<U> r2) {
    return static_cast<T>(r1) < static_cast<U>(r2);
}

template <typename T, typename U>
bool operator>=(TRadians<T> r1, TRadians<U> r2) {
    return static_cast<T>(r1) >= static_cast<U>(r2);
}

template <typename T, typename U>
bool operator<=(TRadians<T> r1, TRadians<U> r2) {
    return static_cast<T>(r1) <= static_cast<U>(r2);
}

template <typename T, typename U>
constexpr bool operator==(TDegrees<T> r1, TDegrees<U> r2) {
    return static_cast<T>(r1) == static_cast<U>(r2);
}

template <typename T, typename U>
constexpr bool operator!=(TDegrees<T> r1, TDegrees<U> r2) {
    return !(r1 == r2);
}

template <typename T, typename U>
constexpr bool operator>(TDegrees<T> r1, TDegrees<U> r2) {
    return static_cast<T>(r1) > static_cast<U>(r2);
}

template <typename T, typename U>
constexpr bool operator<(TDegrees<T> r1, TDegrees<U> r2) {
    return static_cast<T>(r1) < static_cast<U>(r2);
}

template <typename T, typename U>
constexpr bool operator>=(TDegrees<T> r1, TDegrees<U> r2) {
    return static_cast<T>(r1) >= static_cast<U>(r2);
}

template <typename T, typename U>
constexpr bool operator<=(TDegrees<T> r1, TDegrees<U> r2) {
    return static_cast<T>(r1) <= static_cast<U>(r2);
}

// radians degree comparision

template <typename T, typename U>
bool operator==(TRadians<T> rad, TDegrees<U> deg) {
    return TDegrees<T>(rad) == deg;
}

template <typename T, typename U>
bool operator!=(TRadians<T> rad, TDegrees<U> deg) {
    return !(rad == deg);
}

template <typename T, typename U>
bool operator>(TRadians<T> rad, TDegrees<U> deg) {
    return TDegrees<T>(rad) > deg;
}

template <typename T, typename U>
bool operator<(TRadians<T> rad, TDegrees<U> deg) {
    return TDegrees<T>(rad) < deg;
}

template <typename T, typename U>
bool operator>=(TRadians<T> rad, TDegrees<U> deg) {
    return TDegrees<T>(rad) >= deg;
}

template <typename T, typename U>
bool operator<=(TRadians<T> rad, TDegrees<U> deg) {
    return TDegrees<T>(rad) <= deg;
}

template <typename T, typename U>
bool operator==(TDegrees<U> deg, TRadians<T> rad) {
    return rad == deg;
}

template <typename T, typename U>
bool operator!=(TDegrees<U> deg, TRadians<T> rad) {
    return rad != deg;
}

template <typename T, typename U>
bool operator>(TDegrees<U> deg, TRadians<T> rad) {
    return rad < deg;
}

template <typename T, typename U>
bool operator<(TDegrees<U> deg, TRadians<T> rad) {
    return rad > deg;
}

template <typename T, typename U>
bool operator>=(TDegrees<U> deg, TRadians<T> rad) {
    return rad <= deg;
}

template <typename T, typename U>
bool operator<=(TDegrees<U> deg, TRadians<T> rad) {
    return rad >= deg;
}

}  // namespace nickel
