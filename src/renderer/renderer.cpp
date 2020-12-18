#include "renderer.hpp"
#include "../app.hpp"

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

Renderer::Renderer(uint *indices, void *app) {
    this->m_app = app;
    glGenVertexArrays(1, &VAO);
    glGenBuffers(1, &VBO);
    glGenBuffers(1, &EBO);

    glBindVertexArray(VAO);
    glBindBuffer(GL_ARRAY_BUFFER, VBO);
    glBufferData(GL_ARRAY_BUFFER, sizeof(Vertex) * MAX_BATCH_SIZE * QUAD_VERTEX_COUNT, nullptr, GL_DYNAMIC_DRAW);
    glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, EBO);
    glBufferData(GL_ELEMENT_ARRAY_BUFFER, sizeof(uint) * MAX_BATCH_SIZE * QUAD_INDEX_COUNT, indices, GL_STATIC_DRAW);

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
        fragment_shader += "in vec2 v_texture_uv;\n";
        fragment_shader += "in vec4 v_color;\n";
        fragment_shader += "in float v_texture_slot_index;\n";
        fragment_shader += "in float v_sampler_type;\n";
        fragment_shader += "\n";
        fragment_shader += "out vec4 f_color;\n";
        fragment_shader += "\n";
        fragment_shader += "uniform sampler2D textures[" + std::to_string(this->max_texture_slots) + "];\n";
        fragment_shader += "\n";
        fragment_shader += "void main()\n";
        fragment_shader += "{\n";
        fragment_shader += "vec4 sampled;\n";
        fragment_shader += "switch (int(v_texture_slot_index)) {\n";
        for (int i = 0; i < this->max_texture_slots; i++) {
            fragment_shader += "case " + std::to_string(i) + ":\n";
            fragment_shader += "switch (int(v_sampler_type)) {\n";
            fragment_shader += "case 0: ";
            fragment_shader += "sampled = vec4(1.0, 1.0, 1.0, 1.0);\n";
            fragment_shader += "break;\n";
            fragment_shader += "case 1: ";
            fragment_shader += "sampled = vec4(texture(textures[" + std::to_string(i) + "], v_texture_uv));\n";
            fragment_shader += "break;\n";
            fragment_shader += "case 2: ";
            fragment_shader += "sampled = vec4(1.0, 1.0, 1.0, texture(textures[" + std::to_string(i) + "], v_texture_uv).r);\n";
            fragment_shader += "break;\n";
            fragment_shader += "}\n";
            fragment_shader += "break;\n";
        }
        fragment_shader += "}\n";
        fragment_shader += "f_color = v_color * sampled;\n";
        fragment_shader += "}";

    this->shader = Shader(
        "#version 330 core\n"
        "layout (location = 0) in vec2 a_opengl_position;\n"
        "layout (location = 1) in vec2 a_texture_uv;\n"
        "layout (location = 2) in vec4 a_color;\n"
        "layout (location = 3) in float a_texture_slot_index;\n"
        "layout (location = 4) in float a_sampler_type;\n"
        "layout (location = 5) in vec4 a_rect;\n"
        "\n"
        "out vec2 v_texture_uv;\n"
        "out vec4 v_color;\n"
        "out float v_texture_slot_index;\n"
        "out float v_sampler_type;\n"
        "\n"
        "uniform mat4 projection;\n"
        "\n"
        "void main()\n"
        "{\n"
            "mat4 model = mat4(\n"
                "vec4(a_rect.z, 0.0, 0.0, 0.0),\n"
                "vec4(0.0, a_rect.w, 0.0, 0.0),\n"
                "vec4(0.0, 0.0, 1.0, 0.0),\n"
                "vec4(a_rect.x, a_rect.y, 0.0, 1.0)\n"
            ");\n"
            "gl_Position = projection * model * vec4(a_opengl_position, 1.0, 1.0);\n"
            "v_texture_uv = a_texture_uv;\n"
            "v_color = a_color;\n"
            "v_texture_slot_index = a_texture_slot_index;\n"
            "v_sampler_type = a_sampler_type;\n"
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

Renderer::~Renderer() {
    glDeleteVertexArrays(1, &VAO);
    glDeleteBuffers(1, &VBO);
    glDeleteBuffers(1, &EBO);
    delete[] vertices;
}

void Renderer::reset() {
    this->index = 0;
    this->count = 0;
    this->current_texture_slot = 2;
}

void Renderer::check() {
    if (this->index + QUAD_VERTEX_COUNT > MAX_BATCH_SIZE * QUAD_VERTEX_COUNT) render();
}

void Renderer::fillText(Font *font, std::string text, float x, float y, Color color, float scale) {
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
            
            // TOP LEFT
            vertices[index++] = {
                {xpos, ypos + h}, 
                {ch.textureX, (h / font->atlas_height)},
                {color.r, color.g, color.b, color.a},
                (float)this->current_texture_slot,
                (float)Renderer::Sampler::Text,
                {1.0, 1.0, 1.0, 1.0}
            };
            // BOTTOM LEFT
            vertices[index++] = {
                {xpos, ypos}, 
                {ch.textureX, 0.0},
                {color.r, color.g, color.b, color.a},
                (float)this->current_texture_slot,
                (float)Renderer::Sampler::Text,
                {1.0, 1.0, 1.0, 1.0}
            };
            // BOTTOM RIGHT
            vertices[index++] = {
                {xpos + w, ypos}, 
                {ch.textureX + (w / font->atlas_width), 0.0},
                {color.r, color.g, color.b, color.a},
                (float)this->current_texture_slot,
                (float)Renderer::Sampler::Text,
                {1.0, 1.0, 1.0, 1.0}
            };
            // TOP RIGHT
            vertices[index++] = {
                {xpos + w, ypos + h}, 
                {ch.textureX + (w / font->atlas_width), (h / font->atlas_height)},
                {color.r, color.g, color.b, color.a},
                (float)this->current_texture_slot,
                (float)Renderer::Sampler::Text,
                {1.0, 1.0, 1.0, 1.0}
            };

            count++;
        }
        x += advance;
    }
    this->current_texture_slot++;
}

