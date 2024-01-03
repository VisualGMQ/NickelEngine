#include "dialog.hpp"

// TODO: add file dialog under ubuntu

/*
WARN: Don't move this to .hpp! windows.h defined some macro that will make
project chaos!
*/
#ifdef _WIN32
#include <shlobj.h>
#include <windows.h>
#include <commdlg.h>

#endif

std::string concatExtensions(const std::vector<std::string>& extensions) {
    std::string extension;
    for (auto& ext : extensions) {
        extension += ext + ";";
    }
    extension.pop_back();
    return extension;
}

std::vector<std::filesystem::path> OpenFileDialog(
    const std::string& title, const std::vector<std::string>& extensions) {
#ifdef _WIN32
    OPENFILENAME ofn;
    char szFile[MAX_PATH] = {0};
    auto extension = concatExtensions(extensions);

    ZeroMemory(&ofn, sizeof(ofn));
    ofn.lStructSize = sizeof(ofn);
    ofn.hwndOwner = NULL;
    ofn.lpstrFile = szFile;
    ofn.nMaxFile = sizeof(szFile);
    ofn.lpstrInitialDir = NULL;
    ofn.Flags = OFN_PATHMUSTEXIST | OFN_FILEMUSTEXIST | OFN_NOCHANGEDIR |
                OFN_ALLOWMULTISELECT | OFN_EXPLORER | OFN_ENABLESIZING;
    ofn.lpstrFilter = extension.c_str();
    ofn.lpstrTitle = title.c_str();

    if (GetOpenFileName(&ofn) == TRUE) {
        return {std::filesystem::path{szFile}};
    } else {
        return {};
    }
#else
    return {};
#endif
}

std::filesystem::path OpenDirDialog(const std::string& title) {
#ifdef _WIN32
    BROWSEINFO bi = {0};
    bi.hwndOwner = NULL;
    bi.ulFlags = BIF_RETURNONLYFSDIRS | BIF_NEWDIALOGSTYLE;
    bi.lpszTitle = title.c_str();

    LPITEMIDLIST pidl = SHBrowseForFolder(&bi);
    std::filesystem::path result;
    if (pidl != NULL) {
        char folderPath[MAX_PATH] = {0};
        SHGetPathFromIDList(pidl, folderPath);
        result = folderPath;

        CoTaskMemFree(pidl);
    }

    return result;
#else
    return {};
#endif
}

std::filesystem::path SaveFileDialog(
    const std::string& title, const std::vector<std::string>& extensions) {
#ifdef _WIN32
    OPENFILENAME ofn;

    auto extension = concatExtensions(extensions);

    char szFile[MAX_PATH] = {0};
    ZeroMemory(&ofn, sizeof(ofn));
    ofn.lStructSize = sizeof(ofn);
    ofn.hwndOwner = NULL;
    ofn.lpstrFile = szFile;
    ofn.nMaxFile = sizeof(szFile);
    ofn.lpstrInitialDir = NULL;
    ofn.Flags =
        OFN_PATHMUSTEXIST | OFN_NOCHANGEDIR | OFN_EXPLORER | OFN_ENABLESIZING;
    ofn.lpstrFilter = extension.c_str();
    ofn.lpstrTitle = title.c_str();

    if (GetSaveFileName(&ofn) == TRUE) {
        return {std::filesystem::path{szFile}};
    } else {
        return {};
    }
#else
    return {};
#endif
}

void ShowSimpleMessageBox(MessageBoxType type, const std::string& title, const std::string& content) {
    SDL_ShowSimpleMessageBox(static_cast<SDL_MessageBoxFlags>(type),
                             title.c_str(), content.c_str(), nullptr);
}