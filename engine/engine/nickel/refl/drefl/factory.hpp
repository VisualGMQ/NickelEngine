#pragma once

#include "nickel/refl/drefl/string.hpp"
#include "nickel/refl/drefl/any.hpp"
#include "nickel/refl/drefl/array.hpp"
#include "nickel/refl/drefl/bool.hpp"
#include "nickel/refl/drefl/class.hpp"
#include "nickel/refl/drefl/config.hpp"
#include "nickel/refl/drefl/enum.hpp"
#include "nickel/refl/drefl/numeric.hpp"
#include "nickel/refl/drefl/operation_traits.hpp"
#include "nickel/refl/drefl/optional.hpp"
#include "nickel/refl/drefl/pointer.hpp"
#include "nickel/refl/drefl/raw_type.hpp"
#include "nickel/refl/util/function_traits.hpp"
#include "nickel/refl/util/misc.hpp"
#include "nickel/refl/util/variable_traits.hpp"
#include <memory>
#include <type_traits>

namespace nickel::refl {

namespace internal {

template <typename T>
constexpr bool is_array_v = std::is_array_v<T> || is_std_array_v<T> ||
                            is_vector_v<T> || is_std_list_v<T>;

template <typename T>
constexpr bool is_string_v =
    std::is_same_v<std::string, T> || std::is_same_v<std::string_view, T>;
}  // namespace internal

class Class;
class ClassVisitor;

template <typename>
class ClassFactory;

template <typename>
class EnumFactory;

template <typename>
class Factory;

template <typename>
class StringFactory;

template <typename>
class NumericFactory;

template <typename>
class ArrayFactory;

template <typename>
class OptionalFactory;

class BooleanFactory final {
public:
    static auto& Instance() noexcept {
        static BooleanFactory inst;
        return inst;
    }

    auto& Info() const noexcept { return m_info; }

private:
    Boolean m_info;
};

class Property : public Type {
public:
    Property(const std::string& name, const Class* owner, Qualifier q)
        : Type(ValueKind::Property, name, nullptr), m_owner(owner), m_qualifier(q) {}

    virtual void Visit(ClassVisitor*) = 0;
    virtual Any CallConst(const Any&) const = 0;
    virtual Any Call(Any&) const = 0;

    virtual ~Property() = default;

    auto ClassInfo() const noexcept { return m_owner; }

    bool IsConst() const noexcept {
        return static_cast<long>(m_qualifier) &
               static_cast<long>(Qualifier::Const);
    }

    bool IsRef() const noexcept {
        return static_cast<long>(m_qualifier) &
               static_cast<long>(Qualifier::Ref);
    }

    virtual const Type* TypeInfo() const noexcept = 0;

    const Class* Owner() const noexcept { return m_owner; }

private:
    const Class* m_owner;
    Qualifier m_qualifier = Qualifier::None;
};

class StringProperty : public Property {
public:
    using Property::Property;

    void Visit(ClassVisitor* visitor) override;
};

class BooleanProperty : public Property {
public:
    BooleanProperty(const std::string& name, const Class* owner, Qualifier q)
        : Property(name, owner, q),
          m_type_info(&BooleanFactory::Instance().Info()) {}

    void Visit(ClassVisitor* visitor) override;

    const Type* TypeInfo() const noexcept override {
        return m_type_info;
    }

private:
    const Boolean* m_type_info;
};

class PointerProperty : public Property {
public:
    PointerProperty(const std::string& name, const Class* owner, Qualifier q,
                     const Type* pointer_type)
        : Property(name, owner, q), m_pointer_type(pointer_type) {}

    void Visit(ClassVisitor* visitor) override;

    const Type* PointerType() const noexcept { return m_pointer_type; }

private:
    const Type* m_pointer_type;
};

class NumericProperty : public Property {
public:
    using Property::Property;

    void Visit(ClassVisitor* visitor) override;
};

class ClassProperty : public Property {
public:
    using Property::Property;

    void Visit(ClassVisitor*) override;
};

class EnumProperty : public Property {
public:
    explicit EnumProperty(const std::string& name, const Class* owner,
                           Qualifier q)
        : Property(name, owner, q) {}

    void Visit(ClassVisitor* visitor) override;
};

class ArrayProperty : public Property {
public:
    explicit ArrayProperty(const std::string& name, const Class* owner,
                            Qualifier q)
        : Property(name, owner, q) {}

