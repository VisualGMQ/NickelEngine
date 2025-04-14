#include "nickel/fs/storage.hpp"
#include "nickel/common/internal/sdl_call.hpp"
#include "nickel/common/log.hpp"
#include "nickel/common/macro.hpp"
#include "nickel/internal/pch.hpp"

namespace nickel {
class StorageImpl {
public:
    enum class Type {
        Title,
        User,
        Local,
    };

    StorageImpl(Type type, const std::string& param1,
                const std::string& param2);
    StorageImpl(const StorageImpl&) = delete;
    StorageImpl(StorageImpl&&) = delete;
    StorageImpl& operator=(const StorageImpl&) = delete;
    StorageImpl& operator=(StorageImpl&&) = delete;
    ~StorageImpl();

    uint64_t GetFileSize(const Path& filename) const;
    uint64_t GetRemainingSpacing() const;
    std::vector<char> ReadStorageFile(const Path& filename) const;
    bool ReadStorageFile(const Path& filename, void* buffer,
                         uint64_t size) const;
    bool WriteStorageFile(const Path& filename, const void* buffer,
                          uint64_t size) const;
    bool CopyFile(const Path& old_path, const Path& new_path) const;
    bool RemovePath(const Path& path) const;
    bool MovePath(const Path& old_path, const Path& new_path) const;
    bool IsStorageReady() const;
    void WaitStorageReady(
        uint32_t timeout = std::numeric_limits<uint32_t>::max()) const;
    void EnumerateStorage(const Path& path, StorageEnumerator enumerator);

private:
    SDL_Storage* m_storage = nullptr;

