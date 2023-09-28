#pragma once

#include "pch.hpp"
#include "core/singlton.hpp"

namespace nickel {

using HandleInnerIDType = uint32_t;

template <typename Tag>
class HandleIDGenerator final {
public:
    HandleInnerIDType Generate() {
        auto id = curID_;
        assert(id != 0);
        curID_++;
        return id;
    }

private:
    HandleInnerIDType curID_ = 1;
};

template <typename HandleIDGeneratorType>
class HandleIDManagerBase final
    : public Singlton<HandleIDManagerBase<HandleIDGeneratorType>, false> {
public:
    auto Generate() {
        auto id = generator_.Generate();
        sparseSet_.insert(id);
        return id;
    }

    bool Has(HandleInnerIDType id) const { return sparseSet_.contain(id); }

    void Remove(HandleInnerIDType id) { sparseSet_.remove(id); }

private:
    gecs::basic_sparse_set<HandleInnerIDType, gecs::config::PageSize> sparseSet_;
    HandleIDGeneratorType generator_;
};

template <typename T>
using HandleIDManager = HandleIDManagerBase<HandleIDGenerator<T>>;

template <typename Tag>
class Handle {
public:
    template <typename T>
    friend std::ostream& operator<<(std::ostream&, Handle<T>);

    Handle() = default;

    struct Hash final {
        size_t operator()(const Handle& k) const {
            return std::hash<HandleInnerIDType>{}(k.handle_);
        }
    };

    struct HashEq final {
        size_t operator()(const Handle& h1, const Handle& h2) const {
            return h1 == h2;
        }
    };

    static Handle Null() { return Handle{0}; }

    static Handle Create() {
        return Handle{HandleIDManager<Tag>::Instance().Generate()};
    }

    static void Destroy(Handle handle) {
        HandleIDManager<Tag>::Instance().Remove(handle.handle_);
    }

    bool IsValid() const {
        return HandleIDManager<Tag>::Instance().Has(handle_);
    }

    explicit operator HandleInnerIDType() const {
        return handle_;
    }

    explicit operator bool() const { return IsValid(); }

    bool operator==(const Handle& o) const { return handle_ == o.handle_; }

    bool operator!=(const Handle& o) const { return !(o == *this); }

    Handle& operator=(const Handle& o) = default;

    static Handle ForceCastFromIntegral(HandleInnerIDType id) {
        return Handle{id};
    }

private:
    HandleInnerIDType handle_ = 0;

    explicit Handle(HandleInnerIDType handle) : handle_(handle) {}
};

template <typename T>
std::ostream& operator<<(std::ostream& o, Handle<T> handle) {
    o << "Handle(" << handle.handle_ << ")";
    return o;
}

}  // namespace nickel