    void Visit(ClassVisitor* visitor) override;
};

class OptionalProperty : public Property {
public:
    explicit OptionalProperty(const std::string& name, const Class* owner,
                                Qualifier q)
        : Property(name, owner, q) {}

    void Visit(ClassVisitor* visitor) override;
};

template <typename T>
Any CallPropertyConst(const Any& a, T accessor, const Type* owner) {
    if (owner != a.TypeInfo() || a.IsNull()) {
        return {};
    }

    using traits = variable_traits<T>;
    using Class = typename traits::clazz;
    using type = remove_cvref_t<typename traits::type>;

    auto& value = ((const Class*)(a.Payload()))->*accessor;

    auto& operations = TypeOperationTraits<type>::GetOperations();
    auto info = Factory<type>::Info();
    if (!info) {
        info = &ClassFactory<type>::Instance().Info();
    }

    return {Any::AccessType::ConstRef, (void*)&value, &operations, info};
}

template <typename T>
Any CallProperty(Any& a, T accessor, const Type* owner) {
    if (owner != a.TypeInfo() || a.IsNull()) {
        return {};
    }

    using traits = variable_traits<T>;
    using Class = typename traits::clazz;
    using type = remove_cvref_t<typename traits::type>;

    auto& value = ((Class*)a.Payload())->*accessor;
    auto& operations = TypeOperationTraits<type>::GetOperations();
    auto info = Factory<type>::Info();
    if (!info) {
        info = &ClassFactory<type>::Instance().Info();
    }

    if (a.IsConstRef()) {
        return {Any::AccessType::ConstRef, (void*)&value, &operations, info};
    } else {
        return {Any::AccessType::Ref, (void*)&value, &operations, info};
    }
}

template <typename T>
class StringPropertyImpl : public StringProperty {
public:
    StringPropertyImpl(const std::string& name, const Class* owner,
                         Qualifier q, T pointer)
        : StringProperty(name, owner, q),
          m_pointer(pointer),
          m_type_info(&StringFactory<remove_cvref_t<
                          typename variable_traits<T>::type>>::Instance()
                          .Info()) {}

    Any CallConst(const Any& a) const override {
        return CallPropertyConst(a, m_pointer, this->Owner());
    }

    Any Call(Any& a) const override {
        return CallProperty(a, m_pointer, this->Owner());
    }

    const Type* TypeInfo() const noexcept override {
        return m_type_info;
    }

private:
    T m_pointer;
    const String* m_type_info;
};

template <typename T>
class BooleanPropertyImpl : public BooleanProperty {
public:
    BooleanPropertyImpl(const std::string& name, const Class* owner,
                          Qualifier q, T pointer)
        : BooleanProperty(name, owner, q), m_pointer(pointer) {}

    Any CallConst(const Any& a) const override {
        return CallPropertyConst(a, m_pointer, this->Owner());
    }

    Any Call(Any& a) const override {
        return CallProperty(a, m_pointer, this->Owner());
    }

private:
    T m_pointer;
};

template <typename T>
class NumericPropertyImpl : public NumericProperty {
public:
    NumericPropertyImpl(const std::string& name, const Class* owner,
                          Qualifier q, T pointer)
        : NumericProperty(name, owner, q),
          m_pointer(pointer),
          m_type_info(&NumericFactory<remove_cvref_t<
                          typename variable_traits<T>::type>>::Instance()
                          .Info()) {}

    Any CallConst(const Any& a) const override {
        return CallPropertyConst(a, m_pointer, this->Owner());
    }

    Any Call(Any& a) const override {
        return CallProperty(a, m_pointer, this->Owner());
    }

    const Type* TypeInfo() const noexcept override {
        return m_type_info;
    }

private:
    T m_pointer = nullptr;
    const Numeric* m_type_info;
};

template <typename T>
class PointerPropertyImpl : public PointerProperty {
public:
    PointerPropertyImpl(const std::string& name, const Class* owner,
                          Qualifier q, const Type* pointer_type, T pointer)
        : PointerProperty(name, owner, q, pointer_type),
          m_pointer(pointer),
          m_type_info(&NumericFactory<remove_cvref_t<
                          typename variable_traits<T>::type>>::Instance()
                          .Info()) {}

    Any CallConst(const Any& a) const override {
        return CallPropertyConst(a, m_pointer, this->Owner());
    }

    Any Call(Any& a) const override {
        return CallProperty(a, m_pointer, this->Owner());
    }

