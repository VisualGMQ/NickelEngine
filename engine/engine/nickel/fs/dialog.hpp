#pragma once

#include <string>
#include <vector>
#include <memory>

namespace nickel {

class FileDialog {
public:
    static FileDialog CreateOpenFileDialog();
    static FileDialog CreateSaveDialog();
    static FileDialog CreateOpenFolderDialog();

    FileDialog& SetTitle(const std::string&);
    FileDialog& SetAcceptButtonText(const std::string&);
    FileDialog& SetCancelButtonText(const std::string&);
    FileDialog& AllowMultipleSelect(bool);
    FileDialog& AddFilter(const std::string& name, const std::string& pattern);
    FileDialog& SetDefaultFolder(const std::string&);
    FileDialog& Open();

    const std::vector<std::string>& GetSelected() const;

private:
    class Impl;
    std::shared_ptr<Impl> m_impl;

    FileDialog(std::shared_ptr<Impl>);
};

}  // namespace nickel