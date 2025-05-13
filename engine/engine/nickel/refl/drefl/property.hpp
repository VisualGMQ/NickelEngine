#pragma once

#include "nickel/refl/drefl/string.hpp"
#include "nickel/refl/drefl/enum.hpp"
#include "nickel/refl/drefl/factory.hpp"
#include "nickel/refl/drefl/qualifier.hpp"
#include "nickel/refl/drefl/type.hpp"
#include "nickel/refl/drefl/value_kind.hpp"
#include "nickel/refl/util/misc.hpp"
#include "nickel/refl/util/variable_traits.hpp"
#include <string>

namespace nickel::refl {

class Class;
class ClassVisitor;

namespace internal {

template <typename T>
using property_raw_t = remove_all_pointers_t<
    remove_cvref_t<typename variable_traits<T>::type>>;
}

class Property : public Type {
public:
    Property(const std::string& name, const Class* owner, Qualifier q)
        : Type(ValueKind::Property, name), m_owner(owner), m_qualifier(q) {}

    virtual void Visit(ClassVisitor*) = 0;

    virtual ~Property() = default;

    auto ClassInfo() const noexcept { return m_owner; }

    bool is_const_pointer() const noexcept {
        return static_cast<long>(m_qualifier) &
               static_cast<long>(Qualifier::ConstPointer);
    }

    bool IsConst() const noexcept {
        return static_cast<long>(m_qualifier) &
               static_cast<long>(Qualifier::Const);
    }

    bool IsRef() const noexcept {
        return static_cast<long>(m_qualifier) &
               static_cast<long>(Qualifier::Ref);
    }

    bool is_pointer() const noexcept {
        return static_cast<long>(m_qualifier) &
               static_cast<long>(Qualifier::Pointer);
    }

    virtual const Type* TypeInfo() const noexcept = 0;

private:
    const Class* m_owner;
    Qualifier m_qualifier = Qualifier::None;
};

class StringProperty : public Property {
public:
    using Property::property;

    void visit(ClassVisitor* visitor) override;
};

template <typename T>
class StringPropertyImpl : public StringProperty {
public:
    StringPropertyImpl(const std::string& name, const Class* owner,
                         Qualifier q, T pointer)
        : StringProperty(name, owner, q),
          m_pointer(pointer),
          m_type_info(
              &StringFactory<internal::property_raw_t<T>>::instance().info()) {
    }

    const struct Type* TypeInfo() const noexcept override {
        return m_type_info;
    }

private:
    T m_pointer;
    const String* m_type_info;
};

class BooleanProperty : public Property {
public:
    BooleanProperty(const std::string& name, const Class* owner, Qualifier q)
        : Property(name, owner, q),
          m_type_info(&BooleanFactory::Instance().Info()) {}

    void visit(ClassVisitor* visitor) override;

    const struct Type* TypeInfo() const noexcept override {
        return m_type_info;
    }

private:
    const Boolean* m_type_info;
};

template <typename T>
class BooleanPropertyImpl : public BooleanProperty {
public:
    BooleanPropertyImpl(const std::string& name, const Class* owner,
                          Qualifier q, T pointer)
        : BooleanProperty(name, owner, q), m_pointer(pointer) {}

private:
    T m_pointer;
};

class NumericProperty : public Property {
public:
    using Property::property;

    void visit(ClassVisitor* visitor) override;
};

template <typename T>
class NumericPropertyImpl : public NumericProperty {
public:
    NumericPropertyImpl(const std::string& name, const Class* owner,
                          Qualifier q, T pointer)
        : NumericProperty(name, owner, q),
          m_pointer(pointer),
          m_type_info(&NumericFactory<internal::property_raw_t<T>>::instance()
                          .info()) {}

    const struct Type* TypeInfo() const noexcept override {
        return m_type_info;
    }

private:
    T m_pointer = nullptr;
    const class Numeric* m_type_info;
};

class ClassProperty : public Property {
public:
    using Property::property;

    void visit(ClassVisitor*) override;
};

template <typename T>
class ClassPropertyImpl : public ClassProperty {
public:
    ClassPropertyImpl(const std::string& name, T accessor)
        : ClassProperty(
              name,
              &ClassFactory<
                   remove_cvref_t<typename variable_traits<T>::clazz>>::instance()
                   .info(),
              GetQualifier<typename variable_traits<T>::type>()),
          accessor_(accessor),
          m_type_info(
              &ClassFactory<internal::property_raw_t<T>>::instance().info()) {}

    const struct Type* TypeInfo() const noexcept override {
        return m_type_info;
    }

private:
    T accessor_ = nullptr;
    const class Class* m_type_info;
};

class EnumProperty : public Property {
public:
    explicit EnumProperty(const std::string& name, const Class* owner,
                           Qualifier q, const EnumInfo& enum_info)
        : Property(name, owner, q) {}

    void visit(ClassVisitor* visitor) override;
};

template <typename T>
class EnumPropertyImpl : public EnumProperty {
public:
    EnumPropertyImpl(const std::string& name, const Class* owner, Qualifier q,
                       T pointer)
        : EnumProperty(name, owner, q),
          m_pointer(pointer),
          type_info_(
              &EnumFactory<internal::property_raw_t<T>>::instance().info()) {}

    const struct Type* TypeInfo() const noexcept override {
        return type_info_;
    }

private:
    T m_pointer = nullptr;
    const class EnumInfo* type_info_ = nullptr;
};

}  // namespace nickel::refl