Size Renderer::measureText(Font *font, std::string text, float scale) {
    std::string::const_iterator c;
    Size size;
    for (char c : text) {
        Font::Character ch = font->characters[c];
        size.w += (ch.advance >> 6) * scale;
        size.h = font->max_height; // TODO this should account for newlines
    }

    return size;
}

Size Renderer::measureText(Font *font, char c, float scale) {
    Size size;
    Font::Character ch = font->characters[c];
    size.w = ch.advance >> 6;
    size.h = font->max_height;

    return size;
}

void Renderer::drawImage(float x, float y, Texture *texture, Color color) {
    check();

    if (this->current_texture_slot > this->max_texture_slots - 1) {
        render();
    }
    glActiveTexture(gl_texture_begin + this->current_texture_slot);
    glBindTexture(GL_TEXTURE_2D, texture->ID);

    // TOP LEFT
    vertices[index++] = {
        {0.0,  1.0},
        {0.0, 1.0},
        {color.r, color.g, color.b, color.a},
        (float)this->current_texture_slot,
        (float)Renderer::Sampler::Texture,
        {x, y, (float)texture->width, (float)texture->height}
    };
    // BOTTOM LEFT
    vertices[index++] = {
        {0.0,  0.0},
        {0.0, 0.0},
        {color.r, color.g, color.b, color.a},
        (float)this->current_texture_slot,
        (float)Renderer::Sampler::Texture,
        {x, y, (float)texture->width, (float)texture->height}
    };
    // BOTTOM RIGHT
    vertices[index++] = {
        {1.0,  0.0},
        {1.0, 0.0},
        {color.r, color.g, color.b, color.a},
        (float)this->current_texture_slot,
        (float)Renderer::Sampler::Texture,
        {x, y, (float)texture->width, (float)texture->height}
    };
    // TOP RIGHT
    vertices[index++] = {
        {1.0,  1.0},
        {1.0, 1.0},
        {color.r, color.g, color.b, color.a},
        (float)this->current_texture_slot,
        (float)Renderer::Sampler::Texture,
        {x, y, (float)texture->width, (float)texture->height}
    };
    count++;
    this->current_texture_slot++;
}

void Renderer::drawImageAtSize(float x, float y, Size size, Texture *texture, Color color) {
    check();

    if (this->current_texture_slot > this->max_texture_slots - 1) {
        render();
    }
    glActiveTexture(gl_texture_begin + this->current_texture_slot);
    glBindTexture(GL_TEXTURE_2D, texture->ID);

    // TOP LEFT
    vertices[index++] = {
        {0.0,  1.0},
        {0.0, 1.0},
        {color.r, color.g, color.b, color.a},
        (float)this->current_texture_slot,
        (float)Renderer::Sampler::Texture,
        {x, y, size.w, size.h}
    };
    // BOTTOM LEFT
    vertices[index++] = {
        {0.0,  0.0},
        {0.0, 0.0},
        {color.r, color.g, color.b, color.a},
        (float)this->current_texture_slot,
        (float)Renderer::Sampler::Texture,
        {x, y, size.w, size.h}
    };
    // BOTTOM RIGHT
    vertices[index++] = {
        {1.0,  0.0},
        {1.0, 0.0},
        {color.r, color.g, color.b, color.a},
        (float)this->current_texture_slot,
        (float)Renderer::Sampler::Texture,
        {x, y, size.w, size.h}
    };
    // TOP RIGHT
    vertices[index++] = {
        {1.0,  1.0},
        {1.0, 1.0},
        {color.r, color.g, color.b, color.a},
        (float)this->current_texture_slot,
        (float)Renderer::Sampler::Texture,
        {x, y, size.w, size.h}
    };
    count++;
    this->current_texture_slot++;
}

