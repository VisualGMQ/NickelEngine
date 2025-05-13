#pragma once

#include "nickel/refl/drefl/exception.hpp"
#include "nickel/refl/drefl/operation_traits.hpp"
#include "nickel/refl/util/misc.hpp"


namespace nickel::refl {

struct Type;

class Any final {
public:
    friend class Class;

    template <typename T>
    friend Any AnyMakeConstRef(const T&) noexcept;

    template <typename T>
    friend Any AnyMakeRef(T&) noexcept;

    template <typename T>
    friend Any AnyMakeCopy(T&& value) noexcept(
        std::is_rvalue_reference_v<T&&>
            ? std::is_nothrow_move_constructible_v<remove_cvref_t<T>>
            : std::is_nothrow_copy_constructible_v<remove_cvref_t<T>>);

    template <typename T>
    friend T* TryCast(Any&);

    template <typename T>
    friend const T* TryCastConst(const Any&);

    enum class AccessType {
        Null,
        ConstRef,
        Ref,
        Copy,
    };

    auto AccessType() const noexcept { return m_access; }

    Any() = default;

    Any(enum AccessType access, void* payload, const TypeOperations* operations,
        const Type* typeinfo)
        : m_access(access),
          m_payload(payload),
          m_operations(operations),
          m_type(typeinfo) {}

    Any(const Any&);
    Any(Any&& o);
    Any& operator=(const Any&);
    Any& operator=(Any&& o);

    ~Any();

    Any ConstRef() noexcept {
        return {AccessType::ConstRef, m_payload, m_operations, m_type};
    }

    Any Ref() {
        if (AccessType() == AccessType::ConstRef) {
            throw BadAnyAccess("can't make_ref from const& any");
        }
        return {AccessType::Ref, m_payload, m_operations, m_type};
    }

    Any Copy() {
        void* elem = m_operations->copy_construct(m_payload);
        return {AccessType::Copy, elem, m_operations, m_type};
    }

    void CopyAssign(Any& o) {
        if (o.TypeInfo() == TypeInfo()) {
            m_operations->copy_assignment(m_payload, o.m_payload);
        } else {
            LOGE("can't copy assign between two different types");
        }
    }

    void StealAssign(Any&& o) {
        if (o.TypeInfo() == TypeInfo()) {
            m_operations->steal_assignment(m_payload, o.m_payload);
        } else {
            LOGE("can't copy assign between two different types");
        }
    }

    [[nodiscard]] Any Steal() {
        auto access = m_access;
        auto payload = m_operations->steal_construct(m_payload);
        auto operations = m_operations;
        auto type = m_type;

        m_access = AccessType::Null;
        if (m_payload && m_access == AccessType::Copy) {
            m_operations->destroy(m_payload);
        }
        m_payload = nullptr;
        m_type = nullptr;

        return {access, payload, operations, type};
    }

    const Type* TypeInfo() const noexcept { return m_type; }

    bool HasValue() const noexcept { return m_access != AccessType::Null && m_payload; }

    bool IsRef() const noexcept {
        return m_access == AccessType::Ref;
    }

    bool IsConstRef() const noexcept {
        return m_access == AccessType::ConstRef;
    }

    bool IsCopyable() const noexcept {
        return m_access == AccessType::Copy;
    }

    bool IsNull() const noexcept {
        return m_access == AccessType::Null;
    }

    void* Payload() noexcept { return m_payload; }
    const void* Payload() const noexcept { return m_payload; }

    /**
     * @brief release payload
     */
    void* Release() {
        auto payload = m_payload;
        m_payload = nullptr;
        m_access = AccessType::Null;
        return payload;
    }

private:
    enum AccessType m_access = AccessType::Null;
    void* m_payload = nullptr;
    const TypeOperations* m_operations = &TypeOperations::null;
    const Type* m_type = nullptr;
};

}  // namespace nickel::refl