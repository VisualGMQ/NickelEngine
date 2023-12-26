#pragma once

#include "nickel.hpp"
#include "efsw/efsw.hpp"
#include "context.hpp"

// Inherits from the abstract listener class, and implements the the file action
// handler
class UpdateListener : public efsw::FileWatchListener {
public:
    explicit UpdateListener(gecs::registry reg) : reg_(reg) {}

    void handleFileAction(efsw::WatchID watchid, const std::string& dir,
                          const std::string& filename, efsw::Action action,
                          std::string oldFilename) override;

private:
    gecs::registry reg_;
};

class FileWatcher final {
public:
    FileWatcher(const std::filesystem::path& path, gecs::registry reg);
    ~FileWatcher();

private:
    std::unique_ptr<efsw::FileWatcher> watcher_;
    std::unique_ptr<UpdateListener> listener_;
    efsw::WatchID watchID_;
};

struct FileChangeEvent final {
    enum class Action {
        Delete,
        Add,
        Modified,
        Moved,
    } action;
    std::filesystem::path filename;
    std::filesystem::path oldFilename;
    std::filesystem::path dir;
};

void RegistFileChangeEventHandler(gecs::event_dispatcher<FileChangeEvent>);
void FileChangeEventHandler(
    const FileChangeEvent& event,
    gecs::resource<gecs::mut<nickel::AssetManager>>,
    gecs::resource<gecs::mut<EditorContext>>);