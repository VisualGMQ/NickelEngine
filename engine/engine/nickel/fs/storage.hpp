#pragma once
#include "nickel/common/memory/memory.hpp"
#include "nickel/fs/path.hpp"

#include <vector>

namespace nickel {

class StorageImpl;

enum class StorageEnumerateBehavior {
    Continue,
    Success,
    Failed,
};

using StorageEnumerator =
    std::function<StorageEnumerateBehavior(std::string_view, std::string_view)>;

namespace internal {

class CommonStorageBehavior {
public:
    void Initialize(StorageImpl*);

    bool IsStorageReady() const;
    void WaitStorageReady(
        uint32_t timeout = std::numeric_limits<uint32_t>::max()) const;
    void EnumerateStorage(const Path& path, StorageEnumerator enumerator);

private:
    StorageImpl* m_impl{};
};

class ReadOnlyStorageBehavior {
public:
    void Initialize(StorageImpl*);
    uint64_t GetFileSize(const Path& filename) const;
    uint64_t GetRemainingSpacing() const;
    std::vector<char> ReadStorageFile(const Path& filename) const;
    bool ReadStorageFile(const Path& filename, void* buffer,
                         uint64_t size) const;

private:
    StorageImpl* m_impl{};
};

class WritableStorageBehavior {
public:
    void Initialize(StorageImpl*);
    bool WriteStorageFile(const std::string& filename, const void* buffer,
                          uint64_t size) const;
    bool CopyFile(const Path& old_path, const Path& new_path) const;
    bool RemovePath(const Path& path) const;
    bool MovePath(const Path& old_path, const Path& new_path) const;

private:
    StorageImpl* m_impl{};
};

}  // namespace internal

class AppReadOnlyStorage : public internal::CommonStorageBehavior,
                           public internal::ReadOnlyStorageBehavior {
public:
    AppReadOnlyStorage();
    AppReadOnlyStorage(const Path& root_path);
    ~AppReadOnlyStorage();

private:
    std::unique_ptr<StorageImpl> m_impl{};
};

class LocalStorage : public internal::CommonStorageBehavior,
                     public internal::ReadOnlyStorageBehavior,
                     public internal::WritableStorageBehavior {
public:
    LocalStorage();
    explicit LocalStorage(const Path& base_path);
    ~LocalStorage();

private:
    std::unique_ptr<StorageImpl> m_impl;
};

class UserStorage : public internal::CommonStorageBehavior,
                    public internal::ReadOnlyStorageBehavior,
                    public internal::WritableStorageBehavior {
public:
    UserStorage();
    explicit UserStorage(const std::string& org, const std::string& app);
    ~UserStorage();

private:
    std::unique_ptr<StorageImpl> m_impl;
};

class StorageManager {
public:
    StorageManager(const std::string& org, const std::string& app);
    AppReadOnlyStorage& GetAppReadOnlyStorage();
    const AppReadOnlyStorage& GetAppReadOnlyStorage() const;
    UserStorage& GetUserStorage();
    const UserStorage& GetUserStorage() const;

    std::unique_ptr<LocalStorage> AcquireLocalStorage(
        const Path& base_path = "");

private:
    AppReadOnlyStorage m_app_storage;
    UserStorage m_user_storage;
};

}  // namespace nickel