    const Type* TypeInfo() const noexcept override {
        return m_type_info;
    }

private:
    T m_pointer = nullptr;
    const Numeric* m_type_info;
};

template <typename T>
class ClassPropertyImpl : public ClassProperty {
public:
    ClassPropertyImpl(const std::string& name, T accessor)
        : ClassProperty(
              name,
              &ClassFactory<remove_cvref_t<
                   typename variable_traits<T>::Class>>::Instance()
                   .Info(),
              GetQualifier<typename variable_traits<T>::type>()),
          m_pointer(accessor),
          m_type_info(&ClassFactory<remove_cvref_t<
                          typename variable_traits<T>::type>>::Instance()
                          .Info()) {}

    const Type* TypeInfo() const noexcept override {
        return m_type_info;
    }

    Any CallConst(const Any& a) const override {
        return CallPropertyConst(a, m_pointer, this->Owner());
    }

    Any Call(Any& a) const override {
        return CallProperty(a, m_pointer, this->Owner());
    }

private:
    T m_pointer = nullptr;
    const Class* m_type_info;
};

template <typename T>
class EnumPropertyImpl : public EnumProperty {
public:
    EnumPropertyImpl(const std::string& name, const Class* owner, Qualifier q,
                       T pointer)
        : EnumProperty(name, owner, q),
          m_pointer(pointer),
          m_type_info(&EnumFactory<remove_cvref_t<
                          typename variable_traits<T>::type>>::Instance()
                          .Info()) {}

    const Type* TypeInfo() const noexcept override {
        return m_type_info;
    }

    Any CallConst(const Any& a) const override {
        return CallPropertyConst(a, m_pointer, this->Owner());
    }

    Any Call(Any& a) const override {
        return CallProperty(a, m_pointer, this->Owner());
    }

private:
    T m_pointer = nullptr;
    const EnumInfo* m_type_info = nullptr;
};

template <typename T>
class ArrayPropertyImpl : public ArrayProperty {
public:
    ArrayPropertyImpl(const std::string& name, const Class* owner,
                        Qualifier q, T pointer)
        : ArrayProperty(name, owner, q),
          m_pointer(pointer),
          m_type_info(&ArrayFactory<remove_cvref_t<
                          typename variable_traits<T>::type>>::Instance()
                          .Info()) {}

    const Type* TypeInfo() const noexcept override {
        return m_type_info;
    }

    Any CallConst(const Any& a) const override {
        return CallPropertyConst(a, m_pointer, this->Owner());
    }

    Any Call(Any& a) const override {
        return CallProperty(a, m_pointer, this->Owner());
    }

private:
    T m_pointer = nullptr;
    const Array* m_type_info = nullptr;
};

template <typename T>
class OptionalPropertyImpl : public OptionalProperty {
public:
    OptionalPropertyImpl(const std::string& name, const Class* owner,
                        Qualifier q, T pointer)
        : OptionalProperty{name, owner, q},
          m_pointer(pointer),
          m_type_info(&OptionalFactory<remove_cvref_t<
                          typename variable_traits<T>::type>>::Instance()
                          .Info()) {}

    const Type* TypeInfo() const noexcept override {
        return m_type_info;
    }

    Any CallConst(const Any& a) const override {
        return CallPropertyConst(a, m_pointer, this->Owner());
    }

    Any Call(Any& a) const override {
        return CallProperty(a, m_pointer, this->Owner());
    }

private:
    T m_pointer = nullptr;
    const Optional* m_type_info = nullptr;
};


class EnumPropertyFactory final {
public:
    template <typename T>
    EnumPropertyFactory(const std::string& name, T accessor) {
        using traits = variable_traits<T>;
        using var_type = typename traits::type;
        using enum_type = remove_cvref_t<var_type>;

        static_assert(std::is_enum_v<enum_type>);

        m_property = std::make_shared<EnumPropertyImpl<T>>(
            name, &ClassFactory<typename traits::clazz>::Instance().Info(),
            GetQualifier<var_type>(), accessor);
    }

    auto& Get() const noexcept { return m_property; }

private:
    std::shared_ptr<EnumProperty> m_property;
};

class NumericPropertyFactory final {
public:
    template <typename T>
    NumericPropertyFactory(const std::string& name, T accessor) {
        using traits = variable_traits<T>;
        using var_type = typename traits::type;

        m_property = std::make_shared<NumericPropertyImpl<T>>(
            name, &ClassFactory<typename traits::clazz>::Instance().Info(),
            GetQualifier<var_type>(), accessor);
    }

