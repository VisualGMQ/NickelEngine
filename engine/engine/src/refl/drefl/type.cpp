#include "nickel/refl/drefl/type.hpp"
#include "nickel/refl/drefl/string.hpp"
#include "nickel/refl/drefl/array.hpp"
#include "nickel/refl/drefl/bool.hpp"
#include "nickel/refl/drefl/class.hpp"
#include "nickel/refl/drefl/enum.hpp"
#include "nickel/refl/drefl/numeric.hpp"
#include "nickel/refl/drefl/optional.hpp"
#include "nickel/refl/drefl/pointer.hpp"
#include "nickel/refl/drefl/value_kind.hpp"

namespace nickel::refl {

const Class* Type::AsClass() const noexcept {
    if (Kind() == ValueKind::Class) {
        return static_cast<const Class*>(this);
    }
    return nullptr;
}

const Numeric* Type::AsNumeric() const noexcept {
    if (Kind() == ValueKind::Numeric) {
        return static_cast<const Numeric*>(this);
    }
    return nullptr;
}

const EnumInfo* Type::AsEnum() const noexcept {
    if (Kind() == ValueKind::Enum) {
        return static_cast<const EnumInfo*>(this);
    }
    return nullptr;
}

const Boolean* Type::AsBoolean() const noexcept {
    if (Kind() == ValueKind::Boolean) {
        return static_cast<const Boolean*>(this);
    }
    return nullptr;
}

const String* Type::AsString() const noexcept {
    if (Kind() == ValueKind::String) {
        return static_cast<const String*>(this);
    }
    return nullptr;
}

const Pointer* Type::AsPointer() const noexcept {
    if (Kind() == ValueKind::Pointer) {
        return static_cast<const Pointer*>(this);
    }

    return nullptr;
}

const Array* Type::AsArray() const noexcept {
    if (Kind() == ValueKind::Array) {
        return static_cast<const Array*>(this);
    }

    return nullptr;
}

const Optional* Type::AsOptional() const noexcept {
    if (Kind() == ValueKind::Optional) {
        return static_cast<const Optional*>(this);
    }
    return nullptr;
}

bool Type::IsClass() const noexcept {
    return m_kind == ValueKind::Class;
}

bool Type::IsNumeric() const noexcept {
    return m_kind == ValueKind::Numeric;
}

bool Type::IsEnum() const noexcept {
    return m_kind == ValueKind::Enum;
}

bool Type::IsBoolean() const noexcept {
    return m_kind == ValueKind::Boolean;
}

bool Type::IsString() const noexcept {
    return m_kind == ValueKind::String;
}

bool Type::IsPointer() const noexcept {
    return m_kind == ValueKind::Pointer;
}

bool Type::IsArray() const noexcept {
    return m_kind == ValueKind::Array;
}

bool Type::IsOptional() const noexcept {
    return m_kind == ValueKind::Optional;
}

}  // namespace nickel::refl