#include "renderer/font.hpp"
#include "core/gogl.hpp"
#include "misc/asset_manager.hpp"

namespace nickel {

FT_Library gFtLib;

void FontSystemInit() {
    if (auto err = FT_Init_FreeType(&gFtLib); err) {
        LOGE(log_tag::Renderer, "freetype2 init failed! ",
             FT_Error_String(err));
    }
}

void FontSystemShutdown() {
    if (auto err = FT_Done_FreeType(gFtLib); err) {
        LOGE(log_tag::Asset, "shutdown freetype2 failed! ", FT_Error_String(err));
    }
}

Font Font::Null;

Character::Character(const FT_GlyphSlot& g)
    : size{cgmath::Vec2(g->bitmap.width, g->bitmap.rows)},
      bearing{cgmath::Vec2(g->bitmap_left, g->bitmap_top)},
      advance{cgmath::Vec2(g->advance.x, g->advance.y)} {
    auto bitmap = g->bitmap;

    auto& assetMgr = gWorld->cur_registry()->res_mut<AssetManager>().get();
    gogl::Sampler sampler = gogl::Sampler::CreateLinearRepeat();
    sampler.wrapper.s = gogl::TextureWrapperType::ClampToEdge;
    sampler.wrapper.t = gogl::TextureWrapperType::ClampToEdge;
    GL_CALL(glPixelStorei(GL_UNPACK_ALIGNMENT, 1));
    texture = assetMgr.TextureMgr().CreateSolitary(
        bitmap.buffer, bitmap.width, bitmap.rows, sampler, gogl::Format::Red,
        gogl::Format::Red);
    GL_CALL(glPixelStorei(GL_UNPACK_ALIGNMENT, 4));
}

Font::Font(const std::filesystem::path& rootPath,
           const std::filesystem::path& filename)
    : Asset(filename) {
    if (auto err = FT_New_Face(
            gFtLib, (rootPath / filename.string()).string().c_str(), 0, &face_);
        err) {
        LOGE(log_tag::Asset, "load font ", filename,
             " failed! error code: ", FT_Error_String(err));
        err = FT_Select_Charmap(face_, FT_ENCODING_UNICODE);
        if (err || !face_->charmap ||
            face_->charmap->encoding != FT_ENCODING_UNICODE) {
            LOGE(log_tag::Asset, "font ", filename,
                 " don't support unicode charset! please change a font! ",
                 FT_Error_String(err));
        }
    }
}

FT_GlyphSlot Font::GetGlyph(uint64_t c, int size) const {
    if (auto err = FT_Set_Pixel_Sizes(face_, 0, size); err) {
        LOGE(log_tag::Asset, "change font pixel size failed! ",
             FT_Error_String(err));
        return nullptr;
    }

    auto glyphIdx = FT_Get_Char_Index(face_, c);
    if (auto err = FT_Load_Glyph(face_, glyphIdx, FT_LOAD_RENDER); err) {
        LOGE(log_tag::Asset, "load glyph ", c, " failed! ", FT_Error_String(err));
        return nullptr;
    }

    return face_->glyph;
}

Font::~Font() {
    FT_Done_Face(face_);
}

FontHandle FontManager::Load(const std::filesystem::path& filename) {
    auto relativePath = filename.is_relative() ? filename
                                               : std::filesystem::relative(
                                                     filename, GetRootPath());
    if (Has(relativePath)) {
        return GetHandle(relativePath);
    }

    auto handle = FontHandle::Create();
    auto font = std::unique_ptr<Font>(new Font{GetRootPath(), relativePath});
    if (font && *font) {
        storeNewItem(handle, std::move(font));
        return handle;
    } else {
        return FontHandle::Null();
    }
}

toml::table FontManager::Save2Toml() const {
    toml::table tbl;
    tbl.emplace("root_path", GetRootPath().string());

    toml::array arr;
    for (auto&& [handle, font] : AllDatas()) {
        arr.push_back(font->RelativePath().string());
    }

    tbl.emplace("font", std::move(arr));
    return tbl;
}

void FontManager::LoadFromToml(toml::table& tbl) {
    std::filesystem::path rootPath;
    if (auto path = tbl.get("root_path"); path && path->is_string()) {
        SetRootPath(path->as_string()->get());
    }

    if (auto fonts = tbl.get("fonts"); fonts && fonts->is_array()) {
        auto arr = *fonts->as_array();
        for (auto&& elem : arr) {
            if (!elem.is_string()) {
                continue;
            }

            Load(elem.as_string()->get());
        }
    }
}

}  // namespace nickel