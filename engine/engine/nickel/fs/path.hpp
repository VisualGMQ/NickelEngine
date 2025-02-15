#pragma once

#include <filesystem>
#include "spdlog/spdlog.h"
#include "spdlog/fmt/ostr.h"

namespace nickel {

class Path {
public:
    Path(const std::string& path);
    Path(const char* path);
    Path(std::string_view path);
    Path(const Path&) = default;
    Path(Path&&) = default;

    void Clear();
    std::string ToString() const;
    Path& RemoveFilename();
    Path Filename() const;
    bool HasFilename() const;
    Path Extension() const;
    bool HasExtension() const;
    Path RootName() const;
    Path RootDirectory() const;
    Path RootPath() const;
    bool HasRootDirectory() const;
    bool HasRootPath() const;
    bool HasRootName() const;
    bool IsAbsolute() const;
    bool IsRelative() const;
    Path ParentPath() const;
    bool HasParentPath() const;
    bool IsEmpty() const;
    Path& ReplaceFilename(const Path& path);
    Path& ReplaceExtension(const Path& extension);

    Path operator/(const Path&) const noexcept;
    Path& operator/=(const Path&) noexcept;

    bool operator ==(const Path&) const noexcept;
    bool operator !=(const Path&) const noexcept;

    const std::filesystem::path& GetUnderlyingPath() const noexcept;

private:
    std::filesystem::path m_path;

    explicit Path(const std::filesystem::path& path);
};

std::ostream& operator<<(std::ostream& os, const Path& path);

}  // namespace nickel

// for spdlog output
template <> struct fmt::formatter<nickel::Path> : fmt::ostream_formatter {};

namespace std {
template <>
struct hash<nickel::Path> {
    size_t operator()(const nickel::Path& p) const noexcept {
        auto hasher = hash<std::filesystem::path>();
        return hasher(p.GetUnderlyingPath());
    }
};
}  // namespace std