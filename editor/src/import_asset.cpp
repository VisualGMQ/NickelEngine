#include "import_asset.hpp"
#include "content_browser.hpp"

void SelectAndLoadAsset(gecs::registry reg) {
    auto filenames = OpenFileDialog("load assets");

    bool contentChanged = false;
    auto& cbInfo = reg.res<gecs::mut<ContentBrowserInfo>>().get();

    for (auto& filename : filenames) {
        auto type = DetectFileType(filename);
        auto copyPath = cbInfo.path / filename.filename();
        if (filename != copyPath) {
            if (!std::filesystem::copy_file(filename, copyPath)) {
                LOGW(nickel::log_tag::Editor, "copy ", filename, " to ",
                     copyPath, " failed");
                continue;
            }
        }
        auto relativePath =
            std::filesystem::relative(copyPath, cbInfo.rootPath);

        switch (type) {
            case FileType::Unknown:
                LOGW(nickel::log_tag::Editor, relativePath,
                     " can't load as asset(unknown type)");
                break;
            case FileType::Image:
                reg.res<gecs::mut<nickel::TextureManager>>()->Load(
                    relativePath, nickel::gogl::Sampler::CreateLinearRepeat());
                contentChanged = true;
                break;
            case FileType::Font:
                reg.res<gecs::mut<nickel::FontManager>>()->Load(relativePath);
                contentChanged = true;
                break;
            case FileType::Audio:
                // TODO: add audio
                break;
        }
    }

    // TODO: use file watcher to do this
    if (contentChanged) {
        cbInfo.RescanDir();
    }
}