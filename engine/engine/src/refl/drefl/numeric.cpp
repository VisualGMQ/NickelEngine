#include "nickel/refl/drefl/numeric.hpp"
#include "nickel/refl/drefl/any.hpp"
#include "nickel/refl/drefl/cast_any.hpp"
#include "nickel/refl/drefl/make_any.hpp"
#include "nickel/refl/drefl/value_kind.hpp"

namespace nickel::refl {

Any DefaultCreate(enum Numeric::NumericKind kind) {
    switch (kind) {
        case Numeric::NumericKind::Unknown:
            return {};
        case Numeric::NumericKind::Char:
            return AnyMakeCopy<char>(0);
        case Numeric::NumericKind::Int:
            return AnyMakeCopy<int>(0);
        case Numeric::NumericKind::Short:
            return AnyMakeCopy<short>(0);
        case Numeric::NumericKind::LongLong:
            return AnyMakeCopy<long long>(0);
        case Numeric::NumericKind::Uint8:
            return AnyMakeCopy<uint8_t>(0);
        case Numeric::NumericKind::Uint16:
            return AnyMakeCopy<uint16_t>(0);
        case Numeric::NumericKind::Uint32:
            return AnyMakeCopy<uint32_t>(0);
        case Numeric::NumericKind::Uint64:
            return AnyMakeCopy<uint64_t>(0);
        case Numeric::NumericKind::Float:
            return AnyMakeCopy<float>(0);
        case Numeric::NumericKind::Double:
            return AnyMakeCopy<double>(0);
    }

    return {};
}

Numeric::Numeric(ValueKind value_kind, enum NumericKind numeric_kind,
                 const std::string& name)
    : Type(value_kind, name, [=]() { return DefaultCreate(numeric_kind); }),
      m_kind(numeric_kind) {}

template <typename T>
void do_set_value(Any& a, T value) {
    switch (a.TypeInfo()->AsNumeric()->NumericKind()) {
        case Numeric::NumericKind::Unknown:
            LOGE("any has unknown numeric type");
            break;
        case Numeric::NumericKind::Char:
            *TryCast<char>(a) = value;
            break;
        case Numeric::NumericKind::Int:
            *TryCast<int>(a) = value;
            break;
        case Numeric::NumericKind::Short:
            *TryCast<short>(a) = value;
            break;
        case Numeric::NumericKind::LongLong:
            *TryCast<long long>(a) = value;
            break;
        case Numeric::NumericKind::Uint8:
            *TryCast<uint8_t>(a) = value;
            break;
        case Numeric::NumericKind::Uint16:
            *TryCast<uint16_t>(a) = value;
            break;
        case Numeric::NumericKind::Uint32:
            *TryCast<uint32_t>(a) = value;
            break;
        case Numeric::NumericKind::Uint64:
            *TryCast<uint64_t>(a) = value;
            break;
        case Numeric::NumericKind::Float:
            *TryCast<float>(a) = value;
            break;
        case Numeric::NumericKind::Double:
            *TryCast<double>(a) = value;
            break;
    }
}

void Numeric::SetValue(Any& a, long value) const {
    if (!SET_VALUE_CHECK(a, ValueKind::Numeric)) {
        LOGE("can't set boolean value to any");
        return;
    }

    do_set_value(a, value);
}

void Numeric::SetValue(Any& a, uint64_t value) const {
    if (!SET_VALUE_CHECK(a, ValueKind::Numeric)) {
        LOGE("can't set boolean value to any");
        return;
    }

    do_set_value(a, value);
}

void Numeric::SetValue(Any& a, double value) const {
    if (!SET_VALUE_CHECK(a, ValueKind::Numeric)) {
        LOGE("can't set boolean value to any");
        return;
    }

    do_set_value(a, value);
}

double Numeric::GetValue(const Any& a) const {
    if (a.TypeInfo()->Kind() != ValueKind::Numeric) return 0;

    switch (a.TypeInfo()->AsNumeric()->NumericKind()) {
        case NumericKind::Unknown:
            return 0;
        case NumericKind::Char:
            return *TryCastConst<char>(a);
        case NumericKind::Int:
            return *TryCastConst<int>(a);
        case NumericKind::Short:
            return *TryCastConst<short>(a);
        case NumericKind::LongLong:
            return *TryCastConst<long long>(a);
        case NumericKind::Uint8:
            return *TryCastConst<uint8_t>(a);
        case NumericKind::Uint16:
            return *TryCastConst<uint16_t>(a);
        case NumericKind::Uint32:
            return *TryCastConst<uint32_t>(a);
        case NumericKind::Uint64:
            return *TryCastConst<uint64_t>(a);
        case NumericKind::Float:
            return *TryCastConst<float>(a);
        case NumericKind::Double:
            return *TryCastConst<double>(a);
    }

    return 0;
}

}  // namespace nickel::refl