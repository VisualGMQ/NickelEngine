#include "renderer/font.hpp"
#include "core/gogl.hpp"
#include "misc/asset_manager.hpp"

namespace nickel {

TextCache TextCache::Null;

FT_Library gFtLib;

void FontSystemInit() {
    if (auto err = FT_Init_FreeType(&gFtLib); err) {
        LOGE(log_tag::Renderer, "freetype2 init failed! ",
             FT_Error_String(err));
    }
}

void FontSystemShutdown() {
    if (auto err = FT_Done_FreeType(gFtLib); err) {
        LOGE(log_tag::Asset, "shutdown freetype2 failed! ",
             FT_Error_String(err));
    }
}

Font Font::Null;

Font::Font(const toml::table& tbl) {
    if (auto node = tbl.get("path"); node && node->is_string()) {
        *this = Font{node->as_string()->get()};
    }
}

Font::Font(const std::filesystem::path& filename) : Asset(filename) {
    if (auto err = FT_New_Face(gFtLib, filename.string().c_str(), 0, &face_);
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

template <>
std::unique_ptr<Font> LoadAssetFromToml(const toml::table& tbl) {
    if (auto path = tbl.get("path"); path && path->is_string()) {
        return std::make_unique<Font>(path->as_string()->get());
    }
    return nullptr;
}

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

FT_GlyphSlot Font::GetGlyph(uint64_t c, int size) const {
    if (auto err = FT_Set_Pixel_Sizes(face_, 0, size); err) {
        LOGE(log_tag::Asset, "change font pixel size failed! ",
             FT_Error_String(err));
        return nullptr;
    }

    auto glyphIdx = FT_Get_Char_Index(face_, c);
    if (auto err = FT_Load_Glyph(face_, glyphIdx, FT_LOAD_RENDER); err) {
        LOGE(log_tag::Asset, "load glyph ", c, " failed! ",
             FT_Error_String(err));
        return nullptr;
    }

    return face_->glyph;
}

toml::table Font::Save2Toml() const {
    toml::table tbl;
    tbl.emplace("path", RelativePath().string());
    return tbl;
}

Font::~Font() {
    FT_Done_Face(face_);
}

FontHandle FontManager::Load(const std::filesystem::path& filename) {
    if (Has(filename)) {
        return GetHandle(filename);
    }

    auto handle = FontHandle::Create();
    auto font = std::make_unique<Font>(filename);
    if (font && *font) {
        storeNewItem(handle, std::move(font));
        return handle;
    } else {
        return FontHandle::Null();
    }
}

}  // namespace nickel