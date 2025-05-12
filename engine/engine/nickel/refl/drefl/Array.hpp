#pragma once

#include "nickel/refl/drefl/type.hpp"
#include "nickel/refl/drefl/any.hpp"
#include "nickel/refl/drefl/array_operations.hpp"
#include "nickel/refl/drefl/operation_traits.hpp"
#include "nickel/refl/util/misc.hpp"
#include <iterator>

namespace nickel::refl {

struct TypeOperations;

class Array : public Type {
public:
    template <typename>
    friend class ArrayFactory;

    enum class ArrayType {
        Static,
        Dynamic,
    };

    enum class AddressingType {
        Random,   // like std::vector, std::array
        Forward,  // like std::list
    };

    template <typename T>
    static Array Create(const Type* elem_type) {
        return {
            getName<T>(),
            getArrayType<T>(),
            getAddressingType<T>(),
            elem_type,
            &array_operation_traits<T>::GetOperations(),
            &TypeOperationTraits<array_element_t<T>>::GetOperations(),
            nullptr};
    }

    enum ArrayType ArrayType() const noexcept { return m_array_type; }

    enum AddressingType AddressingType() const noexcept {
        return m_addressing_type;
    }

    auto ElemType() const noexcept {
        return m_elem_type;
    }

    Any Get(size_t idx, Any&) const noexcept;
    Any GetConst(size_t idx, const Any&) const noexcept;
    bool PushBack(const Any&, Any&) const noexcept;
    bool PopBack(Any&) const noexcept;
    Any Back(Any&) const noexcept;
    Any BackConst(const Any&) const noexcept;
    bool Resize(size_t size, Any& array) const noexcept;
    size_t Size(const Any&) const noexcept;
    size_t Capacity(const Any&) const noexcept;
    bool Insert(size_t idx, const Any&, Any&) const noexcept;

private:
    enum ArrayType m_array_type;
    enum AddressingType m_addressing_type;
    const Type* m_elem_type;
    const ArrayOperations* m_operations;
    const TypeOperations* m_elem_operations;

    Array(const std::string& name, enum ArrayType arr_type,
          enum AddressingType addr_type, const Type* elem_type,
          const ArrayOperations* operations,
          const TypeOperations* elem_operations,
          default_construct_fn fn)
        : Type{ValueKind::Array, name, fn},
          m_array_type(arr_type),
          m_addressing_type(addr_type),
          m_elem_type(elem_type),
          m_operations(operations),
          m_elem_operations(elem_operations) {}

    template <typename T>
    static std::string getName() {
        if constexpr (std::is_array_v<T>) {
            return "array";
        } else if constexpr (is_std_array_v<T>) {
            return "std::array";
        } else if constexpr (is_vector_v<T>) {
            return "std::vector";
        } else if constexpr (is_std_list_v<T>) {
            return "std::list";
        }

        return "unknown-array";
    }

    template <typename T>
    static enum ArrayType getArrayType() {
        if constexpr (std::is_array_v<T> || is_std_array_v<T>) {
            return ArrayType::Static;
        } else {
            return ArrayType::Dynamic;
        }
    }

    template <typename T>
    static enum AddressingType getAddressingType() {
        if constexpr (std::is_array_v<T>) {
            return AddressingType::Random;
        } else {
            return std::is_same_v<typename std::iterator_traits<
                                      typename T::iterator>::iterator_category,
                                  std::random_access_iterator_tag>
                       ? AddressingType::Random
                       : AddressingType::Forward;
        }
    }
};

}  // namespace nickel::refl