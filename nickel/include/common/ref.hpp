#pragma once

#include "common/assert.hpp"
#include "common/ecs.hpp"
#include "common/singlton.hpp"
#include "common/typeid_generator.hpp"
#include "common/data_id.hpp"

namespace nickel {

template <typename T, typename Storage>
struct Ref {
public:
    struct Hash final {
        size_t operator()(const Ref& k) const {
            return std::hash<uint32_t>{}(DataID2Num(k.id_));
        }
    };

    struct Eq final {
        size_t operator()(const Ref& h1, const Ref& h2) const {
            return h1 == h2;
        }
    };

    template <typename... Args>
    static Ref Create(Args&&... args) {
        DataID id =
            Storage::Instance().template Emplace<T>(std::forward<Args>(args)...);
        return Ref{id};
    }

    Ref() : id_{InvalidDataID} {}

    /**
     * @brief Construct a new Ref object by id, don't inc refcount
     *
     * @param id
     */
    explicit Ref(DataID id) : id_{id} {
        if (id_ != InvalidDataID) {
            Storage::Instance().template incRefcount<T>(id_);
        }
    }

    Ref(const Ref& ref);
    Ref(Ref&& ref);
    Ref& operator=(const Ref& ref);
    Ref& operator=(Ref&& ref);
    ~Ref();

    T* GetData();
    T* GetData() const;
    const T* GetDataConst() const;
    bool Valid() const;

    operator bool() const { return Valid(); }

    operator DataID() const { return id_; }

    /**
     * @brief release payload without dec refcount
     *
     * @return DataID
     */
    DataID Release() const { return id_; }

    bool operator==(const Ref& o) const {
        return id_ == o.id_;
    }

    bool operator!=(const Ref& o) const {
        return !(*this == o);
    }

private:
    DataID id_;
};

template <typename T, typename Storage>
bool Ref<T, Storage>::Valid() const {
    return id_ != InvalidDataID && Storage::Instance().template Exists<T>(*this);
}

template <typename T, typename Storage>
T* Ref<T, Storage>::GetData() {
    if (!Storage::HasInstance()) {
        return nullptr;
    }
    return Storage::Instance().template Get<T>(id_);
}

template <typename T, typename Storage>
const T* Ref<T, Storage>::GetDataConst() const {
    if (!Storage::HasInstance()) {
        return nullptr;
    }
    return Storage::Instance().template Get<T>(id_);
}

template <typename T, typename Storage>
T* Ref<T, Storage>::GetData() const {
    if (!Storage::HasInstance()) {
        return nullptr;
    }
    return Storage::Instance().template Get<T>(id_);
}

template <typename T, typename Storage>
Ref<T, Storage>::Ref(const Ref& o) : id_{o.id_} {
    id_ = o.id_;
    Storage::Instance().template incRefcount<T>(id_);
}

template <typename T, typename Storage>
Ref<T, Storage>::Ref(Ref&& o) : id_{o.id_} {
    o.id_ = InvalidDataID;
}

template <typename T, typename Storage>
Ref<T, Storage>& Ref<T, Storage>::operator=(const Ref& o) {
    if (&o != this) {
        id_ = o.id_;
        Storage::Instance().template incRefcount<T>(id_);
    }
    return *this;
}

template <typename T, typename Storage>
Ref<T, Storage>& Ref<T, Storage>::operator=(Ref&& o) {
    if (&o != this) {
        id_ = o.id_;
        o.id_ = InvalidDataID;
    }
    return *this;
}

template <typename T, typename Storage>
Ref<T, Storage>::~Ref() {
    if (Storage::HasInstance()) {
        Storage::Instance().template decRefcount<T>(id_);
    }
}

}  // namespace nickel
