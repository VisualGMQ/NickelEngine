#pragma once

#include "nickel/refl/drefl/type.hpp"
#include "nickel/refl/drefl/value_kind.hpp"
#include "nickel/refl/drefl/any.hpp"
#include <cstdint>

namespace nickel::refl {

class Numeric: public Type {
public:
    template <typename>
    friend class NumericFactory;

    enum class NumericKind {
        Unknown,
        Char,
        Int,
        Short,
        LongLong,
        Uint8,
        Uint16,
        Uint32,
        Uint64,
        Float,
        Double,
    };

    auto NumericKind() const noexcept { return m_kind; }

    Numeric(ValueKind value_kind, enum NumericKind numeric_kind,
            const std::string& name);

    void SetValue(Any&, long value) const;
    void SetValue(Any&, uint64_t value) const;
    void SetValue(Any&, double value) const;
    double GetValue(const Any&) const;

    bool IsInteger() const {
        return m_kind != NumericKind::Unknown && m_kind != NumericKind::Float &&
               m_kind != NumericKind::Double;
    }

    bool IsFloatingPoint() const {
        return m_kind != NumericKind::Unknown &&
               (m_kind == NumericKind::Float || m_kind == NumericKind::Double);
    }

private:
    enum NumericKind m_kind;

    template <typename T>
    static Numeric create() {
        return {ValueKind::Numeric, getKind<T>(), getName<T>()};
    }

    template <typename T>
    static enum NumericKind getKind() {
        if constexpr (std::is_same_v<int, T>) {
            return NumericKind::Int;
        }
        if constexpr (std::is_same_v<char, T>) {
            return NumericKind::Char;
        }
        if constexpr (std::is_same_v<short, T>) {
            return NumericKind::Short;
        }
        if constexpr (std::is_same_v<long long, T>) {
            return NumericKind::LongLong;
        }
        if constexpr (std::is_same_v<uint8_t, T>) {
            return NumericKind::Uint8;
        }
        if constexpr (std::is_same_v<uint16_t, T>) {
            return NumericKind::Uint16;
        }
        if constexpr (std::is_same_v<uint32_t, T>) {
            return NumericKind::Uint32;
        }
        if constexpr (std::is_same_v<uint64_t, T>) {
            return NumericKind::Uint64;
        }
        if constexpr (std::is_same_v<float, T>) {
            return NumericKind::Float;
        }
        if constexpr (std::is_same_v<double, T>) {
            return NumericKind::Double;
        }

        return NumericKind::Unknown;
    }

    template <typename T>
    static std::string getName() {
        if constexpr (std::is_same_v<int, T>) {
            return "int";
        }
        if constexpr (std::is_same_v<char, T>) {
            return "char";
        }
        if constexpr (std::is_same_v<short, T>) {
            return "short";
        }
        if constexpr (std::is_same_v<long long, T>) {
            return "long long";
        }
        if constexpr (std::is_same_v<uint8_t, T>) {
            return "uint8";
        }
        if constexpr (std::is_same_v<uint16_t, T>) {
            return "uint16";
        }
        if constexpr (std::is_same_v<uint32_t, T>) {
            return "uint32";
        }
        if constexpr (std::is_same_v<uint64_t, T>) {
            return "uint64";
        }
        if constexpr (std::is_same_v<float, T>) {
            return "float";
        }
        if constexpr (std::is_same_v<double, T>) {
            return "double";
        }

        return "unknown-numeric-type";
    }
};

}