#include "renderer.hpp"
#include "../application.hpp"

Renderer::Renderer(unsigned int *indices) {
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

    glVertexAttribPointer(1, 2, GL_FLOAT, GL_FALSE, sizeof(Vertex), (void*)offsetof(Vertex, texture_uv));
    glEnableVertexAttribArray(1);

    glVertexAttribPointer(2, 4, GL_FLOAT, GL_FALSE, sizeof(Vertex), (void*)offsetof(Vertex, color));
    glEnableVertexAttribArray(2);

    glVertexAttribPointer(3, 1, GL_FLOAT, GL_FALSE, sizeof(Vertex), (void*)offsetof(Vertex, texture_index));
    glEnableVertexAttribArray(3);

    glVertexAttribPointer(4, 1, GL_FLOAT, GL_FALSE, sizeof(Vertex), (void*)offsetof(Vertex, is_text));
    glEnableVertexAttribArray(4);

    glVertexAttribPointer(5, 4, GL_FLOAT, GL_FALSE, sizeof(Vertex), (void*)offsetof(Vertex, rect));
    glEnableVertexAttribArray(5);

    glVertexAttribPointer(6, 4, GL_FLOAT, GL_FALSE, sizeof(Vertex), (void*)offsetof(Vertex, clip_rect));
    glEnableVertexAttribArray(6);

    glGetIntegerv(GL_MAX_TEXTURE_IMAGE_UNITS, &max_texture_slots);

    std::string fragment_shader = "#version 330 core\n";
        fragment_shader += "layout (origin_upper_left) in vec4 gl_FragCoord;\n";
        fragment_shader += "in vec2 v_texture_uv;\n";
        fragment_shader += "in vec4 v_color;\n";
        fragment_shader += "in float v_texture_slot_index;\n";
        fragment_shader += "in float v_sampler_type;\n";
        fragment_shader += "in vec4 v_clip_rect;\n";
        fragment_shader += "\n";
        fragment_shader += "out vec4 f_color;\n";
        fragment_shader += "\n";
        fragment_shader += "uniform sampler2D textures[" + std::to_string(max_texture_slots) + "];\n";
        fragment_shader += "\n";
        fragment_shader += "void main()\n";
        fragment_shader += "{\n";
        fragment_shader += "if ((gl_FragCoord.x < v_clip_rect.x || gl_FragCoord.y < v_clip_rect.y) ||\n";
        fragment_shader += "    (gl_FragCoord.x > (v_clip_rect.x + v_clip_rect.z) || gl_FragCoord.y > (v_clip_rect.y + v_clip_rect.w))) {\n";
        fragment_shader += "discard;\n";
        fragment_shader += "}\n";
        fragment_shader += "vec4 sampled;\n";
        fragment_shader += "switch (int(v_texture_slot_index)) {\n";
        for (int i = 0; i < max_texture_slots; i++) {
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

    shader = Shader(
        "#version 330 core\n"
        "layout (location = 0) in vec2 a_opengl_position;\n"
        "layout (location = 1) in vec2 a_texture_uv;\n"
        "layout (location = 2) in vec4 a_color;\n"
        "layout (location = 3) in float a_texture_slot_index;\n"
        "layout (location = 4) in float a_sampler_type;\n"
        "layout (location = 5) in vec4 a_rect;\n"
        "layout (location = 6) in vec4 a_clip_rect;\n"
        "\n"
        "out vec2 v_texture_uv;\n"
        "out vec4 v_color;\n"
        "out float v_texture_slot_index;\n"
        "out float v_sampler_type;\n"
        "out vec4 v_clip_rect;\n"
        "\n"
        "uniform mat4 u_projection;\n"
        "\n"
        "void main()\n"
        "{\n"
            "mat4 model = mat4(\n"
                "vec4(a_rect.z, 0.0, 0.0, 0.0),\n"
                "vec4(0.0, a_rect.w, 0.0, 0.0),\n"
                "vec4(0.0, 0.0, 1.0, 0.0),\n"
                "vec4(a_rect.x, a_rect.y, 0.0, 1.0)\n"
            ");\n"
            "gl_Position = u_projection * model * vec4(a_opengl_position, 1.0, 1.0);\n"
            "v_texture_uv = a_texture_uv;\n"
            "v_color = a_color;\n"
            "v_texture_slot_index = a_texture_slot_index;\n"
            "v_sampler_type = a_sampler_type;\n"
            "v_clip_rect = a_clip_rect;\n"
        "}",
        fragment_shader.c_str()
    );

    shader.use();
    std::vector<int> texture_indices;
    for (int i = 0; i < 32; i++) {
        texture_indices.push_back(i);
    }
    auto loc = glGetUniformLocation(shader.ID, "textures");
    glUniform1iv(loc, 32, texture_indices.data());
}

Renderer::~Renderer() {
    glDeleteVertexArrays(1, &VAO);
    glDeleteBuffers(1, &VBO);
    glDeleteBuffers(1, &EBO);
    delete[] vertices;
}

void Renderer::reset() {
    index = 0;
    quad_count = 0;
    current_texture_slot = 2;
}

void Renderer::check() {
    if (index + QUAD_VERTEX_COUNT > MAX_BATCH_SIZE * QUAD_VERTEX_COUNT) render();
}

void Renderer::textCheck(Font *font) {
    if (index + QUAD_VERTEX_COUNT > MAX_BATCH_SIZE * QUAD_VERTEX_COUNT) {
        render();
        glActiveTexture(gl_texture_begin + current_texture_slot);
        glBindTexture(GL_TEXTURE_2D, font->atlas_ID);
    }
}

void Renderer::fillText(Font *font, const char *text, size_t text_length, Point point, Color color, int tab_width) {
    Size window = Application::get()->size;
    // TODO handle tab characters

    if (current_texture_slot > max_texture_slots - 1) {
        render();
    }
    glActiveTexture(gl_texture_begin + current_texture_slot);
    glBindTexture(GL_TEXTURE_2D, font->atlas_ID);
    for (size_t i = 0; i < text_length && point.x <= window.w; i++) {
        char c = text[i];
        textCheck(font);
        Font::Character ch = font->characters[c];
        int advance = ch.advance;
        if (c == '\t') { advance = font->characters[' '].advance * tab_width; }
        if (point.x + advance >= 0) {
            float xpos = point.x + ch.bearing.x;
            float ypos = point.y + (font->characters['H'].bearing.y - ch.bearing.y);

            float w = ch.size.w;
            float h = ch.size.h;

            // TOP LEFT
            vertices[index++] = {
                {xpos, ypos + h},
                {ch.texture_x, (h / font->atlas_height)},
                {color.r, color.g, color.b, color.a},
                (float)current_texture_slot,
                (float)Renderer::Sampler::Text,
                {1.0, 1.0, 1.0, 1.0},
                {(float)clip_rect.x, (float)clip_rect.y, (float)clip_rect.w, (float)clip_rect.h}
            };
            // BOTTOM LEFT
            vertices[index++] = {
                {xpos, ypos},
                {ch.texture_x, 0.0},
                {color.r, color.g, color.b, color.a},
                (float)current_texture_slot,
                (float)Renderer::Sampler::Text,
                {1.0, 1.0, 1.0, 1.0},
                {(float)clip_rect.x, (float)clip_rect.y, (float)clip_rect.w, (float)clip_rect.h}
            };
            // BOTTOM RIGHT
            vertices[index++] = {
                {xpos + w, ypos},
                {ch.texture_x + (w / font->atlas_width), 0.0},
                {color.r, color.g, color.b, color.a},
                (float)current_texture_slot,
                (float)Renderer::Sampler::Text,
                {1.0, 1.0, 1.0, 1.0},
                {(float)clip_rect.x, (float)clip_rect.y, (float)clip_rect.w, (float)clip_rect.h}
            };
            // TOP RIGHT
            vertices[index++] = {
                {xpos + w, ypos + h},
                {ch.texture_x + (w / font->atlas_width), (h / font->atlas_height)},
                {color.r, color.g, color.b, color.a},
                (float)current_texture_slot,
                (float)Renderer::Sampler::Text,
                {1.0, 1.0, 1.0, 1.0},
                {(float)clip_rect.x, (float)clip_rect.y, (float)clip_rect.w, (float)clip_rect.h}
            };
            quad_count++;
        }
        point.x += advance;
    }
    current_texture_slot++;
}

void Renderer::fillTextMultiline(Font *font, std::string text, Point point, Color color, int line_spacing, int tab_width) {
    Size window = Application::get()->size;
    std::string::const_iterator c;
    // TODO handle tab characters

    if (current_texture_slot > max_texture_slots - 1) {
        render();
    }
    glActiveTexture(gl_texture_begin + current_texture_slot);
    glBindTexture(GL_TEXTURE_2D, font->atlas_ID);
    int x = point.x;
    for (c = text.begin(); c != text.end() && point.x <= window.w; c++) {
        textCheck(font);
        Font::Character ch = font->characters[*c];
        int advance = ch.advance;
        if (*c == '\t') { advance = font->characters[' '].advance * tab_width; }
        if (x + advance >= 0) {
            if (*c == '\n') {
                point.y += font->max_height + line_spacing;
                x = point.x;
            }
            float xpos = x + ch.bearing.x;
            float ypos = point.y + (font->characters['H'].bearing.y - ch.bearing.y);

            float w = ch.size.w;
            float h = ch.size.h;

            // TOP LEFT
            vertices[index++] = {
                {xpos, ypos + h},
                {ch.texture_x, (h / font->atlas_height)},
                {color.r, color.g, color.b, color.a},
                (float)current_texture_slot,
                (float)Renderer::Sampler::Text,
                {1.0, 1.0, 1.0, 1.0},
                {(float)clip_rect.x, (float)clip_rect.y, (float)clip_rect.w, (float)clip_rect.h}
            };
            // BOTTOM LEFT
            vertices[index++] = {
                {xpos, ypos},
                {ch.texture_x, 0.0},
                {color.r, color.g, color.b, color.a},
                (float)current_texture_slot,
                (float)Renderer::Sampler::Text,
                {1.0, 1.0, 1.0, 1.0},
                {(float)clip_rect.x, (float)clip_rect.y, (float)clip_rect.w, (float)clip_rect.h}
            };
            // BOTTOM RIGHT
            vertices[index++] = {
                {xpos + w, ypos},
                {ch.texture_x + (w / font->atlas_width), 0.0},
                {color.r, color.g, color.b, color.a},
                (float)current_texture_slot,
                (float)Renderer::Sampler::Text,
                {1.0, 1.0, 1.0, 1.0},
                {(float)clip_rect.x, (float)clip_rect.y, (float)clip_rect.w, (float)clip_rect.h}
            };
            // TOP RIGHT
            vertices[index++] = {
                {xpos + w, ypos + h},
                {ch.texture_x + (w / font->atlas_width), (h / font->atlas_height)},
                {color.r, color.g, color.b, color.a},
                (float)current_texture_slot,
                (float)Renderer::Sampler::Text,
                {1.0, 1.0, 1.0, 1.0},
                {(float)clip_rect.x, (float)clip_rect.y, (float)clip_rect.w, (float)clip_rect.h}
            };
            quad_count++;
        }
        x += advance;
    }
    current_texture_slot++;
}

Size Renderer::measureText(Font *font, std::string text, int tab_width) {
    Size size = Size(0, font->max_height);
    for (char c : text) {
        Font::Character ch = font->characters[c];
        if (c == '\t') {
            size.w += font->characters[' '].advance * tab_width;
        } else {
            size.w += ch.advance;
        }
    }

    return size;
}

Size Renderer::measureText(Font *font, char c, int tab_width) {
    Size size = Size(0, font->max_height);
    Font::Character ch = font->characters[c];
    if (c == '\t') {
        size.w = font->characters[' '].advance * tab_width;
    } else {
        size.w = ch.advance;
    }

    return size;
}

Size Renderer::measureTextMultiline(Font *font, std::string text, int line_spacing, int tab_width) {
    Size size = Size(0, font->max_height);
    int line_width = 0;
    for (char c : text) {
        Font::Character ch = font->characters[c];
        if (c == '\t') {
            line_width += font->characters[' '].advance * tab_width;
        } else {
            line_width += ch.advance;
        }
        if (c == '\n') {
            size.h += font->max_height + line_spacing;
            if (line_width > size.w) {
                size.w = line_width;
                line_width = 0;
            }
        }
    }
    if (line_width > size.w) { size.w = line_width; }

    return size;
}

void Renderer::drawTexture(Point point, Size size, Texture *texture, TextureCoordinates *coords, Color color) {
    check();

    if (current_texture_slot > max_texture_slots - 1) {
        render();
    }
    glActiveTexture(gl_texture_begin + current_texture_slot);
    glBindTexture(GL_TEXTURE_2D, texture->ID);

    // TOP LEFT
    vertices[index++] = {
        {0.0, 1.0},
        {(float)coords->top_left.x, (float)coords->top_left.y},
        {color.r, color.g, color.b, color.a},
        (float)current_texture_slot,
        (float)Renderer::Sampler::Texture,
        {(float)point.x, (float)point.y, (float)size.w, (float)size.h},
        {(float)clip_rect.x, (float)clip_rect.y, (float)clip_rect.w, (float)clip_rect.h}
    };
    // BOTTOM LEFT
    vertices[index++] = {
        {0.0, 0.0},
        {(float)coords->bottom_left.x, (float)coords->bottom_left.y},
        {color.r, color.g, color.b, color.a},
        (float)current_texture_slot,
        (float)Renderer::Sampler::Texture,
        {(float)point.x, (float)point.y, (float)size.w, (float)size.h},
        {(float)clip_rect.x, (float)clip_rect.y, (float)clip_rect.w, (float)clip_rect.h}
    };
    // BOTTOM RIGHT
    vertices[index++] = {
        {1.0, 0.0},
        {(float)coords->bottom_right.x, (float)coords->bottom_right.y},
        {color.r, color.g, color.b, color.a},
        (float)current_texture_slot,
        (float)Renderer::Sampler::Texture,
        {(float)point.x, (float)point.y, (float)size.w, (float)size.h},
        {(float)clip_rect.x, (float)clip_rect.y, (float)clip_rect.w, (float)clip_rect.h}
    };
    // TOP RIGHT
    vertices[index++] = {
        {1.0, 1.0},
        {(float)coords->top_right.x, (float)coords->top_right.y},
        {color.r, color.g, color.b, color.a},
        (float)current_texture_slot,
        (float)Renderer::Sampler::Texture,
        {(float)point.x, (float)point.y, (float)size.w, (float)size.h},
        {(float)clip_rect.x, (float)clip_rect.y, (float)clip_rect.w, (float)clip_rect.h}
    };
    quad_count++;
    current_texture_slot++;
}

void Renderer::render() {
    shader.use();
    glBindVertexArray(VAO);
    glBindBuffer(GL_ARRAY_BUFFER, VBO);
    glBufferSubData(GL_ARRAY_BUFFER, 0, sizeof(Vertex) * index, vertices);
    glDrawElements(GL_TRIANGLES, QUAD_INDEX_COUNT * quad_count, GL_UNSIGNED_INT, 0);
    // TODO later on we could introduce rounded corners and circles by sampling pixels in the fragment shader??
    reset();
}

void Renderer::fillRect(Rect rect, Color color) {
    check();

    // TOP LEFT
    vertices[index++] = {
        {0.0, 1.0},
        {0.0, 0.0},
        {color.r, color.g, color.b, color.a},
        0.0,
        (float)Renderer::Sampler::Color,
        {(float)rect.x, (float)rect.y, (float)rect.w, (float)rect.h},
        {(float)clip_rect.x, (float)clip_rect.y, (float)clip_rect.w, (float)clip_rect.h}
    };
    // BOTTOM LEFT
    vertices[index++] = {
        {0.0, 0.0},
        {0.0, 0.0},
        {color.r, color.g, color.b, color.a},
        0.0,
        (float)Renderer::Sampler::Color,
        {(float)rect.x, (float)rect.y, (float)rect.w, (float)rect.h},
        {(float)clip_rect.x, (float)clip_rect.y, (float)clip_rect.w, (float)clip_rect.h}
    };
    // BOTTOM RIGHT
    vertices[index++] = {
        {1.0, 0.0},
        {0.0, 0.0},
        {color.r, color.g, color.b, color.a},
        0.0,
        (float)Renderer::Sampler::Color,
        {(float)rect.x, (float)rect.y, (float)rect.w, (float)rect.h},
        {(float)clip_rect.x, (float)clip_rect.y, (float)clip_rect.w, (float)clip_rect.h}
    };
    // TOP RIGHT
    vertices[index++] = {
        {1.0, 1.0},
        {0.0, 0.0},
        {color.r, color.g, color.b, color.a},
        0.0,
        (float)Renderer::Sampler::Color,
        {(float)rect.x, (float)rect.y, (float)rect.w, (float)rect.h},
        {(float)clip_rect.x, (float)clip_rect.y, (float)clip_rect.w, (float)clip_rect.h}
    };

    quad_count++;
}

void Renderer::fillRectWithGradient(Rect rect, Color fromColor, Color toColor, Gradient orientation) {
    check();

    switch (orientation) {
        case Gradient::TopToBottom: {
            // TOP LEFT
            vertices[index++] = {
                {0.0, 1.0},
                {0.0, 0.0},
                {toColor.r, toColor.g, toColor.b, toColor.a},
                0.0,
                (float)Renderer::Sampler::Color,
                {(float)rect.x, (float)rect.y, (float)rect.w, (float)rect.h},
                {(float)clip_rect.x, (float)clip_rect.y, (float)clip_rect.w, (float)clip_rect.h}
            };
            // BOTTOM LEFT
            vertices[index++] = {
                {0.0, 0.0},
                {0.0, 0.0},
                {fromColor.r, fromColor.g, fromColor.b, fromColor.a},
                0.0,
                (float)Renderer::Sampler::Color,
                {(float)rect.x, (float)rect.y, (float)rect.w, (float)rect.h},
                {(float)clip_rect.x, (float)clip_rect.y, (float)clip_rect.w, (float)clip_rect.h}
            };
            // BOTTOM RIGHT
            vertices[index++] = {
                {1.0, 0.0},
                {0.0, 0.0},
                {fromColor.r, fromColor.g, fromColor.b, fromColor.a},
                0.0,
                (float)Renderer::Sampler::Color,
                {(float)rect.x, (float)rect.y, (float)rect.w, (float)rect.h},
                {(float)clip_rect.x, (float)clip_rect.y, (float)clip_rect.w, (float)clip_rect.h}
            };
            // TOP RIGHT
            vertices[index++] = {
                {1.0, 1.0},
                {0.0, 0.0},
                {toColor.r, toColor.g, toColor.b, toColor.a},
                0.0,
                (float)Renderer::Sampler::Color,
                {(float)rect.x, (float)rect.y, (float)rect.w, (float)rect.h},
                {(float)clip_rect.x, (float)clip_rect.y, (float)clip_rect.w, (float)clip_rect.h}
            };
            break;
        }
        case Gradient::LeftToRight: {
            // TOP LEFT
            vertices[index++] = {
                {0.0, 1.0},
                {0.0, 0.0},
                {fromColor.r, fromColor.g, fromColor.b, fromColor.a},
                0.0,
                (float)Renderer::Sampler::Color,
                {(float)rect.x, (float)rect.y, (float)rect.w, (float)rect.h},
                {(float)clip_rect.x, (float)clip_rect.y, (float)clip_rect.w, (float)clip_rect.h}
            };
            // BOTTOM LEFT
            vertices[index++] = {
                {0.0, 0.0},
                {0.0, 0.0},
                {fromColor.r, fromColor.g, fromColor.b, fromColor.a},
                0.0,
                (float)Renderer::Sampler::Color,
                {(float)rect.x, (float)rect.y, (float)rect.w, (float)rect.h},
                {(float)clip_rect.x, (float)clip_rect.y, (float)clip_rect.w, (float)clip_rect.h}
            };
            // BOTTOM RIGHT
            vertices[index++] = {
                {1.0, 0.0},
                {0.0, 0.0},
                {toColor.r, toColor.g, toColor.b, toColor.a},
                0.0,
                (float)Renderer::Sampler::Color,
                {(float)rect.x, (float)rect.y, (float)rect.w, (float)rect.h},
                {(float)clip_rect.x, (float)clip_rect.y, (float)clip_rect.w, (float)clip_rect.h}
            };
            // TOP RIGHT
            vertices[index++] = {
                {1.0, 1.0},
                {0.0, 0.0},
                {toColor.r, toColor.g, toColor.b, toColor.a},
                0.0,
                (float)Renderer::Sampler::Color,
                {(float)rect.x, (float)rect.y, (float)rect.w, (float)rect.h},
                {(float)clip_rect.x, (float)clip_rect.y, (float)clip_rect.w, (float)clip_rect.h}
            };
            break;
        }
    }

    quad_count++;
}
