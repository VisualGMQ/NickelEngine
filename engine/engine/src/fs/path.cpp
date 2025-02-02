#include "nickel/fs/path.hpp"

namespace nickel {

Path::Path(const std::string& path)
    : m_path{path, std::filesystem::path::generic_format} {}

Path::Path(const char* path)
    : m_path{path, std::filesystem::path::generic_format} {}

Path::Path(std::string_view path)
    : m_path{path, std::filesystem::path::generic_format} {}

void Path::Clear() {
    m_path.clear();
}

std::string Path::ToString() const {
    return m_path.string();
}

Path& Path::RemoveFilename() {
    m_path.remove_filename();
    return *this;
}

bool Path::HasFilename() const {
    return m_path.has_filename();
}

bool Path::HasExtension() const {
    return m_path.has_extension();
}

Path Path::RootName() const {
    return Path{m_path.root_name()};
}

Path Path::RootDirectory() const {
    return Path{m_path.root_directory()};
}

Path Path::RootPath() const {
    return Path{m_path.root_path()};
}

bool Path::HasRootDirectory() const {
    return m_path.has_root_directory();
}

bool Path::HasRootPath() const {
    return m_path.has_root_path();
}

bool Path::HasRootName() const {
    return m_path.has_root_name();
}

bool Path::IsAbsolute() const {
    return m_path.is_absolute();
}

bool Path::IsRelative() const {
    return m_path.is_relative();
}

Path Path::ParentPath() const {
    return Path{m_path.parent_path()};
}

bool Path::HasParentPath() const {
    return m_path.has_parent_path();
}

bool Path::IsEmpty() const {
    return m_path.empty();
}

Path& Path::ReplaceFilename(const Path& path) {
    m_path.replace_filename(path.m_path);
    return *this;
}

Path& Path::ReplaceExtension(const Path& extension) {
    m_path.replace_extension(extension.m_path);
    return *this;
}

Path Path::operator/(const Path& o) const noexcept {
    return Path{m_path / o.m_path};
}

Path& Path::operator/=(const Path& o) noexcept {
    m_path /= o.m_path;
    return *this;
}

bool Path::operator==(const Path& o) const noexcept {
    return m_path == o.m_path;
}

bool Path::operator!=(const Path& o) const noexcept {
    return !(*this == o);
}

const std::filesystem::path& Path::GetUnderlyingPath() const noexcept {
    return m_path;
}

Path Path::Filename() const {
    return Path{m_path.filename()};
}

Path Path::Extension() const {
    return Path{m_path.extension()};
}

Path::Path(const std::filesystem::path& path)
    : m_path{path, std::filesystem::path::generic_format} {}

}  // namespace nickel

std::ostream& operator<<(std::ostream& os, const nickel::Path& path) {
    os << path.ToString();
    return os;
}
