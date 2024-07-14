#pragma once

#include "common/assert.hpp"
#include "common/ecs.hpp"
#include "common/singlton.hpp"
#include "common/typeid_generator.hpp"
#include "common/data_id.hpp"
#include "common/data_pool.hpp"

namespace nickel {

template <typename T>
struct Handle {
public:
    struct Hash final {
        size_t operator()(const Handle& k) const {
            return std::hash<uint32_t>{}(DataID2Num(k.id_));
        }
    };

    struct Eq final {
        size_t operator()(const Handle& h1, const Handle& h2) const {
            return h1 == h2;
        }
    };

    template <typename... Args>
    static Handle Create(Args&&... args) {
        DataID id =
            DataPool::Instance().Emplace<T>(std::forward<Args>(args)...);
        return Handle{id};
    }

    Handle() : id_{InvalidDataID} {}

    /**
     * @brief Construct a new Ref object by id, don't inc refcount
     *
     * @param id
     */
    explicit Handle(DataID id) : id_{id} {
        DataPool::Instance().IncRefCount<T>(id_);
    }

    Handle(const Handle& ref);
    Handle(Handle&& ref);
    Handle& operator=(const Handle<T>& ref);
    Handle& operator=(Handle<T>&& ref);
    ~Handle();

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

    bool operator==(const Handle& o) const {
        return id_ == o.id_;
    }

    bool operator!=(const Handle& o) const {
        return !(*this == o);
    }

private:
    DataID id_;
};

template <typename T>
bool Handle<T>::Valid() const {
    return id_ != InvalidDataID && DataPool::Instance().Exists<T>(id_);
}

template <typename T>
T* Handle<T>::GetData() {
    return DataPool::Instance().Get<T>(id_);
}

template <typename T>
const T* Handle<T>::GetDataConst() const {
    return DataPool::Instance().Get<T>(id_);
}

template <typename T>
T* Handle<T>::GetData() const {
    return DataPool::Instance().Get<T>(id_);
}

template <typename T>
Handle<T>::Handle(const Handle& o) : id_{o.id_} {
    id_ = o.id_;
    DataPool::Instance().IncRefCount<T>(id_);
}

template <typename T>
Handle<T>::Handle(Handle&& o) : id_{o.id_} {
    o.id_ = InvalidDataID;
}

template <typename T>
Handle<T>& Handle<T>::operator=(const Handle<T>& o) {
    if (&o != this) {
        id_ = o.id_;
        DataPool::Instance().IncRefCount<T>(id_);
    }
    return *this;
}

template <typename T>
Handle<T>& Handle<T>::operator=(Handle<T>&& o) {
    if (&o != this) {
        id_ = o.id_;
        o.id_ = InvalidDataID;
    }
    return *this;
}

template <typename T>
Handle<T>::~Handle() {
    if (DataPool::HasInstance()) {
        DataPool::Instance().DecRefCount<T>(id_);
    }
}

}  // namespace nickel