    static SDL_EnumerationResult sdlEnumerateCallback(void* userdata,
                                                      const char* dirname,
                                                      const char* fname);
};

StorageImpl::StorageImpl(Type type, const std::string& param1,
                         const std::string& param2) {
    switch (type) {
        case Type::Title:
            m_storage = SDL_OpenTitleStorage(param1.c_str(), {});
            break;
        case Type::User:
            m_storage = SDL_OpenUserStorage(param1.c_str(), param2.c_str(), {});
            break;
        case Type::Local:
            m_storage = SDL_OpenFileStorage(param1.c_str());
            break;
    }

    NICKEL_RETURN_IF_FALSE(m_storage);
}

StorageImpl::~StorageImpl() {
    SDL_CALL(SDL_CloseStorage(m_storage));
}

bool StorageImpl::CopyFile(const Path& old_path, const Path& new_path) const {
    bool success = SDL_CopyStorageFile(m_storage, old_path.ToString().c_str(),
                                       new_path.ToString().c_str());
    if (!success) {
        LOGW("copy file {} to {} failed: {}", old_path, new_path,
             SDL_GetError());
    }
    return success;
}

uint64_t StorageImpl::GetFileSize(const Path& filename) const {
    Uint64 length = 0;
    SDL_CALL(SDL_GetStorageFileSize(m_storage, filename.ToString().c_str(),
                                    &length));
    return static_cast<uint64_t>(length);
}

uint64_t StorageImpl::GetRemainingSpacing() const {
    return static_cast<uint64_t>(SDL_GetStorageSpaceRemaining(m_storage));
}

std::vector<char> StorageImpl::ReadStorageFile(
    const Path& filename) const {
    uint64_t file_size = GetFileSize(filename);
    if (file_size == 0) {
        return {};
    }

    std::vector<char> buffer;
    buffer.resize(file_size);
    SDL_CALL(SDL_ReadStorageFile(m_storage, filename.ToString().c_str(),
                                 buffer.data(), file_size));
    return buffer;
}

bool StorageImpl::ReadStorageFile(const Path& filename, void* buffer,
                                  uint64_t size) const {
    if (!SDL_ReadStorageFile(m_storage, filename.ToString().c_str(), buffer,
                             size)) {
        LOGE("read file {} failed: {}", filename, SDL_GetError());
        return false;
    }
    return true;
}

bool StorageImpl::WriteStorageFile(const Path& filename,
                                   const void* buffer, uint64_t size) const {
    if (!SDL_WriteStorageFile(m_storage, filename.ToString().c_str(), buffer,
                              size)) {
        LOGE("write file {} failed: {}", filename, SDL_GetError());
        return false;
    }
    return true;
}

bool StorageImpl::IsStorageReady() const {
    return SDL_StorageReady(m_storage);
}

void StorageImpl::WaitStorageReady(uint32_t timeout) const {
    constexpr uint32_t wait_interval = 10;
    uint32_t cur_time = 0;
    while (!IsStorageReady() && cur_time < timeout) {
        cur_time += wait_interval;
        SDL_Delay(wait_interval);
    }
}

void StorageImpl::EnumerateStorage(const Path& path,
                                   StorageEnumerator enumerator) {
    SDL_EnumerateStorageDirectory(m_storage, path.ToString().c_str(),
                                  sdlEnumerateCallback, &enumerator);
}

SDL_EnumerationResult StorageImpl::sdlEnumerateCallback(void* userdata,
                                                        const char* dirname,
                                                        const char* fname) {
    if (!userdata) {
        return SDL_ENUM_FAILURE;
    }
    StorageEnumerator* enumerator = static_cast<StorageEnumerator*>(userdata);
    StorageEnumerateBehavior behavior = (*enumerator)(dirname, fname);

    switch (behavior) {
        case StorageEnumerateBehavior::Continue:
            return SDL_ENUM_CONTINUE;
        case StorageEnumerateBehavior::Success:
            return SDL_ENUM_SUCCESS;
        case StorageEnumerateBehavior::Failed:
            return SDL_ENUM_FAILURE;
    }

    NICKEL_CANT_REACH();
    return {};
}

bool StorageImpl::RemovePath(const Path& path) const {
    if (!SDL_RemoveStoragePath(m_storage, path.ToString().c_str())) {
        LOGE("remove path {} failed: {}", path, SDL_GetError());
        return false;
    }
    return true;
}

bool StorageImpl::MovePath(const Path& old_path, const Path& new_path) const {
    if (!SDL_RenameStoragePath(m_storage, old_path.ToString().c_str(),
                               new_path.ToString().c_str())) {
        LOGE("move path {} to {} failed: {}", old_path, new_path,
             SDL_GetError());
        return false;
    }
    return true;
}

SDL_EnumerationResult EnumerateCallback(void*, const char* dirname,
                                        const char* fname) {
    LOGI("dirname: {}, fname: {}", dirname, fname);
    return SDL_ENUM_CONTINUE;
}

namespace internal {

void CommonStorageBehavior::Initialize(StorageImpl* impl) {
    m_impl = impl;
}

bool CommonStorageBehavior::IsStorageReady() const {
    return m_impl->IsStorageReady();
}

void CommonStorageBehavior::WaitStorageReady(uint32_t timeout) const {
    return m_impl->WaitStorageReady(timeout);
}

void CommonStorageBehavior::EnumerateStorage(const Path& path,
                                             StorageEnumerator enumerator) {
    return m_impl->EnumerateStorage(path, enumerator);
}

void ReadOnlyStorageBehavior::Initialize(StorageImpl* impl) {
    m_impl = impl;
}

uint64_t ReadOnlyStorageBehavior::GetFileSize(
    const Path& filename) const {
    return m_impl->GetFileSize(filename);
}

uint64_t ReadOnlyStorageBehavior::GetRemainingSpacing() const {
    return m_impl->GetRemainingSpacing();
}

std::vector<char> ReadOnlyStorageBehavior::ReadStorageFile(
    const Path& filename) const {
    return m_impl->ReadStorageFile(filename);
}

bool ReadOnlyStorageBehavior::ReadStorageFile(const Path& filename,
                                              void* buffer,
                                              uint64_t size) const {
    return m_impl->ReadStorageFile(filename, buffer, size);
}

void WritableStorageBehavior::Initialize(StorageImpl* impl) {
    m_impl = impl;
}

bool WritableStorageBehavior::WriteStorageFile(const std::string& filename,
                                               const void* buffer,
                                               uint64_t size) const {
    return m_impl->WriteStorageFile(filename, buffer, size);
}

bool WritableStorageBehavior::CopyFile(const Path& old_path,
                                       const Path& new_path) const {
    return m_impl->CopyFile(old_path, new_path);
}

bool WritableStorageBehavior::RemovePath(const Path& path) const {
    return m_impl->RemovePath(path);
}

bool WritableStorageBehavior::MovePath(const Path& old_path,
                                       const Path& new_path) const {
    return m_impl->MovePath(old_path, new_path);
}

}  // namespace internal

AppReadOnlyStorage::AppReadOnlyStorage()
    : m_impl(std::make_unique<StorageImpl>(StorageImpl::Type::Title, "", "")) {
    CommonStorageBehavior::Initialize(m_impl.get());
    ReadOnlyStorageBehavior::Initialize(m_impl.get());
}

AppReadOnlyStorage::AppReadOnlyStorage(const Path& root_path)
    : m_impl(std::make_unique<StorageImpl>(StorageImpl::Type::Title,
                                           root_path.ToString().c_str(), "")) {
    CommonStorageBehavior::Initialize(m_impl.get());
    ReadOnlyStorageBehavior::Initialize(m_impl.get());
}

AppReadOnlyStorage::~AppReadOnlyStorage() {}

LocalStorage::LocalStorage()
    : m_impl(std::make_unique<StorageImpl>(StorageImpl::Type::Local, "", "")) {
    CommonStorageBehavior::Initialize(m_impl.get());
    ReadOnlyStorageBehavior::Initialize(m_impl.get());
    WritableStorageBehavior::Initialize(m_impl.get());
}

LocalStorage::LocalStorage(const Path& base_path)
    : m_impl(std::make_unique<StorageImpl>(StorageImpl::Type::Local,
                                           base_path.ToString().c_str(), "")) {
    CommonStorageBehavior::Initialize(m_impl.get());
    ReadOnlyStorageBehavior::Initialize(m_impl.get());
    WritableStorageBehavior::Initialize(m_impl.get());
}

LocalStorage::~LocalStorage() {}

UserStorage::UserStorage()
    : m_impl(std::make_unique<StorageImpl>(StorageImpl::Type::User, "", "")) {
    CommonStorageBehavior::Initialize(m_impl.get());
    ReadOnlyStorageBehavior::Initialize(m_impl.get());
    WritableStorageBehavior::Initialize(m_impl.get());
}

UserStorage::UserStorage(const std::string& org, const std::string& app)
    : m_impl(std::make_unique<StorageImpl>(StorageImpl::Type::User, org.c_str(),
                                           app.c_str())) {
    CommonStorageBehavior::Initialize(m_impl.get());
    ReadOnlyStorageBehavior::Initialize(m_impl.get());
    WritableStorageBehavior::Initialize(m_impl.get());
}

UserStorage::~UserStorage() {}

StorageManager::StorageManager(const std::string& org, const std::string& app)
    : m_user_storage(org, app) {}

AppReadOnlyStorage& StorageManager::GetAppReadOnlyStorage() {
    return m_app_storage;
}

const AppReadOnlyStorage& StorageManager::GetAppReadOnlyStorage() const {
    return m_app_storage;
}

UserStorage& StorageManager::GetUserStorage() {
    return m_user_storage;
}

const UserStorage& StorageManager::GetUserStorage() const {
    return m_user_storage;
}

std::unique_ptr<LocalStorage> StorageManager::AcquireLocalStorage(
    const Path& base_path) {
    if (base_path.IsEmpty()) {
        return std::make_unique<LocalStorage>();
    }
    return std::make_unique<LocalStorage>(base_path);
}

}  // namespace nickel