#pragma once

#include "nickel/refl/drefl/type.hpp"
#include "nickel/refl/drefl/value_kind.hpp"
#include "nickel/refl/drefl/any.hpp"
#include "nickel/refl/drefl/operation_traits.hpp"

namespace nickel::refl {

class Optional final : public Type {
public:
    template <typename T>
    friend class OptionalFactory;

    using set_value_fn = void(const void*, void*);
    using set_inner_value_fn = void(const void*, void*);
    using get_value_fn = void*(void*);
    using has_value_fn = bool(const void*);

    struct operations final {
        set_value_fn* set_value_ = nullptr;
        get_value_fn* get_value_ = nullptr;
        has_value_fn* has_value_ = nullptr;
        set_inner_value_fn* set_inner_value_ = nullptr;
    };

    template <typename T>
    struct Traits {
        using value_type = typename T::value_type;

        static void SetInnerValue(const void* src, void* dst) {
            *(T*)(dst) = *(value_type*)src;
        }

        static void SetValue(const void* src, void* dst) {
            *(T*)(dst) = *(T*)src;
        }

        static void* GetValue(void* value) {
            return &((T*)value)->value();
        }

        static bool HasValue(const void* value) {
            return ((const T*)(value))->has_value();
        }

        static const operations& GetOperations() {
            using type = Traits<T>;
            static operations ops = {SetValue, GetValue,
                                     HasValue, SetInnerValue};
            return ops;
        }
    };

    Optional(const Type* elem_type, const std::string& name,
                      const operations& ops, const TypeOperations& elem_ops)
        : Type{ValueKind::Optional, name, nullptr},
          m_elem_type(elem_type),
          m_operations{&ops},
          m_elem_operations(&elem_ops) {}

    template <typename T>
    static Optional Create(const Type* elem_type) {
        return {elem_type, "std::optional<" + elem_type->Name() + ">",
                Traits<T>::GetOperations(),
                TypeOperationTraits<typename T::value_type>::GetOperations()};
    }

    const Type* ElemType() const noexcept { return m_elem_type; }

    Any GetValue(Any& value) const {
        void* elem = m_operations->get_value_(value.Payload());
        return {value.IsConstRef() ? Any::AccessType::ConstRef
                                    : Any::AccessType::Ref,
                elem, m_elem_operations, m_elem_type};
    }

    Any GetValueConst(const Any& value) const {
        void* elem = m_operations->get_value_((void*)value.Payload());
        return {Any::AccessType::ConstRef,
                elem, m_elem_operations, m_elem_type};
    }

    void SetValue(const Any& src, Any& value) const {
        if (src.TypeInfo() == this) {
            m_operations->set_value_(src.Payload(), value.Payload());
        } else {
            LOGE("can't set std::optional value due to type incorrect");
        }
    }

    void SetInnerValue(const Any& src, Any& value) const {
        if (src.TypeInfo() == m_elem_type) {
            m_operations->set_inner_value_(src.Payload(), value.Payload());
        } else {
            LOGE("can't set std::optional value due to type incorrect");
        }
    }

    bool HasValue(const Any& value) const noexcept {
        return m_operations->has_value_(value.Payload());
    }

private:
    const Type* m_elem_type;
    const operations* m_operations;
    const TypeOperations* m_elem_operations;
};

}  // namespace nickel::refl