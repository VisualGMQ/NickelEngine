#pragma once

#include <array>
#include <list>
#include <vector>

namespace nickel::refl {

struct ArrayOperations final {
    using get_fn = void*(size_t, void*, bool);
    using push_back_fn = bool(const void*, void*);
    using pop_back_fn = bool(void*);
    using back_fn = void*(void*, bool);
    using resize_fn = bool(size_t, void*);
    using size_fn = size_t(const void*);
    using capacity_fn = size_t(const void*);
    using insert_fn = bool(size_t, const void*, void*);

    static ArrayOperations null;

    get_fn* get = emptyGet;
    push_back_fn* push_back = emptyPushBack;
    back_fn* back = emptyBack;
    pop_back_fn* pop_back = emptyPopBack;
    resize_fn* resize = emptyResize;
    size_fn* size = emptySize;
    capacity_fn* capacity = emptyCapacity;
    insert_fn* insert = emptyInsert;

private:
    static void* emptyGet(size_t, void*, bool) { return nullptr; }

    static bool emptyPushBack(const void*, void*) { return false; }

    static bool emptyPopBack(void*) { return false; }

    static void* emptyBack(void*, bool) { return nullptr; }

    static bool emptyResize(size_t, void*) { return false; }

    static size_t emptySize(const void*) { return 0; }

    static size_t emptyCapacity(const void*) { return 0; }

    static bool emptyInsert(size_t, const void*, void*) { return false; }
};

template <typename T>
struct array_operation_traits;

// sperialize for T[N]
template <typename T, size_t N>
struct array_operation_traits<T[N]> final {
    using type = T[N];

    static void* Get(size_t idx, void* array, bool is_const) {
        if (is_const) {
            return (void*)((const T*)array + idx);
        } else {
            return (T*)array + idx;
        }
    }

    static bool PushBack(const void*, void*) { return false; }

    static bool PopBack(void*) { return false; }

    static void* Back(void*, bool) { return nullptr; }

    static bool Resize(size_t, void*) { return false; }

    static size_t Size(const void*) { return N; }

    static size_t Capacity(const void*) { return N; }

    static bool Insert(size_t, const void*, void*) { return false; }

    static auto& GetOperations() {
        using traits = array_operation_traits<type>;

        static ArrayOperations operations = {
            traits::Get,      traits::PushBack, traits::Back,
            traits::PopBack, traits::Resize,    traits::Size,
            traits::Capacity, traits::Insert};
        return operations;
    }
};

// sperialize for std::array
template <typename T, size_t N>
struct array_operation_traits<std::array<T, N>> final {
    using type = std::array<T, N>;

    static void* Get(size_t idx, void* array, bool is_const) {
        if (is_const) {
            return (void*)&((const type*)array)->operator[](idx);
        } else {
            return &((type*)array)->operator[](idx);
        }
    }

    static bool PushBack(const void*, void*) { return false; }

    static bool PopBack(void*) { return false; }

    static void* Back(void*, bool) { return nullptr; }

    static bool Resize(size_t, void*) { return false; }

    static size_t Size(const void*) { return N; }

    static size_t Capacity(const void*) { return N; }

    static bool Insert(size_t, const void*, void*) { return false; }

    static auto& GetOperations() {
        using traits = array_operation_traits<type>;

        static ArrayOperations operations = {
            traits::Get,      traits::PushBack, traits::Back,
            traits::PopBack, traits::Resize,    traits::Size,
            traits::Capacity, traits::Insert};
        return operations;
    }
};

// specialize for std::vector
template <typename T, typename Alloc>
struct array_operation_traits<std::vector<T, Alloc>> final {
    using type = std::vector<T, Alloc>;

    static void* Get(size_t idx, void* array, bool is_const) {
        if (is_const) {
            return (void*)&((const type*)array)->operator[](idx);
        } else {
            return &((type*)array)->operator[](idx);
        }
    }

    static bool PushBack(const void* elem, void* array) {
        ((type*)array)->push_back(*((T*)elem));
        return true;
    }

    static bool PopBack(void* array) {
        ((type*)array)->pop_back();
        return true;
    }

    static void* Back(void* array, bool is_const) {
        if (is_const) {
            return (void*)&((const type*)array)->back();
        } else {
            return &((type*)array)->back();
        }
    }

    static bool Resize(size_t idx, void* array) {
        ((type*)array)->resize(idx);
        return true;
    }

    static size_t Size(const void* array) { return ((type*)array)->size(); }

    static size_t Capacity(const void* array) {
        return ((type*)array)->capacity();
    }

    static bool Insert(size_t idx, const void* elem, void* array) {
        type* arr = (type*)array;
        auto it = arr->insert(arr->begin() + idx, *((T*)elem));
        return it != arr->end();
    }

    static auto& GetOperations() {
        using traits = array_operation_traits<type>;

        static ArrayOperations operations = {
            traits::Get,      traits::PushBack, traits::Back,
            traits::PopBack, traits::Resize,    traits::Size,
            traits::Capacity, traits::Insert};
        return operations;
    }
};

// specialize for std::list
template <typename T, typename Alloc>
struct array_operation_traits<std::list<T, Alloc>> final {
    using type = std::list<T, Alloc>;

    static void* Get(size_t idx, void* array, bool is_const) {
        if (is_const) {
            auto arr = (const type*)array;
            auto it = arr->begin();
            while (idx > 0) {
                it++;
                idx--;
            }
            return (void*)&(*it);
        } else {
            auto arr = (type*)array;
            auto it = arr->begin();
            while (idx > 0) {
                it++;
                idx--;
            }
            return &(*it);
        }
    }

    static bool PushBack(const void* elem, void* array) {
        ((type*)array)->push_back(*((T*)elem));
        return true;
    }

    static bool PopBack(void* array) {
        ((type*)array)->pop_back();
        return true;
    }

    static void* Back(void* array, bool is_const) {
        if (is_const) {
            return (void*)&((const type*)array)->back();
        } else {
            return &((type*)array)->back();
        }
    }

    static bool Resize(size_t idx, void* array) {
        ((type*)array)->resize(idx);
        return true;
    }

    static size_t Size(const void* array) { return ((type*)array)->size(); }

    static size_t Capacity(const void* array) { return ((type*)array)->size(); }

    static bool Insert(size_t idx, const void* elem, void* array) {
        type* arr = (type*)array;
        auto it = arr->begin();
        while (idx > 0) {
            idx--;
            it++;
        }
        auto result = arr->insert(it, *((T*)elem));
        return result != arr->end();
    }

    static auto& GetOperations() {
        using traits = array_operation_traits<type>;

        static ArrayOperations operations = {
            traits::Get,      traits::PushBack, traits::Back,
            traits::PopBack, traits::Resize,    traits::Size,
            traits::Capacity, traits::Insert};
        return operations;
    }
};

}  // namespace nickel::refl