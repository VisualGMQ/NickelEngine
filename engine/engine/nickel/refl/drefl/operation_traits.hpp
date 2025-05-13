#pragma once

#include "nickel/common/log.hpp"
#include "nickel/refl/drefl/exception.hpp"
#include "nickel/refl/util/variable_traits.hpp"
#include <tuple>
#include <type_traits>

namespace nickel::refl {

struct TypeOperations final {
    using destroy_fn = void(void*);
    using copy_construct_fn = void*(void*);
    using steal_construct_fn = void*(void*);
    using copy_assignment_fn = void(void*, void*);
    using steal_assignment_fn = void(void*, void*);

    destroy_fn* destroy = emptyDestroy;
    copy_construct_fn* copy_construct = emptyCopy;
    steal_construct_fn* steal_construct = emptySteal;
    copy_assignment_fn* copy_assignment = emptyCopyAssign;
    steal_assignment_fn* steal_assignment = emptyStealAssign;

    static TypeOperations null;

private:
    static void emptyDestroy(void*) {}
    static void* emptyCopy(void*) { return nullptr; }
    static void* emptySteal(void*) { return nullptr; }
    static void emptyCopyAssign(void*, void*) {}
    static void emptyStealAssign(void*, void*) {}
};

template <typename T>
struct TypeOperationTraits {
    static void Destroy(void* elem) {
        if constexpr (std::is_destructible_v<T>) {
            if constexpr (std::is_array_v<T>){
                delete[] (T*)(elem);
            } else {
                delete (T*)(elem);
            }
        } else {
            LOGE("type don't support destruct");
        }
    }

    static void* CopyConstruct(void* elem) {
        if constexpr (std::is_copy_constructible_v<T>) {
            return new T{*(const T*)elem};
        } else {
            LOGE("type don't support copy construct");
            return nullptr;
        }
    }

    static void* StealConstruct(void* elem) {
        if constexpr (std::is_move_constructible_v<T>) {
            return new T{std::move(*(T*)elem)};
        } else {
            LOGE("type don't support move construct");
            return nullptr;
        }
    }

    static void CopyAssignment(void* dst, void* src) {
        if constexpr (std::is_copy_assignable_v<T>) {
            *(T*)(dst) = *(const T*)(src);
        } else {
            LOGE("type don't support copy assignment");
        }
    }

    static void StealAssignment(void* dst, void* src) {
        if constexpr (std::is_move_assignable_v<T>) {
            *(T*)(dst) = std::move(*(T*)(src));
        } else {
            LOGE("type don't support copy assignment");
        }
    }

    static auto& GetOperations() {
        using traits = TypeOperationTraits<T>;

        static TypeOperations operations = {
            Destroy, CopyConstruct, StealConstruct,
            CopyAssignment, StealAssignment};
        return operations;
    }
};

}  // namespace nickel::refl