    auto& Get() const noexcept { return m_property; }

private:
    std::shared_ptr<NumericProperty> m_property;
};

class StringPropertyFactory final {
public:
    template <typename T>
    StringPropertyFactory(const std::string& name, T accessor) {
        using traits = variable_traits<T>;
        using var_type = typename traits::type;

        m_property = std::make_shared<StringPropertyImpl<T>>(
            name, &ClassFactory<typename traits::clazz>::Instance().Info(),
            GetQualifier<var_type>(), accessor);
    }

    auto& Get() const noexcept { return m_property; }

private:
    std::shared_ptr<StringProperty> m_property;
};

class BooleanPropertyFactory final {
public:
    template <typename T>
    BooleanPropertyFactory(const std::string& name, T accessor) {
        using traits = variable_traits<T>;
        using var_type = typename traits::type;

        m_property = std::make_shared<BooleanPropertyImpl<T>>(
            name, &ClassFactory<typename traits::clazz>::Instance().Info(),
            GetQualifier<var_type>(), accessor);
    }

    auto& Get() const noexcept { return m_property; }

private:
    std::shared_ptr<BooleanProperty> m_property;
};

class PointerPropertyFactory final {
public:
    template <typename T>
    PointerPropertyFactory(const std::string& name, T accessor) {
        using traits = variable_traits<T>;
        using var_type = typename traits::type;

        m_property = std::make_shared<PointerPropertyImpl<T>>(
            name, &ClassFactory<typename traits::clazz>::Instance().Info(),
            GetQualifier<var_type>(), Factory<remove_cvref_t<T>>::Info(),
            accessor);
    }

    auto& Get() const noexcept { return m_property; }

private:
    std::shared_ptr<PointerProperty> m_property;
};

class ArrayPropertyFactory final {
public:
    template <typename T>
    ArrayPropertyFactory(const std::string& name, T accessor) {
        using traits = variable_traits<T>;
        using var_type = typename traits::type;

        m_property = std::make_shared<ArrayPropertyImpl<T>>(
            name, &ClassFactory<typename traits::clazz>::Instance().Info(),
            GetQualifier<var_type>(), accessor);
    }

    auto& Get() const noexcept { return m_property; }

private:
    std::shared_ptr<ArrayProperty> m_property;
};

class OptionalPropertyFactory final {
public:
    template <typename T>
    OptionalPropertyFactory(const std::string& name, T accessor) {
        using traits = variable_traits<T>;
        using var_type = typename traits::type;

        m_property = std::make_shared<OptionalPropertyImpl<T>>(
            name, &ClassFactory<typename traits::clazz>::Instance().Info(),
            GetQualifier<var_type>(), accessor);
    }

    auto& Get() const noexcept { return m_property; }

private:
    std::shared_ptr<OptionalProperty> m_property;
};

class ClassPropertyFactory final {
public:
    template <typename T>
    ClassPropertyFactory(const std::string& name, T accessor) {
        using traits = variable_traits<T>;
        using var_type = typename traits::type;

        m_property = std::make_shared<ClassPropertyImpl<T>>(name, accessor);
    }

    auto& Get() const { return m_property; }

private:
    std::shared_ptr<ClassProperty> m_property;
};

class PropertyFactory final {
public:
    template <typename T>
    std::shared_ptr<Property> Create(const std::string& name, T accessor) {
        if constexpr (is_function_v<T>) {
            // TODO: use function_factory here
            return nullptr;
        } else {
            using traits = variable_traits<T>;
            using type = remove_cvref_t<typename traits::type>;

            if constexpr (std::is_pointer_v<type>) {
                return PointerPropertyFactory{name, accessor}.Get();
            } else if constexpr (is_optional_v<type>) {
                return OptionalPropertyFactory{name, accessor}.Get();
            } else if constexpr (std::is_same_v<bool, type>) {
                return BooleanPropertyFactory{name, accessor}.Get();
            } else if constexpr (std::is_enum_v<type>) {
                return EnumPropertyFactory{name, accessor}.Get();
            } else if constexpr (std::is_fundamental_v<type>) {
                return NumericPropertyFactory{name, accessor}.Get();
            } else if constexpr (internal::is_string_v<type>) {
                return StringPropertyFactory{name, accessor}.Get();
            } else if constexpr (internal::is_array_v<type>) {
                return ArrayPropertyFactory{name, accessor}.Get();
            } else {
                return ClassPropertyFactory{name, accessor}.Get();
            }
        }
    }
};

class TypeDict final {
public:
    static auto& Instance() {
        static TypeDict inst;
        return inst;
    }

