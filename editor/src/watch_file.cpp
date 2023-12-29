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

void FileChangeEventHandler(
    const FileChangeEvent& event,
    gecs::resource<gecs::mut<nickel::AssetManager>> assetMgr,
    gecs::resource<gecs::mut<EditorContext>> ctx) {
    auto absolutePath = event.dir / event.filename;
    auto path = ctx->GetRelativePath(absolutePath);
    // auto relativePath = std::filesystem::relative(
    //     absolutePath, editorCtx->projectInfo.projectPath);

    if (std::filesystem::exists(path) &&
        std::filesystem::is_directory(path)) {
        return;
    }

    if (event.action == FileChangeEvent::Action::Add) {
        assetMgr->Load(path);
    }

    if (event.action == FileChangeEvent::Action::Delete) {
        assetMgr->Destroy(path);
    }

    if (event.action == FileChangeEvent::Action::Moved) {
        auto filetype = nickel::DetectFileType(path);
        auto oldPath = event.dir / event.oldFilename;
        nickel::VisitTuple(assetMgr->Managers(),
                           [=, &oldPath, &path](auto&& mgr) {
                               if (mgr.GetFileType() == filetype) {
                                   if (mgr.Has(oldPath)) {
                                       auto handle = mgr.GetHandle(oldPath);
                                       mgr.AssociateFile(handle, path);
                                   }
                               }
                           });
    }

    if (event.action == FileChangeEvent::Action::Modified) {
        auto filetype = nickel::DetectFileType(path);
        nickel::VisitTuple(
            assetMgr->Managers(), [=, &path](auto&& mgr) {
                if (mgr.GetFileType() == filetype) {
                    if (auto handle = mgr.GetHandle(path); handle) {
                        mgr.Reload(handle, path);
                    }
                }
            });
    }

    auto& cbWindow = ctx->contentBrowserWindow;
    if (std::filesystem::equivalent(cbWindow.CurPath(), event.dir)) {
        cbWindow.RescanDir();
    }
}