#include "font.hpp"

Font::Font(std::string file_path, unsigned int pixel_size, Font::Type type) {
    this->file_path = file_path;
    this->pixel_size = pixel_size;
    this->type = type;
    load();
}

Font::Font(const unsigned char *data, signed long length, unsigned int pixel_size, Font::Type type) {
    file_path = ":memory:";
    this->pixel_size = pixel_size;
    this->type = type;
    loadFromMemory(data, length);   
}

Font::~Font() {
    glDeleteTextures(1, &this->atlas_ID);
}

void Font::load() {
    FT_Library ft;    
    if (FT_Init_FreeType(&ft)) {
        error("FAILED_TO_INITIALISE_FREETYPE");
    }
    
    FT_Face face;
    if (FT_New_Face(ft, this->file_path.c_str(), 0, &face)) {
        error("FAILED_TO_LOAD_FONT",  file_path);
    }
    
    FT_Set_Pixel_Sizes(face, 0, this->pixel_size);

    FT_GlyphSlot g = face->glyph;
    for (GLubyte c = 32; c < 128; c++)   {
        if (FT_Load_Char(face, c, FT_LOAD_RENDER)) {
            error("FAILED_TO_LOAD_CHAR",  std::string(1, c));
        }
        this->atlas_width += g->bitmap.width;
        if (g->bitmap.rows > this->atlas_height) {
            this->atlas_height = g->bitmap.rows;
        }
    }
    glActiveTexture(GL_TEXTURE0);
    glGenTextures(1, &this->atlas_ID);
    glBindTexture(GL_TEXTURE_2D, this->atlas_ID);
    glTexImage2D(
        GL_TEXTURE_2D,
        0,
        GL_RED,
        this->atlas_width,
        this->atlas_height,
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
            error("FAILED_TO_LOAD_CHAR",  std::string(1, c));
        }
        glTexSubImage2D(GL_TEXTURE_2D, 0, x, 0, g->bitmap.width, g->bitmap.rows, GL_RED, GL_UNSIGNED_BYTE, g->bitmap.buffer);
        Font::Character character = {
            glm::ivec2(g->bitmap.width, g->bitmap.rows),
            glm::ivec2(g->bitmap_left, g->bitmap_top),
            g->advance.x,
            (float)x / (float)this->atlas_width
        };
        this->characters.insert(std::pair<char, Font::Character>(c, character));
        x += g->bitmap.width;
        if (g->bitmap.rows > this->max_height) {
            this->max_height = g->bitmap.rows;
        }
    }
    
    FT_Done_Face(face);
    FT_Done_FreeType(ft);
}

void Font::loadFromMemory(const unsigned char *data, signed long length) {
    FT_Library ft;    
    if (FT_Init_FreeType(&ft)) {
        error("FAILED_TO_INITIALISE_FREETYPE");
    }
    
    FT_Face face;
    if (FT_New_Memory_Face(ft, data, length, 0, &face)) {
        error("FAILED_TO_LOAD_FONT",  file_path);
    }
    
    FT_Set_Pixel_Sizes(face, 0, this->pixel_size);

    FT_GlyphSlot g = face->glyph;
    for (GLubyte c = 32; c < 128; c++)   {
        if (FT_Load_Char(face, c, FT_LOAD_RENDER)) {
            error("FAILED_TO_LOAD_CHAR",  std::string(1, c));
        }
        this->atlas_width += g->bitmap.width;
        if (g->bitmap.rows > this->atlas_height) {
            this->atlas_height = g->bitmap.rows;
        }
    }
    glActiveTexture(GL_TEXTURE0);
    glGenTextures(1, &this->atlas_ID);
    glBindTexture(GL_TEXTURE_2D, this->atlas_ID);
    glTexImage2D(
        GL_TEXTURE_2D,
        0,
        GL_RED,
        this->atlas_width,
        this->atlas_height,
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
            error("FAILED_TO_LOAD_CHAR",  std::string(1, c));
        }
        glTexSubImage2D(GL_TEXTURE_2D, 0, x, 0, g->bitmap.width, g->bitmap.rows, GL_RED, GL_UNSIGNED_BYTE, g->bitmap.buffer);
        Font::Character character = {
            glm::ivec2(g->bitmap.width, g->bitmap.rows),
            glm::ivec2(g->bitmap_left, g->bitmap_top),
            g->advance.x,
            (float)x / (float)this->atlas_width
        };
        this->characters.insert(std::pair<char, Font::Character>(c, character));
        x += g->bitmap.width;
        if (g->bitmap.rows > this->max_height) {
            this->max_height = g->bitmap.rows;
        }
    }
    
    FT_Done_Face(face);
    FT_Done_FreeType(ft);
}