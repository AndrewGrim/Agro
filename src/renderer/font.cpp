#include "font.hpp"

Font::Font(FT_Library ft, std::string file_path, u32 pixel_size, Font::Type type)
: file_path{file_path}, pixel_size{pixel_size}, type{type} {
    if (FT_New_Face(ft, file_path.c_str(), 0, &face)) {
        fail("FAILED_TO_LOAD_FONT", file_path);
        if (FT_Select_Charmap(face, FT_ENCODING_UNICODE)) {
            fail("FAILED_TO_SET_UNICODE_CHARMAP_FOR_FONT", file_path);
        }
    }
    load(face);
}

Font::Font(FT_Library ft, const unsigned char *data, i64 length, u32 pixel_size, Font::Type type)
: file_path{":memory:"}, pixel_size{pixel_size}, type{type} {
    if (FT_New_Memory_Face(ft, data, length, 0, &face)) {
        fail("FAILED_TO_LOAD_FONT", file_path);
        if (FT_Select_Charmap(face, FT_ENCODING_UNICODE)) {
            fail("FAILED_TO_SET_UNICODE_CHARMAP_FOR_FONT", file_path);
        }
    }
    load(face);
}

Font::~Font() {
    FT_Done_Face(face);
    glDeleteTextures(1, &atlas_ID);
}

u32 Font::maxHeight() {
    return atlas_height;
}

Font::Character Font::get(u32 codepoint) {
    auto search = characters.find(codepoint);
    if (search == characters.end()) {
        loadGlyph(codepoint, true);
        return characters[codepoint];
    }
    return search->second;
}

void Font::load(FT_Face face) {
    FT_Set_Pixel_Sizes(face, pixel_size, pixel_size);

    atlas_width = 1024;
    atlas_height = pixel_size; // Note: that this is not guaranteed and needs to be verified when loading characters.
    glActiveTexture(GL_TEXTURE0);
    glGenTextures(1, &atlas_ID);
    glBindTexture(GL_TEXTURE_2D, atlas_ID);
    glTexImage2D(
        GL_TEXTURE_2D,
        0,
        GL_RED,
        atlas_width,
        atlas_height,
        0,
        GL_RED,
        GL_UNSIGNED_BYTE,
        NULL
    );
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_S, GL_CLAMP_TO_EDGE);
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_T, GL_CLAMP_TO_EDGE);
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_LINEAR);
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_LINEAR);

    // Pre-load ascii
    for (u8 c = 32; c < 127; c++) {
        loadGlyph((u32)c, false);
    }
}

// Note: this should NOT be used to initialise a texture
// but only to EXPAND an existing one.
void Font::grow(u32 width, u32 height) {
    u32 new_texture;
    glActiveTexture(GL_TEXTURE0);
    glGenTextures(1, &new_texture);
    glBindTexture(GL_TEXTURE_2D, new_texture);
    glTexImage2D(
        GL_TEXTURE_2D,
        0,
        GL_RED,
        width,
        height,
        0,
        GL_RED,
        GL_UNSIGNED_BYTE,
        NULL
    );
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_S, GL_CLAMP_TO_EDGE);
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_T, GL_CLAMP_TO_EDGE);
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_LINEAR);
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_LINEAR);

    glDeleteTextures(1, &atlas_ID);
    atlas_width = width;
    atlas_height = height;
    atlas_ID = new_texture;

    characters.clear(); // This forces us to re-rasterize ascii glyphs and potentially more
    // but I couldn't quite figure out how to copy the old texture into the new one when it grew in width.
}

void Font::loadGlyph(u32 codepoint, bool bind_texture) {
    if (bind_texture) {
        glActiveTexture(GL_TEXTURE0);
        glBindTexture(GL_TEXTURE_2D, atlas_ID);
    }

    FT_GlyphSlot g = face->glyph;
    if (FT_Load_Char(face, codepoint, FT_LOAD_RENDER)) {
        // TODO would be helpful to print what the codepoint should translate to
        fail("FAILED_TO_LOAD_CHAR", std::to_string(codepoint));
    }
    if (next_slot + g->bitmap.width > atlas_width) {
        warn("grow w");
        grow(atlas_width * 2, atlas_height);
    }
    if (g->bitmap.rows > atlas_height) {
        warn("grow h");
        grow(atlas_width, g->bitmap.rows);
    }

    glTexSubImage2D(
        GL_TEXTURE_2D,
        0,
        next_slot,
        0,
        g->bitmap.width,
        g->bitmap.rows,
        GL_RED,
        GL_UNSIGNED_BYTE,
        g->bitmap.buffer
    );
    characters.emplace(
        codepoint,
        Character(
            Point(g->bitmap_left, g->bitmap_top),
            Size(g->bitmap.width, g->bitmap.rows),
            (i32)(g->advance.x >> 6),
            next_slot / (f32)atlas_width
        )
    );
    next_slot += g->bitmap.width;
}
