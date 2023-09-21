#pragma once

#include "core/handle.hpp"

namespace nickel {

template <typename T>
class Manager {
public:
    Manager() = default;
    Manager(const Manager&) = delete;
    Manager operator=(const Manager&) = delete;

    void Destroy(Handle<T> handle) { datas_.erase(handle); }

    const T& Get(Handle<T> handle) const {
        if (auto it = datas_.find(handle); it != datas_.end()) {
            return *it->second;
        } else {
            return T::Null;
        }
    }

    T& Get(Handle<T> handle) {
        return const_cast<T&>(std::as_const(*this).Get(handle));
    }

    bool Has(Handle<T> handle) { return datas_.find(handle) != datas_.end(); }

    std::string_view GetRootPath() const { return rootPath_; }
    void SetRootPath(const std::string& path) { rootPath_ = path; }

protected:
    void storeNewItem(Handle<T> handle, std::unique_ptr<T>&& item) {
        if (handle) {
            datas_.emplace(handle, std::move(item));
        }
    }

    std::string addRootPath(const std::string& path) const {
        return rootPath_ + path;
    }

    std::unordered_map<Handle<T>, std::unique_ptr<T>, typename Handle<T>::Hash,
                       typename Handle<T>::HashEq>
        datas_;
    std::string rootPath_ = "./";
};

template <typename T>
using ResourceManager = Manager<T>;

}  // namespace nickel