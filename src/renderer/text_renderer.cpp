#include "text_renderer.hpp"
#include "../app.hpp"

Font::Font(std::string file_path, unsigned int pixel_size, Font::Type type) {
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

TextRenderer::TextRenderer(unsigned int *indices, void *app) {
    this->m_app = app;
    glGenVertexArrays(1, &VAO);
    glGenBuffers(1, &VBO);
    glGenBuffers(1, &EBO);

    glBindVertexArray(VAO);
    glBindBuffer(GL_ARRAY_BUFFER, VBO);
    glBufferData(GL_ARRAY_BUFFER, sizeof(Vertex) * MAX_BATCH_SIZE * QUAD_VERTEX_COUNT, nullptr, GL_DYNAMIC_DRAW);
    glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, EBO);
    glBufferData(GL_ELEMENT_ARRAY_BUFFER, sizeof(unsigned int) * MAX_BATCH_SIZE * QUAD_INDEX_COUNT, indices, GL_STATIC_DRAW);

    glVertexAttribPointer(0, 2, GL_FLOAT, GL_FALSE, sizeof(Vertex), (void*)offsetof(Vertex, position));
    glEnableVertexAttribArray(0);
    
    glVertexAttribPointer(1, 2, GL_FLOAT, GL_FALSE, sizeof(Vertex), (void*)offsetof(Vertex, textureUV));
    glEnableVertexAttribArray(1);

    glVertexAttribPointer(2, 4, GL_FLOAT, GL_FALSE, sizeof(Vertex), (void*)offsetof(Vertex, color));
    glEnableVertexAttribArray(2);

    glVertexAttribPointer(3, 1, GL_FLOAT, GL_FALSE, sizeof(Vertex), (void*)offsetof(Vertex, texture_index));
    glEnableVertexAttribArray(3);

    glVertexAttribPointer(4, 1, GL_FLOAT, GL_FALSE, sizeof(Vertex), (void*)offsetof(Vertex, is_text));
    glEnableVertexAttribArray(4);

    glVertexAttribPointer(5, 4, GL_FLOAT, GL_FALSE, sizeof(Vertex), (void*)offsetof(Vertex, rect));
    glEnableVertexAttribArray(5);

    glGetIntegerv(GL_MAX_TEXTURE_IMAGE_UNITS, &this->max_texture_slots);

    std::string fragment_shader = "#version 330 core\n";
        fragment_shader += "in vec2 TexCoords;\n";
        fragment_shader += "in vec4 color;\n";
        fragment_shader += "in float vTextureIndex;\n";
        fragment_shader += "in float vIsText;\n";
        fragment_shader += "\n";
        fragment_shader += "out vec4 fColor;\n";
        fragment_shader += "\n";
        fragment_shader += "uniform sampler2D textures[" + std::to_string(this->max_texture_slots) + "];\n";
        fragment_shader += "\n";
        fragment_shader += "void main()\n";
        fragment_shader += "{\n";
        fragment_shader += "vec4 sampled;\n";
        fragment_shader += "switch (int(vTextureIndex)) {\n";
        for (int i = 0; i < this->max_texture_slots; i++) {
            fragment_shader += "case " + std::to_string(i) + ":\n";
            fragment_shader += "switch (int(vIsText)) {\n";
            fragment_shader += "case 0: ";
            fragment_shader += "sampled = vec4(texture(textures[" + std::to_string(i) + "], TexCoords));\n";
            fragment_shader += "break;\n";
            fragment_shader += "case 1: ";
            fragment_shader += "sampled = vec4(1.0, 1.0, 1.0, texture(textures[" + std::to_string(i) + "], TexCoords).r);\n";
            fragment_shader += "break;\n";
            fragment_shader += "}\n";
            fragment_shader += "break;\n";
        }
        fragment_shader += "}\n";
        fragment_shader += "fColor = color * sampled;\n";
        fragment_shader += "}";

    this->shader = Shader(
        "#version 330 core\n"
        "layout (location = 0) in vec2 position;\n"
        "layout (location = 1) in vec2 textureUV;\n"
        "layout (location = 2) in vec4 aColor;\n"
        "layout (location = 3) in float aTextureIndex;\n"
        "layout (location = 4) in float aIsText;\n"
        "layout (location = 5) in vec4 aRect;\n"
        "\n"
        "out vec2 TexCoords;\n"
        "out vec4 color;\n"
        "out float vTextureIndex;\n"
        "out float vIsText;\n"
        "\n"
        "uniform mat4 projection;\n"
        "\n"
        "void main()\n"
        "{\n"
            "mat4 model = mat4(\n"
                "vec4(aRect.z, 0.0, 0.0, 0.0),\n"
                "vec4(0.0, aRect.w, 0.0, 0.0),\n"
                "vec4(0.0, 0.0, 1.0, 0.0),\n"
                "vec4(aRect.x, aRect.y, 0.0, 1.0)\n"
            ");\n"
            "gl_Position = projection * model * vec4(position, 1.0, 1.0);\n"
            "TexCoords = textureUV;\n"
            "color = aColor;\n"
            "vTextureIndex = aTextureIndex;\n"
            "vIsText = aIsText;\n"
        "}",
        fragment_shader.c_str()
    );

    shader.use();
    std::vector<int> texture_indices;
    for (int i = 0; i < 32; i++) {
        texture_indices.push_back(i);
    }
    auto loc = glGetUniformLocation(this->shader.ID, "textures");
    glUniform1iv(loc, 32, texture_indices.data());
}