    void Add(const Type* type) {
        if (type && !type->Name().empty()) {
            m_type_map.insert_or_assign(type->Name(), type);
        }
    }

    const Type* Find(std::string_view name) {
        if (auto it = m_type_map.find(name); it != m_type_map.end())  {
            return it->second;
        }
        return nullptr;
    }

    auto& TypeInfos() const noexcept { return m_type_map; }

private:
    std::unordered_map<std::string_view, const Type*> m_type_map;

    TypeDict() = default;
};

template <typename T>
class EnumFactory final {
public:
    static_assert(std::is_enum_v<T>);

    template <typename>
    friend class EnumFactory;

    EnumFactory() {}

    static EnumFactory& Instance() noexcept {
        static EnumFactory inst;

        static bool inited = false;
        if (!inited) {
            inited = true;
            TypeDict::Instance().Add(&inst.m_enum_info);
            inst.m_enum_info.m_default_constructor = [] {
                return Any{Any::AccessType::Copy, new T{},
                           &TypeOperationTraits<T>::GetOperations(),
                           &inst.m_enum_info};
            };
        }

        return inst;
    }

    auto& Regist(const std::string& name) noexcept {
        m_enum_info.m_name = name;

        if (!TypeDict::Instance().Find(name)) {
            TypeDict::Instance().Add(&m_enum_info);
        } else {
            LOGE("type " + name + "already registered");
        }

        return *this;
    }

    template <typename U>
    auto& Add(const std::string& name, U value) noexcept {
        m_enum_info.add(name, value);

        return *this;
    }

    auto& Info() const noexcept { return m_enum_info; }

    void Unregist() noexcept {
        m_enum_info.m_items.clear();
        m_enum_info.m_name.clear();
    }

    bool HasRegistered() const noexcept { return !m_enum_info.m_name.empty(); }

private:
    EnumInfo m_enum_info;
};

template <typename T>
class NumericFactory final {
public:
    static auto& Instance() noexcept {
        static NumericFactory inst{Numeric::create<T>()};

        static bool inited = false;
        if (!inited) {
            inited = true;
            TypeDict::Instance().Add(&inst.m_info);
        }

        return inst;
    }

    auto& Info() const noexcept { return m_info; }

private:
    NumericFactory(Numeric s) : m_info(s) {}

    Numeric m_info;
};

template <typename T>
class StringFactory final {
public:
    static auto& Instance() noexcept {
        static StringFactory inst{String::Create<T>()};

        static bool inited = false;
        if (!inited) {
            inited = true;
            TypeDict::Instance().Add(&inst.m_info);
        }

        return inst;
    }

    auto& Info() const noexcept { return m_info; }

private:
    StringFactory(const String& info) : m_info(info) {}

    String m_info;
};

template <typename T>
class PointerFactory final {
public:
    static auto& Instance() noexcept;

    auto& Info() const noexcept { return m_info; }

private:
    PointerFactory(const Pointer& p) : m_info{p} {}

    Pointer m_info;
};

template <typename T>
class ArrayFactory final {
public:
    static auto& Instance() noexcept;

    auto& Info() const noexcept { return m_info; }

private:
    ArrayFactory(const Array& a) : m_info{a} {}

    Array m_info;
};

template <typename T>
class OptionalFactory final {
public:
    static auto& Instance() noexcept;
    auto& Info() const noexcept { return m_info; }

private:
    OptionalFactory(const Optional& a): m_info{a} {}

    Optional m_info;
};

template <typename T>
class Factory final {
public:
    static const Type* Info() noexcept {
        if constexpr (std::is_pointer_v<T>) {
            return &PointerFactory<T>::Instance().Info();
        }
        if constexpr (is_optional_v<T>) {
            return &OptionalFactory<T>::Instance().Info();
        }
        if constexpr (std::is_same_v<bool, T>) {
            return &BooleanFactory::Instance().Info();
        }
        if constexpr (internal::is_string_v<T>) {
            return &StringFactory<T>::Instance().Info();
        }
        if constexpr (std::is_enum_v<T>) {
            return &EnumFactory<T>::Instance().Info();
        }
        if constexpr (internal::is_array_v<T>) {
            return &ArrayFactory<T>::Instance().Info();
        }
        if constexpr (std::is_fundamental_v<T>) {
            return &NumericFactory<T>::Instance().Info();
        }

        return nullptr;
    }
};

template <typename T>
class ClassFactory final {
public:
    static auto& Instance() noexcept {
        static ClassFactory inst;

        static bool inited = false;
        if (!inited) {
            inited = true;
            if constexpr (std::is_default_constructible_v<T>) {
                inst.m_info.m_default_constructor = defaultConstructor;
            }
            TypeDict::Instance().Add(&inst.m_info);
        }

        return inst;
    }

