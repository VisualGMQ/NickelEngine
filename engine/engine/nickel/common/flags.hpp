#pragma once
#include <concepts>

namespace nickel {

template <typename T>
requires std::is_enum_v<T>
class Flags {
public:
    using underlying_type = std::underlying_type_t<T>;

    Flags() = default;

    Flags(T value) : value_{static_cast<underlying_type>(value)} {}

    Flags(std::underlying_type_t<T> value) : value_{value} {}

    Flags(const Flags&) = default;
    Flags(Flags&&) = default;

    Flags operator|(T o) const {
        return Flags{value_ | static_cast<underlying_type>(o)};
    }

    Flags operator&(T o) const {
        return Flags{value_ & static_cast<underlying_type>(o)};
    }

    Flags& operator=(const Flags&) = default;

    Flags& operator=(T value) {
        value_ = static_cast<underlying_type>(value);
        return *this;
    }

    Flags& operator|=(T o) {
        value_ |= static_cast<underlying_type>(o);
        return *this;
    }

    Flags& operator&=(T o) {
        value_ &= static_cast<underlying_type>(o);
        return *this;
    }

    Flags operator~() const noexcept { return ~value_; }

    operator T() const { return static_cast<T>(value_); }

    operator underlying_type() const { return value_; }

private:
    underlying_type value_{};
};

}  // namespace tl
