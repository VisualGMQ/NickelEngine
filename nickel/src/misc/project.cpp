#include "misc/project.hpp"
#include "renderer/texture.hpp"

namespace nickel {

void SaveProject(std::string_view rootPath, const TextureManager textureMgr) {
    std::ofstream file(std::string(rootPath) + "/assets.toml");

    toml::table tbl;
    auto textureMgrTbl = textureMgr.Save2Toml();
    tbl.emplace("textures", textureMgrTbl);

    file << toml::toml_formatter{textureMgrTbl} << std::flush;
}

void LoadProject(std::string_view rootPath, TextureManager& textureMgr) {
    auto result = toml::parse_file(std::string(rootPath) + "/assets.toml");
    if (!result) {
        LOGE(log_tag::Res, "load saved textures failed: ", result.error());
        return;
    }

    auto& tbl = result.table();
    if (auto textureMgrTbl = tbl["textures"]; textureMgrTbl.is_table()) {
        textureMgr.LoadFromToml(*textureMgrTbl.as_table());
    }
}

}
