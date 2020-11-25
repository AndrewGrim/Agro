#include "text_renderer.hpp"
#include "../app.hpp"

TextRenderer::TextRenderer(unsigned int *indices, void *app) {
    this->m_app = app;
    glGenVertexArrays(1, &VAO);
    glGenBuffers(1, &VBO);
    glGenBuffers(1, &EBO);

    glBindVertexArray(VAO);
    glBindBuffer(GL_ARRAY_BUFFER, VBO);
    glBufferData(GL_ARRAY_BUFFER, sizeof(vertices), nullptr, GL_DYNAMIC_DRAW);
    glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, EBO);
    glBufferData(GL_ELEMENT_ARRAY_BUFFER, sizeof(unsigned int) * MAX_BATCH_SIZE * QUAD_INDEX_COUNT, indices, GL_STATIC_DRAW);

    glVertexAttribPointer(0, 2, GL_FLOAT, GL_FALSE, sizeof(Vertex), (void*)offsetof(Vertex, position));
    glEnableVertexAttribArray(0);
    
    glVertexAttribPointer(1, 2, GL_FLOAT, GL_FALSE, sizeof(Vertex), (void*)offsetof(Vertex, textureUV));
    glEnableVertexAttribArray(1);

    glVertexAttribPointer(2, 4, GL_FLOAT, GL_FALSE, sizeof(Vertex), (void*)offsetof(Vertex, color));
    glEnableVertexAttribArray(2);
}

TextRenderer::~TextRenderer() {
    glDeleteVertexArrays(1, &VAO);
    glDeleteBuffers(1, &VBO);
    glDeleteBuffers(1, &EBO);
    glDeleteTextures(1, &atlasID);
}

void TextRenderer::reset() {
    index = 0;
    count = 0;
}

void TextRenderer::check() {
    if (index + QUAD_VERTEX_COUNT > MAX_BATCH_SIZE) render();
}

void TextRenderer::load(std::string font, unsigned int fontSize) {
    characters.clear();

    FT_Library ft;    
    if (FT_Init_FreeType(&ft))
        std::cout << "ERROR::FREETYPE: Could not init FreeType Library" << std::endl;
    
    FT_Face face;
    if (FT_New_Face(ft, font.c_str(), 0, &face))
        std::cout << "ERROR::FREETYPE: Failed to load font" << std::endl;
    
    FT_Set_Pixel_Sizes(face, 0, fontSize);
    glPixelStorei(GL_UNPACK_ALIGNMENT, 1);

    FT_GlyphSlot g = face->glyph;

    for (GLubyte c = 32; c < 128; c++)   {
        if (FT_Load_Char(face, c, FT_LOAD_RENDER)) exit(1);
        atlasWidth += g->bitmap.width;
        if (g->bitmap.rows > atlasHeight) atlasHeight = g->bitmap.rows;
    }
    glActiveTexture(GL_TEXTURE0);
    glGenTextures(1, &atlasID);
    glBindTexture(GL_TEXTURE_2D, atlasID);
    glTexImage2D(
        GL_TEXTURE_2D,
        0,
        GL_RED,
        atlasWidth,
        atlasHeight,
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
        if (FT_Load_Char(face, c, FT_LOAD_RENDER)) exit(1);
        glTexSubImage2D(GL_TEXTURE_2D, 0, x, 0, g->bitmap.width, g->bitmap.rows, GL_RED, GL_UNSIGNED_BYTE, g->bitmap.buffer);
        TextCharacter character = {
            glm::ivec2(g->bitmap.width, g->bitmap.rows),
            glm::ivec2(g->bitmap_left, g->bitmap_top),
            g->advance.x,
            (float)x / (float)atlasWidth
        };
        characters.insert(std::pair<char, TextCharacter>(c, character));
        x += g->bitmap.width;
    }
    
    FT_Done_Face(face);
    FT_Done_FreeType(ft);
}

void TextRenderer::fillText(std::string text, float x, float y, Color color, float scale) {
    check();
    
    Size<int> window = ((Application*)this->m_app)->m_size;
    std::string::const_iterator c;
    for (c = text.begin(); c != text.end() && x <= window.w; c++) {
        TextCharacter ch = characters[*c];
        float advance = ((ch.advance >> 6) * scale);
        if (x + advance >= 0) {
            float xpos = x + ch.bearing.x * scale;
            float ypos = y + (characters['H'].bearing.y - ch.bearing.y) * scale;

            float w = ch.size.x * scale;
            float h = ch.size.y * scale;
            
            vertices[index++] = {
                {xpos, ypos + h}, 
                {ch.textureX, (h / atlasHeight)},
                {color.r, color.g, color.b, color.a}
            };
            vertices[index++] = {
                {xpos, ypos}, 
                {ch.textureX, 0.0f},
                {color.r, color.g, color.b, color.a}
            };
            vertices[index++] = {
                {xpos + w, ypos}, 
                {ch.textureX + (w / atlasWidth), 0.0f},
                {color.r, color.g, color.b, color.a}
            };
            vertices[index++] = {
                {xpos + w, ypos + h}, 
                {ch.textureX + (w / atlasWidth), (h / atlasHeight)},
                {color.r, color.g, color.b, color.a}
            };

            count++;
        }
        x += advance;
    }
}

Size<float> TextRenderer::measureText(std::string text, float scale) {
    std::string::const_iterator c;
    Size<float> size;
    for (char c : text) {
        TextCharacter ch = characters[c];
        size.w += (ch.advance >> 6) * scale;
        size.h = ch.size.y > size.h ? ch.size.y : size.h;
    }

    return size;
}

Size<float> TextRenderer::measureText(char c, float scale) {
    Size<float> size;
    TextCharacter ch = characters[c];
    size.w = ch.advance >> 6;
    size.h = ch.size.y;

    return size;
}

void TextRenderer::render() {
    shader.use();
    glActiveTexture(GL_TEXTURE0);
    glBindTexture(GL_TEXTURE_2D, atlasID);
    glBindVertexArray(VAO);
    glBindBuffer(GL_ARRAY_BUFFER, VBO);
    glBufferData(GL_ARRAY_BUFFER, sizeof(Vertex) * index, vertices, GL_STATIC_DRAW);
    glDrawElements(GL_TRIANGLES, 6 * count, GL_UNSIGNED_INT, 0);

    reset();
}