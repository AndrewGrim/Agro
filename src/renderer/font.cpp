#include "font.hpp"

Font::Font(std::string file_path, uint pixel_size, Font::Type type) {
    this->file_path = file_path;
    this->pixel_size = pixel_size;
    this->type = type;
    this->load();
}

Font::~Font() {
    glDeleteTextures(1, &this->atlas_ID);
}

void Font::load() {
    FT_Library ft;    
    if (FT_Init_FreeType(&ft)) {
        println("Error: Failed to initialise FreeType!");
        exit(1); // TODO should we have an enum with different errors and return the appropriate one? probably
    }
    
    FT_Face face;
    if (FT_New_Face(ft, this->file_path.c_str(), 0, &face)) {
        println("Error: Failed to load font!");
        exit(1);
    }
    
    FT_Set_Pixel_Sizes(face, 0, this->pixel_size);

    FT_GlyphSlot g = face->glyph;
    for (GLubyte c = 32; c < 128; c++)   {
        if (FT_Load_Char(face, c, FT_LOAD_RENDER)) {
            exit(1);
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
            exit(1);
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