    auto& Regist(const std::string& name,
                 std::vector<Attribute>&& attrs = {}) {
        m_info.m_name = name;
        m_info.SetAttributes(std::move(attrs));

        if (!TypeDict::Instance().Find(name)) {
            TypeDict::Instance().Add(&m_info);
        } else {
            LOGE("type " + name + "already registered");
        }

        return *this;
    }

    template <typename U>
    auto& Property(const std::string& name, U accessor, std::vector<Attribute>&& attrs = {}) {
        using traits = variable_traits<U>;
        using type = typename traits::type;

        m_info.m_properties.emplace_back(
            PropertyFactory{}.Create(name, accessor));
        m_info.m_properties.back()->SetAttributes(std::move(attrs));

        return *this;
    }

    auto& Info() const noexcept { return m_info; }

private:
    Class m_info;

    ClassFactory() = default;

    static Any defaultConstructor() {
        if constexpr (std::is_default_constructible_v<T>) {
            return Any{Any::AccessType::Copy, new T,
                    &TypeOperationTraits<T>::GetOperations(),
                    &ClassFactory<T>::Instance().Info()};
        } else {
            return {};
        }
    }
};

template <typename T>
class Registrar final {
public:
    Registrar() = delete;

    static auto& Instance() {
        if constexpr (std::is_pointer_v<T>) {
            return PointerFactory<T>::Instance();
        } else if constexpr (is_optional_v<T>) {
            return OptionalFactory<T>::Instance();
        } else if constexpr (std::is_same_v<bool, T>) {
            return BooleanFactory::Instance();
        } else if constexpr (internal::is_string_v<T>) {
            return StringFactory<T>::Instance();
        } else if constexpr (std::is_enum_v<T>) {
            return EnumFactory<T>::Instance();
        } else if constexpr (internal::is_array_v<T>) {
            return ArrayFactory<T>::Instance();
        } else if constexpr (std::is_fundamental_v<T>) {
            return NumericFactory<T>::Instance();
        } else {
            return ClassFactory<T>::Instance();
        }
    }
};

template <typename T>
const Type* TypeInfo() {
    const Type* t = Factory<T>::Info();

    if (!t) {
        if constexpr (std::is_class_v<T>) {
            t = &ClassFactory<T>::Instance().Info();
        }
    }
    
    return t;
}

inline const Type* TypeInfo(std::string_view name) {
    return TypeDict::Instance().Find(name);
}

inline auto& AllTypeInfo() {
    return TypeDict::Instance().TypeInfos();
}

template <typename T>
auto& PointerFactory<T>::Instance() noexcept {
    static PointerFactory inst{Pointer::Create<T>(TypeInfo<raw_type_t<T>>())};

    static bool inited = false;
    if (!inited) {
        inited = true;
        TypeDict::Instance().Add(&inst.m_info);
    }
    return inst;
}

namespace internal {

template <typename T>
struct array_element_type {
    using type = typename T::value_type;
};

template <typename T, size_t N>
struct array_element_type<T[N]> {
    using type = T;
};

};  // namespace internal

template <typename T>
auto& ArrayFactory<T>::Instance() noexcept {
    static ArrayFactory inst{Array::Create<T>(TypeInfo<array_element_t<T>>())};

    static bool inited = false;
    if (!inited) {
        inited = true;
        TypeDict::Instance().Add(&inst.m_info);
        inst.m_info.m_default_constructor = [] {
            return Any{Any::AccessType::Copy, new T{},
                       &TypeOperationTraits<T>::GetOperations(),
                       &inst.m_info};
        };
    }
    return inst;
}

template <typename T>
auto& OptionalFactory<T>::Instance() noexcept {
    static OptionalFactory inst{
        Optional::Create<T>(TypeInfo<typename T::value_type>())};

    static bool inited = false;
    if (!inited) {
        inited = true;
        TypeDict::Instance().Add(&inst.m_info);
        inst.m_info.m_default_constructor = [] {
            return Any{Any::AccessType::Copy, new T{},
                       &TypeOperationTraits<T>::GetOperations(),
                       &inst.m_info};
        };
    }

    return inst;
}

}  // namespace nickel::refl