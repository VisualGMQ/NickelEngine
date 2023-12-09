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

    bool Has(Handle<T> handle) const { return datas_.find(handle) != datas_.end(); }

    std::filesystem::path GetRootPath() const { return rootPath_; }
    void SetRootPath(const std::filesystem::path& path) { rootPath_ = path; }

    void Associate2File(Handle<T> handle, const std::string& filename) {
        if (associateFiles_.count(handle) == 0) {
            associateFiles_.emplace(handle, handle);
        } else {
            // TODO: error handling
        }
    }

    void ReleaseAll() {
        datas_.clear();
        associateFiles_.clear();
    }

    std::string_view GetFilename(Handle<T> handle) const {
        if (auto it = associateFiles_.find(handle); it != associateFiles_.end()) {
            return it->second;
        } else {
            return {};
        }
    }

protected:
    void storeNewItem(Handle<T> handle, std::unique_ptr<T>&& item) {
        if (handle) {
            datas_.emplace(handle, std::move(item));
        }
    }

    std::filesystem::path addRootPath(const std::filesystem::path& path) const {
        return rootPath_/path;
    }

    std::unordered_map<Handle<T>, std::unique_ptr<T>, typename Handle<T>::Hash,
                       typename Handle<T>::HashEq>
        datas_;
    std::unordered_map<Handle<T>, std::string, typename Handle<T>::Hash,
                       typename Handle<T>::HashEq>
        associateFiles_;
    std::filesystem::path rootPath_ = "./";
};

template <typename T>
using ResourceManager = Manager<T>;

}  // namespace nickel