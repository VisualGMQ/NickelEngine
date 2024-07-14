#include "graphics/font.hpp"
#include "common/profile.hpp"
#include "graphics/context.hpp"

#include "ft2build.h"
#include FT_FREETYPE_H

namespace nickel {

TextCache TextCache::Null;

FT_Library gFtLib;

const char* FT_Error_String(FT_Error error_code) {
    #undef FTERRORS_H_
    #define FT_ERRORDEF( e, v, s )  case e: return s;
    #define FT_ERROR_START_LIST     switch (error_code) {
    #define FT_ERROR_END_LIST       }
    #include FT_ERRORS_H
    return "(Unknown error)";
}

void FontSystemInit() {
    PROFILE_BEGIN();

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

bool Font::Load(const std::filesystem::path& filename) {
    return loadFromFile(filename);
}

bool Font::Load(const toml::table& tbl) {
    if (auto node = tbl.get("path"); node && node->is_string()) {
        auto& filename = node->as_string()->get();
        return loadFromFile(filename);
    } else {
        LOGE(log_tag::Asset, "load asset failed: meta file miss `path`");
        return false;
    }
}

bool Font::loadFromFile(const std::filesystem::path& filename) {
    FT_Face newFace{};

    if (auto err = FT_New_Face(gFtLib, filename.string().c_str(), 0, &newFace);
        err) {
        LOGE(log_tag::Asset, "load font ", filename,
            " failed! error code: ", FT_Error_String(err));
        FT_Done_Face(newFace);
        return false;
    }

    auto err = FT_Select_Charmap(face_, FT_ENCODING_UNICODE);
    if (err || !face_->charmap ||
        face_->charmap->encoding != FT_ENCODING_UNICODE) {
        LOGE(log_tag::Asset, "font ", filename,
            " don't support unicode charset! please change a font! ",
            FT_Error_String(err));
        FT_Done_Face(newFace);
        return false;
    }

    ChangeRelativePath(filename);
    FT_Done_Face(face_);
    face_ = newFace;

    return true;
}

bool Font::Save(toml::table& tbl) const {
    NICKEL_TOML_EMPLACE_NODE(tbl, "path", GetRelativePath().string());
    return true;
}

Character::Character(const FT_GlyphSlot& g)
    : size{cgmath::Vec2(g->bitmap.width, g->bitmap.rows)},
      bearing{cgmath::Vec2(g->bitmap_left, g->bitmap_top)},
      advance{cgmath::Vec2(g->advance.x, g->advance.y)} {
    auto bitmap = g->bitmap;

    texture.reset(new Texture(bitmap.buffer, bitmap.width, bitmap.rows));
}

FT_GlyphSlot Font::GetGlyph(uint64_t c, int size) const {
    if (auto err = FT_Set_Pixel_Sizes(face_, 0, size); err) {
        LOGE(log_tag::Asset, "change font pixel size failed! ",
             FT_Error_String(err));
        return {};
    }

    auto glyphIdx = FT_Get_Char_Index(face_, c);
    if (auto err = FT_Load_Glyph(face_, glyphIdx, FT_LOAD_RENDER); err) {
        LOGE(log_tag::Asset, "load glyph ", c, " failed! ",
             FT_Error_String(err));
        return {};
    }

    return face_->glyph;
}

Font::~Font() {
    FT_Done_Face(face_);
}

}  // namespace nickel