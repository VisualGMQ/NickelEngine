#include "nickel/common/common.hpp"
#include "nickel/common/log.hpp"

#include <iterator>

namespace nickel {

std::vector<char> ReadWholeFile(const Path& filename) {
    std::ifstream file(filename.GetUnderlyingPath(), std::ios::binary);

    if (file.fail()) {
        LOGW("file {} load failed", filename);
        return {};
    }

    return std::vector<char>{std::istreambuf_iterator(file),
                             std::istreambuf_iterator<char>()};
}

}  // namespace nickel