void Renderer::render() {
    shader.use();
    glBindVertexArray(VAO);
    glBindBuffer(GL_ARRAY_BUFFER, VBO);
    glBufferSubData(GL_ARRAY_BUFFER, 0, sizeof(Vertex) * index, vertices);
    glDrawElements(GL_TRIANGLES, 6 * count, GL_UNSIGNED_INT, 0);
    // TODO we could finally add a better line implementation, and also add a draw pixel implementation
    // by just using quads, this way we could keep it all in the same buffer
    // TODO later on we could introduce rounded corners and circles by sampling pixels in the fragment shader??
    reset();
}

void Renderer::fillRect(Rect rect, Color color) {
    check();
    
    // TOP LEFT
    vertices[index++] = {
        {0.0,  1.0},
        {0.0, 0.0},
        {color.r, color.g, color.b, color.a},
        0.0,
        (float)Renderer::Sampler::Color,
        {rect.x, rect.y, rect.w, rect.h}
    };
    // BOTTOM LEFT
    vertices[index++] = {
        {0.0,  0.0},
        {0.0, 0.0},
        {color.r, color.g, color.b, color.a},
        0.0,
        (float)Renderer::Sampler::Color,
        {rect.x, rect.y, rect.w, rect.h}
    };
    // BOTTOM RIGHT
    vertices[index++] = {
        {1.0,  0.0},
        {0.0, 0.0},
        {color.r, color.g, color.b, color.a},
        0.0,
        (float)Renderer::Sampler::Color,
        {rect.x, rect.y, rect.w, rect.h}
    };
    // TOP RIGHT
    vertices[index++] = {
        {1.0,  1.0},
        {0.0, 0.0},
        {color.r, color.g, color.b, color.a},
        0.0,
        (float)Renderer::Sampler::Color,
        {rect.x, rect.y, rect.w, rect.h}
    };

    count++;
}

void Renderer::fillGradientRect(Rect rect, Color fromColor, Color toColor, Gradient orientation) {
    check();
    
    switch (orientation) {
        case Gradient::TopToBottom: {
            // TOP LEFT
            vertices[index++] = {
                {0.0,  1.0},
                {0.0, 0.0},
                {toColor.r, toColor.g, toColor.b, toColor.a},
                0.0,
                (float)Renderer::Sampler::Color,
                {rect.x, rect.y, rect.w, rect.h}
            };
            // BOTTOM LEFT
            vertices[index++] = {
                {0.0,  0.0},
                {0.0, 0.0},
                {fromColor.r, fromColor.g, fromColor.b, fromColor.a},
                0.0,
                (float)Renderer::Sampler::Color,
                {rect.x, rect.y, rect.w, rect.h}
            };
            // BOTTOM RIGHT
            vertices[index++] = {
                {1.0,  0.0},
                {0.0, 0.0},
                {fromColor.r, fromColor.g, fromColor.b, fromColor.a},
                0.0,
                (float)Renderer::Sampler::Color,
                {rect.x, rect.y, rect.w, rect.h}
            };
            // TOP RIGHT
            vertices[index++] = {
                {1.0,  1.0},
                {0.0, 0.0},
                {toColor.r, toColor.g, toColor.b, toColor.a},
                0.0,
                (float)Renderer::Sampler::Color,
                {rect.x, rect.y, rect.w, rect.h}
            };
            break;
        }
        case Gradient::LeftToRight: {
            // TOP LEFT
            vertices[index++] = {
                {0.0,  1.0},
                {0.0, 0.0},
                {fromColor.r, fromColor.g, fromColor.b, fromColor.a},
                0.0,
                (float)Renderer::Sampler::Color,
                {rect.x, rect.y, rect.w, rect.h}
            };
            // BOTTOM LEFT
            vertices[index++] = {
                {0.0,  0.0},
                {0.0, 0.0},
                {fromColor.r, fromColor.g, fromColor.b, fromColor.a},
                0.0,
                (float)Renderer::Sampler::Color,
                {rect.x, rect.y, rect.w, rect.h}
            };
            // BOTTOM RIGHT
            vertices[index++] = {
                {1.0,  0.0},
                {0.0, 0.0},
                {toColor.r, toColor.g, toColor.b, toColor.a},
                0.0,
                (float)Renderer::Sampler::Color,
                {rect.x, rect.y, rect.w, rect.h}
            };
            // TOP RIGHT
            vertices[index++] = {
                {1.0,  1.0},
                {0.0, 0.0},
                {toColor.r, toColor.g, toColor.b, toColor.a},
                0.0,
                (float)Renderer::Sampler::Color,
                {rect.x, rect.y, rect.w, rect.h}
            };
            break;
        }
    }

    count++;
}