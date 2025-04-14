#include "nickel/common/common.hpp"
#include "nickel/common/log.hpp"
#include "nickel/context.hpp"

namespace nickel {

std::vector<char> ReadWholeFile(const Path& filename) {
    auto storage = Context::GetInst().GetStorageManager().AcquireLocalStorage();
    uint64_t size = storage->GetFileSize(filename);
    if (size > 0) {
        storage->WaitStorageReady();
        return storage->ReadStorageFile(filename);
    }

    LOGW("load ", filename, " failed: file is empty or not found");
    return {};
}

}  // namespace nickel