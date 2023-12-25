#include "watch_file.hpp"

FileWatcher::FileWatcher(const std::filesystem::path& path,
                         gecs::registry reg) {
    watcher_ = std::make_unique<efsw::FileWatcher>();
    listener_ = std::make_unique<UpdateListener>(reg);
    watchID_ = watcher_->addWatch(path.string(), listener_.get(), true);
    watcher_->watch();
}

FileWatcher::~FileWatcher() {
    watcher_->removeWatch(watchID_);
}

void UpdateListener::handleFileAction(efsw::WatchID watchid,
                                      const std::string& dir,
                                      const std::string& filename,
                                      efsw::Action action,
                                      std::string oldFilename) {
    auto dispatcher = reg_.event_dispatcher<FileChangeEvent>();
    FileChangeEvent event;
    event.filename = filename;
    event.oldFilename = oldFilename;
    event.dir = dir;

    switch (action) {
        case efsw::Actions::Add:
            event.action = FileChangeEvent::Action::Add;
            break;
        case efsw::Actions::Delete:
            event.action = FileChangeEvent::Action::Delete;
            break;
        case efsw::Actions::Modified:
            event.action = FileChangeEvent::Action::Modified;
            break;
        case efsw::Actions::Moved:
            event.action = FileChangeEvent::Action::Moved;
            break;
        default:
            Assert(false, "should never happen!");
    }

    dispatcher.enqueue(event);
}

void RegistFileChangeEventHandler(
    gecs::event_dispatcher<FileChangeEvent> dispatcher) {
    dispatcher.sink().add<FileChangeEventHandler>();
}

void handleAssetChange(nickel::TextureManager& mgr,
                       const FileChangeEvent& event) {
    auto filetype = nickel::DetectFileType(event.filename);
    if (filetype == nickel::FileType::Unknown) {
        return;
    }

    auto absolutePath = event.dir / event.filename;
    if (event.action == FileChangeEvent::Action::Add) {
        mgr.Load(absolutePath, nickel::gogl::Sampler::CreateLinearRepeat());
    } else if (event.action == FileChangeEvent::Action::Delete) {
        mgr.Destroy(absolutePath);
    } else if (event.action == FileChangeEvent::Action::Moved) {
        if (std::filesystem::relative(absolutePath, mgr.GetRootPath())
                .empty()) {
            mgr.Destroy(event.dir / event.oldFilename);
        } else {
            if (mgr.Has(event.oldFilename)) {
                auto& texture = mgr.Get(event.oldFilename);
                // TODO: change file path
            }
        }
    }
}

void FileChangeEventHandler(
    const FileChangeEvent& event,
    gecs::resource<gecs::mut<nickel::AssetManager>> assetMgr,
    gecs::resource<gecs::mut<ContentBrowserInfo>> cbInfo) {
    auto absolutePath = event.dir / event.filename;

    if (event.action == FileChangeEvent::Action::Add) {
        assetMgr->Load(absolutePath);
    }

    if (event.action == FileChangeEvent::Action::Delete) {
        assetMgr->Destroy(absolutePath);
    }

    if (event.action == FileChangeEvent::Action::Moved) {
        auto filetype = nickel::DetectFileType(absolutePath);
        auto oldPath = event.dir / event.oldFilename;
        nickel::VisitTuple(assetMgr->Managers(),
                           [=, &oldPath, &absolutePath](auto&& mgr) {
                               if (mgr.GetFileType() == filetype) {
                                   if (mgr.Has(oldPath)) {
                                       auto handle = mgr.GetHandle(oldPath);
                                       mgr.AssociateFile(handle, absolutePath);
                                   }
                               }
                           });
    }

    if (event.action == FileChangeEvent::Action::Modified) {
        auto filetype = nickel::DetectFileType(absolutePath);
        nickel::VisitTuple(
            assetMgr->Managers(), [=, &absolutePath](auto&& mgr) {
                if (mgr.GetFileType() == filetype) {
                    if (auto handle = mgr.GetHandle(absolutePath); handle) {
                        mgr.Reload(handle, absolutePath);
                    }
                }
            });
    }

    std::string action;
    switch (event.action) {
        case FileChangeEvent::Action::Delete:
            action = "delete";
            break;
        case FileChangeEvent::Action::Add:
            action = "add";
            break;
        case FileChangeEvent::Action::Modified:
            action = "modified";
            break;
        case FileChangeEvent::Action::Moved:
            action = "moved";
            break;
    }

    LOGW("editor", "cbInfo path = ", cbInfo->path);
    LOGW("editor", "event.dir = ", event.dir);
    if (std::filesystem::equivalent(cbInfo->path, event.dir)) {
        cbInfo->RescanDir();
    }

    LOGW(nickel::log_tag::Editor, "file ", action,
         " oldFilename = ", event.oldFilename,
         ", new filename = ", event.filename, ", dir = ", event.dir);
}