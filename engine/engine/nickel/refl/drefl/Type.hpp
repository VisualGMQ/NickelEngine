#pragma once

#include "nickel/refl/drefl/value_kind.hpp"
#include "nickel/refl/drefl/any.hpp"
#include "nickel/refl/drefl/config.hpp"
#include <algorithm>
#include <functional>

namespace nickel::refl {

class Class;
class Numeric;
class EnumInfo;
class Boolean;
class String;
class Pointer;
class Array;
class Optional;

struct Type {
    using default_construct_fn = std::function<Any(void)>;

    Type(ValueKind kind, const std::string& name, default_construct_fn fn)
        : m_kind(kind), m_name(name), m_default_constructor(fn) {}
    Type(ValueKind kind, default_construct_fn fn): m_kind(kind), m_default_constructor(fn) {}
    virtual ~Type() = default;

    auto& Attributes() const { return m_attributes; }
    void SetAttributes(const std::vector<Attribute>& attrs) { m_attributes = attrs; }
    void SetAttributes(std::vector<Attribute>&& attrs) { m_attributes = std::move(attrs);
    }

    bool FindAttribute(Attribute attr) const {
        return std::find(m_attributes.begin(), m_attributes.end(), attr) !=
               std::end(m_attributes);
    }

    auto Kind() const noexcept { return m_kind; }

    const Class* AsClass() const noexcept;
    const Numeric* AsNumeric() const noexcept;
    const EnumInfo* AsEnum() const noexcept;
    const Boolean* AsBoolean() const noexcept;
    const String* AsString() const noexcept;
    const Pointer* AsPointer() const noexcept;
    const Array* AsArray() const noexcept;
    const Optional* AsOptional() const noexcept;

    bool IsClass() const noexcept;
    bool IsNumeric() const noexcept;
    bool IsEnum() const noexcept;
    bool IsBoolean() const noexcept;
    bool IsString() const noexcept;
    bool IsPointer() const noexcept;
    bool IsArray() const noexcept;
    bool IsOptional() const noexcept;

    auto& Name() const noexcept { return m_name; }

    bool IsDefaultConstructible() const {
        return m_default_constructor != nullptr;
    }
    Any DefaultConstruct() const { return IsDefaultConstructible() ? m_default_constructor() : Any{}; }

protected:
    std::string m_name;
    default_construct_fn m_default_constructor;

private:
    ValueKind m_kind;
    std::vector<Attribute> m_attributes;
};

#define SET_VALUE_CHECK(a, type)                    \
    ((a.AccessType() == Any::AccessType::Ref ||   \
      a.AccessType() == Any::AccessType::Copy) && \
     a.TypeInfo()->Kind() == type)

#define COPY_VALUE_CHECK(a, type) \
    (a.AccessType() != Any::AccessType::Null && a.TypeInfo()->Kind() == type)

}  // namespace nickel::refl
