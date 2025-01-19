#pragma once

#include "nickel/common/math/constants.hpp"

namespace nickel {

template <typename T>
requires(std::is_floating_point_v<T>)
class TDegrees;

template <typename T>
requires(std::is_floating_point_v<T>)
class TRadians {
public:
    template <typename U>
    requires(std::convertible_to<T, U>)
    TRadians(U value) : value_{static_cast<T>(value)} {}

    template <typename U>
    requires(std::convertible_to<U, T>)
    TRadians(TDegrees<U> o)
        : value_{static_cast<T>(static_cast<T>(o) * GenericPI<T> /
                                static_cast<T>(180.0))} {}

    TRadians& operator+=(const TRadians& o) {
        value_ += o.value_;
        return *this;
    }

    TRadians& operator-=(const TRadians& o) {
        value_ -= o.value_;
        return *this;
    }

    template <typename U>
    TRadians& operator*=(U value) {
        value_ *= value;
        return *this;
    }

    template <typename U>
    TRadians& operator/=(U value) {
        value_ /= value;
        return *this;
    }

    template <typename U>
    TRadians& operator*=(TRadians<U> value) {
        value_ *= value.value_;
        return *this;
    }

    template <typename U>
    TRadians& operator/=(TRadians<U> value) {
        value_ /= value.value_;
        return *this;
    }

    TRadians(const TRadians&) = default;

    explicit operator T() const noexcept { return value_; }

    T Value() const noexcept { return value_; }

private:
    T value_;
};

template <typename T>
requires(std::is_floating_point_v<T>)
class TDegrees {
public:
    template <typename U>
    requires(std::convertible_to<T, U>)
    TDegrees(U value) : value_{static_cast<T>(value)} {}

    template <typename U>
    requires(std::convertible_to<U, T>)
    TDegrees(TRadians<U> o)
        : value_{static_cast<T>(static_cast<T>(o) * static_cast<T>(180.0) /
                                GenericPI<T>)} {}

    TDegrees& operator+=(const TDegrees& o) {
        value_ += o.value_;
        return *this;
    }

    TDegrees& operator-=(const TDegrees& o) {
        value_ -= o.value_;
        return *this;
    }

    template <typename U>
    TDegrees& operator*=(U value) {
        value_ *= value;
        return *this;
    }

    template <typename U>
    TDegrees& operator/=(U value) {
        value_ /= value;
        return *this;
    }

    template <typename U>
    TDegrees& operator*=(TDegrees<U> value) {
        value_ *= value.value_;
        return *this;
    }

    template <typename U>
    TDegrees& operator/=(TDegrees<U> value) {
        value_ /= value.value_;
        return *this;
    }

    TDegrees(const TDegrees&) = default;

    explicit operator T() const noexcept { return value_; }

    T Value() const noexcept { return value_; }

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

template <typename T>
TRadians<T> operator*(TRadians<T> r1, T value) {
    return TRadians<T>(static_cast<T>(r1) * value);
}

template <typename T>
TRadians<T> operator*(T value, TRadians<T> r1) {
    return r1 * value;
}

template <typename T>
TRadians<T> operator/(TRadians<T> r1, T value) {
    return TRadians<T>(static_cast<T>(r1) / value);
}

template <typename T>
TDegrees<T> operator+(TDegrees<T> r1, TDegrees<T> r2) {
    return TDegrees<T>(static_cast<T>(r1) + static_cast<T>(r2));
}

template <typename T>
TDegrees<T> operator-(TDegrees<T> r1, TDegrees<T> r2) {
    return TDegrees<T>(static_cast<T>(r1) - static_cast<T>(r2));
}

template <typename T>
TDegrees<T> operator-(TDegrees<T> r) {
    return TDegrees<T>(-static_cast<T>(r));
}

template <typename T>
TDegrees<T> operator*(TDegrees<T> r1, T value) {
    return TDegrees<T>(static_cast<T>(r1) * value);
}

template <typename T>
TDegrees<T> operator*(T value, TDegrees<T> r1) {
    return r1 * value;
}

template <typename T>
TDegrees<T> operator/(TDegrees<T> r1, T value) {
    return TDegrees<T>(static_cast<T>(r1) / value);
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
bool operator==(TDegrees<T> r1, TDegrees<U> r2) {
    return static_cast<T>(r1) == static_cast<U>(r2);
}

template <typename T, typename U>
bool operator!=(TDegrees<T> r1, TDegrees<U> r2) {
    return !(r1 == r2);
}

template <typename T, typename U>
bool operator>(TDegrees<T> r1, TDegrees<U> r2) {
    return static_cast<T>(r1) > static_cast<U>(r2);
}

template <typename T, typename U>
bool operator<(TDegrees<T> r1, TDegrees<U> r2) {
    return static_cast<T>(r1) < static_cast<U>(r2);
}

template <typename T, typename U>
bool operator>=(TDegrees<T> r1, TDegrees<U> r2) {
    return static_cast<T>(r1) >= static_cast<U>(r2);
}

template <typename T, typename U>
bool operator<=(TDegrees<T> r1, TDegrees<U> r2) {
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
