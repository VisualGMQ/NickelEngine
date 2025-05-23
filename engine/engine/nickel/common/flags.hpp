#pragma once
#include <concepts>

namespace nickel {

template <typename T>
requires std::is_enum_v<T>
class Flags {
public:
    using enum_type = T;
    using underlying_type = std::underlying_type_t<T>;

    Flags() = default;

    Flags(T value) : m_value{static_cast<underlying_type>(value)} {}

    Flags(std::underlying_type_t<T> value) : m_value{value} {}

    Flags(const Flags&) = default;
    Flags(Flags&&) = default;

    Flags operator|(T o) const {
        return Flags{m_value | static_cast<underlying_type>(o)};
    }

    Flags operator&(T o) const {
        return Flags{m_value & static_cast<underlying_type>(o)};
    }

    Flags& operator=(const Flags&) = default;

    Flags& operator=(T value) {
        m_value = static_cast<underlying_type>(value);
        return *this;
    }

    Flags& operator|=(T o) {
        m_value |= static_cast<underlying_type>(o);
        return *this;
    }

    Flags& operator&=(T o) {
        m_value &= static_cast<underlying_type>(o);
        return *this;
    }

    Flags operator~() const noexcept { return ~m_value; }

    operator T() const { return static_cast<T>(m_value); }

    operator underlying_type() const { return m_value; }

private:
    underlying_type m_value{};
};

}  // namespace tl
