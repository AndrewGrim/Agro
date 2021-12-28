#include "font.hpp"

Font::Font(std::string file_path, unsigned int pixel_size, Font::Type type)
: file_path{file_path}, pixel_size{pixel_size}, type{type} {
    FT_Library ft;
    if (FT_Init_FreeType(&ft)) {
        fail("FAILED_TO_INITIALISE_FREETYPE");
    }
    FT_Face face;
    if (FT_New_Face(ft, file_path.c_str(), 0, &face)) {
        fail("FAILED_TO_LOAD_FONT",  file_path);
    }
    load(face);
    FT_Done_Face(face);
    FT_Done_FreeType(ft);
}

Font::Font(const unsigned char *data, signed long length, unsigned int pixel_size, Font::Type type)
: file_path{":memory:"}, pixel_size{pixel_size}, type{type} {
    FT_Library ft;
    if (FT_Init_FreeType(&ft)) {
        fail("FAILED_TO_INITIALISE_FREETYPE");
    }
    FT_Face face;
    if (FT_New_Memory_Face(ft, data, length, 0, &face)) {
        fail("FAILED_TO_LOAD_FONT",  file_path);
    }
    load(face);
    FT_Done_Face(face);
    FT_Done_FreeType(ft);
}

Font::~Font() {
    glDeleteTextures(1, &atlas_ID);
}

void Font::load(FT_Face face) {
    FT_Set_Pixel_Sizes(face, 0, pixel_size);

    FT_GlyphSlot g = face->glyph;
    for (GLubyte c = 32; c < 128; c++)   {
        if (FT_Load_Char(face, c, FT_LOAD_RENDER)) {
            fail("FAILED_TO_LOAD_CHAR",  std::string(1, c));
        }
        atlas_width += g->bitmap.width;
        if (g->bitmap.rows > atlas_height) {
            atlas_height = g->bitmap.rows;
        }
    }
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

    int x = 0;
    for (GLubyte c = 32; c < 128; c++)   {
        if (FT_Load_Char(face, c, FT_LOAD_RENDER)) {
            fail("FAILED_TO_LOAD_CHAR",  std::string(1, c));
        }
        glTexSubImage2D(GL_TEXTURE_2D, 0, x, 0, g->bitmap.width, g->bitmap.rows, GL_RED, GL_UNSIGNED_BYTE, g->bitmap.buffer);
        Font::Character character = {
            Point(g->bitmap_left, g->bitmap_top),
            Size(g->bitmap.width, g->bitmap.rows),
            g->advance.x >> 6,
            x / (float)atlas_width
        };
        characters.insert(std::pair<char, Font::Character>(c, character));
        x += g->bitmap.width;
        if (g->bitmap.rows > max_height) {
            max_height = g->bitmap.rows;
        }
    }
}