TextRenderer::~TextRenderer() {
    glDeleteVertexArrays(1, &VAO);
    glDeleteBuffers(1, &VBO);
    glDeleteBuffers(1, &EBO);
    delete[] vertices;
}

void TextRenderer::reset() {
    this->index = 0;
    this->count = 0;
    this->current_texture_slot = 2;
}

void TextRenderer::check() {
    if (this->index + QUAD_VERTEX_COUNT > MAX_BATCH_SIZE * QUAD_VERTEX_COUNT) render();
}

void TextRenderer::fillText(Font *font, std::string text, float x, float y, Color color, float scale) {
    Size window = ((Application*)this->m_app)->m_size;
    std::string::const_iterator c;
    // TODO handle newlines and tab characters
    // TODO also we should probably find the max height for each font and store that information somewhere
    // then we could use that to calculate the height of text by multiplying the max with amound of newlines
    
    if (this->current_texture_slot > this->max_texture_slots - 1) {
        render();
    }
    glActiveTexture(gl_texture_begin + this->current_texture_slot); 
    glBindTexture(GL_TEXTURE_2D, font->atlas_ID);
    for (c = text.begin(); c != text.end() && x <= window.w; c++) {
        check();
        Font::Character ch = font->characters[*c];
        float advance = ((ch.advance >> 6) * scale);
        if (x + advance >= 0) {
            float xpos = x + ch.bearing.x * scale;
            float ypos = y + (font->characters['H'].bearing.y - ch.bearing.y) * scale;

            float w = ch.size.x * scale;
            float h = ch.size.y * scale;
            
            vertices[index++] = {
                {xpos, ypos + h}, 
                {ch.textureX, (h / font->atlas_height)},
                {color.r, color.g, color.b, color.a},
                (float)this->current_texture_slot,
                1.0,
                {1.0, 1.0, 1.0, 1.0}
            };
            vertices[index++] = {
                {xpos, ypos}, 
                {ch.textureX, 0.0},
                {color.r, color.g, color.b, color.a},
                (float)this->current_texture_slot,
                1.0,
                {1.0, 1.0, 1.0, 1.0}
            };
            vertices[index++] = {
                {xpos + w, ypos}, 
                {ch.textureX + (w / font->atlas_width), 0.0},
                {color.r, color.g, color.b, color.a},
                (float)this->current_texture_slot,
                1.0,
                {1.0, 1.0, 1.0, 1.0}
            };
            vertices[index++] = {
                {xpos + w, ypos + h}, 
                {ch.textureX + (w / font->atlas_width), (h / font->atlas_height)},
                {color.r, color.g, color.b, color.a},
                (float)this->current_texture_slot,
                1.0,
                {1.0, 1.0, 1.0, 1.0}
            };

            count++;
        }
        x += advance;
    }
    this->current_texture_slot++;
}

Size TextRenderer::measureText(Font *font, std::string text, float scale) {
    std::string::const_iterator c;
    Size size;
    for (char c : text) {
        Font::Character ch = font->characters[c];
        size.w += (ch.advance >> 6) * scale;
        size.h = font->max_height; // TODO this should account for newlines
    }

    return size;
}

Size TextRenderer::measureText(Font *font, char c, float scale) {
    Size size;
    Font::Character ch = font->characters[c];
    size.w = ch.advance >> 6;
    size.h = font->max_height;

    return size;
}

void TextRenderer::drawImage(float x, float y, Texture *texture, Color color) {
    check();

    if (this->current_texture_slot > this->max_texture_slots - 1) {
        render();
    }
    glActiveTexture(gl_texture_begin + this->current_texture_slot);
    glBindTexture(GL_TEXTURE_2D, texture->ID);

    vertices[index++] = {
        // actually the texture->height and width should go
        // and go back to 1.0, we will get the position from the model
        {0.0,  1.0},
        {1.0, 1.0},
        {color.r, color.g, color.b, color.a},
        (float)this->current_texture_slot,
        0.0,
        {x, y, (float)texture->width, (float)texture->height}
    };
    vertices[index++] = {
        {0.0,  0.0},
        {1.0, 0.0},
        {color.r, color.g, color.b, color.a},
        (float)this->current_texture_slot,
        0.0,
        {x, y, (float)texture->width, (float)texture->height}
    };
    vertices[index++] = {
        {1.0,  0.0},
        {0.0, 0.0},
        {color.r, color.g, color.b, color.a},
        (float)this->current_texture_slot,
        0.0,
        {x, y, (float)texture->width, (float)texture->height}
    };
    vertices[index++] = {
        {1.0,  1.0},
        {0.0, 1.0},
        {color.r, color.g, color.b, color.a},
        (float)this->current_texture_slot,
        0.0,
        {x, y, (float)texture->width, (float)texture->height}
    };
    count++;
    this->current_texture_slot++;
}

void TextRenderer::render() {
    shader.use();
    glBindVertexArray(VAO);
    glBindBuffer(GL_ARRAY_BUFFER, VBO);
    glBufferSubData(GL_ARRAY_BUFFER, 0, sizeof(Vertex) * index, vertices);
    glDrawElements(GL_TRIANGLES, 6 * count, GL_UNSIGNED_INT, 0);
    // TODO this would probably be the right point to also consolidate the quad and text renderers
    // since they could easily use the same buffer
    // TODO we could finally add a better line implementation, and also add a draw pixel implementation
    // by just using quads, this way we could keep it all in the same buffer
    // TODO later on we could introduce rounded corners and circles by sampling pixels in the fragment shader??
    reset();
}