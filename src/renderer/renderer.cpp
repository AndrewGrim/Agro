#include "renderer.hpp"
#include "../application.hpp"

Renderer::Renderer(u32 *indices) {
    glGenVertexArrays(1, &VAO);
    glGenBuffers(1, &VBO);
    glGenBuffers(1, &EBO);

    glBindVertexArray(VAO);
    glBindBuffer(GL_ARRAY_BUFFER, VBO);
    glBufferData(GL_ARRAY_BUFFER, sizeof(Vertex) * MAX_BATCH_SIZE * QUAD_VERTEX_COUNT, nullptr, GL_DYNAMIC_DRAW);
    glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, EBO);
    glBufferData(GL_ELEMENT_ARRAY_BUFFER, sizeof(u32) * MAX_BATCH_SIZE * QUAD_INDEX_COUNT, indices, GL_STATIC_DRAW);

    glVertexAttribPointer(0, 2, GL_FLOAT, GL_FALSE, sizeof(Vertex), (void*)offsetof(Vertex, position));
    glEnableVertexAttribArray(0);

    glVertexAttribPointer(1, 2, GL_FLOAT, GL_FALSE, sizeof(Vertex), (void*)offsetof(Vertex, texture_uv));
    glEnableVertexAttribArray(1);

    glVertexAttribPointer(2, 4, GL_FLOAT, GL_FALSE, sizeof(Vertex), (void*)offsetof(Vertex, color));
    glEnableVertexAttribArray(2);

    glVertexAttribPointer(3, 1, GL_FLOAT, GL_FALSE, sizeof(Vertex), (void*)offsetof(Vertex, texture_index));
    glEnableVertexAttribArray(3);

    glVertexAttribPointer(4, 1, GL_FLOAT, GL_FALSE, sizeof(Vertex), (void*)offsetof(Vertex, texture_array_index));
    glEnableVertexAttribArray(4);

    glVertexAttribPointer(5, 1, GL_FLOAT, GL_FALSE, sizeof(Vertex), (void*)offsetof(Vertex, is_text));
    glEnableVertexAttribArray(5);

    glVertexAttribPointer(6, 4, GL_FLOAT, GL_FALSE, sizeof(Vertex), (void*)offsetof(Vertex, rect));
    glEnableVertexAttribArray(6);

    glVertexAttribPointer(7, 4, GL_FLOAT, GL_FALSE, sizeof(Vertex), (void*)offsetof(Vertex, clip_rect));
    glEnableVertexAttribArray(7);

    glGetIntegerv(GL_MAX_TEXTURE_IMAGE_UNITS, &max_texture_slots);
    glGetIntegerv(GL_MAX_ARRAY_TEXTURE_LAYERS, &max_texture_depth);
    glGetIntegerv(GL_MAX_TEXTURE_SIZE, &max_texture_size);

    /*
    Note that because we reserve two texture slots for loading and a texture array
    the texture indices that we submit in the renderer have to be offset to index
    into the texture sampler in this fragment shader (textures[index]).
    So while we still generate cases for all texture slots the first 2 will not be hit.
    */
    String fragment_shader = "#version 330 core\n";
        fragment_shader += "layout (origin_upper_left) in vec4 gl_FragCoord;\n";
        fragment_shader += "in vec2 v_texture_uv;\n";
        fragment_shader += "in vec4 v_color;\n";
        fragment_shader += "in float v_texture_slot_index;\n";
        fragment_shader += "in float v_texture_array_index;\n";
        fragment_shader += "in float v_sampler_type;\n";
        fragment_shader += "in vec4 v_rect;\n";
        fragment_shader += "in vec4 v_clip_rect;\n";
        fragment_shader += "\n";
        fragment_shader += "out vec4 f_color;\n";
        fragment_shader += "\n";
        fragment_shader += "uniform sampler2D textures[" + toString(max_texture_slots - AGRO_OPENGL_RESERVED_TEXTURE_SLOTS) + "];\n";
        fragment_shader += "uniform sampler2DArray texture_array;\n";
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
            fragment_shader += "case " + toString(i) + ":\n";
                fragment_shader += "switch (int(v_sampler_type)) {\n";
                    fragment_shader += "case 0: ";
                        fragment_shader += "sampled = vec4(1.0, 1.0, 1.0, 1.0);\n";
                        fragment_shader += "break;\n";
                    fragment_shader += "case 1: ";
                        fragment_shader += "sampled = vec4(texture(textures[" + toString(i > AGRO_OPENGL_RESERVED_FOR_TEXTURE_ARRAY ? i - AGRO_OPENGL_RESERVED_TEXTURE_SLOTS : i) + "], v_texture_uv));\n";
                        fragment_shader += "break;\n";
                    fragment_shader += "case 2: ";
                        fragment_shader += "sampled = vec4(1.0, 1.0, 1.0, texture(texture_array, vec3(v_texture_uv.xy, v_texture_array_index)).r);\n";
                        fragment_shader += "break;\n";
                    fragment_shader += "case 3: ";
                        // TODO this is acutally pretty good, however i feel like we might want it to be a bit thicker, and it would be nice if it could scale with border width
                        fragment_shader += "if (((int(gl_FragCoord.x) == int(v_rect.x)) || (int(gl_FragCoord.x) == int(v_rect.x + v_rect.z) - 1)) ||\n";
                        fragment_shader += "    ((int(gl_FragCoord.y) == int(v_rect.y)) || (int(gl_FragCoord.y) == int(v_rect.y + v_rect.w) - 1))) {\n";
                        fragment_shader += "    if ((int(gl_FragCoord.y) == int(v_rect.y) || int(gl_FragCoord.y) == int(v_rect.y) + int(v_rect.w) - 1) && int(gl_FragCoord.x) % 10 < 5) { discard; }\n";
                        fragment_shader += "    else if ((int(gl_FragCoord.x) == int(v_rect.x) || int(gl_FragCoord.x) == int(v_rect.x) + int(v_rect.z) - 1) && int(gl_FragCoord.y) % 10 < 5) { discard; }\n";
                        fragment_shader += "} else { discard; }\n";
                        fragment_shader += "sampled = vec4(1.0, 1.0, 1.0, 1.0);\n";
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
        "layout (location = 4) in float a_texture_array_index;\n"
        "layout (location = 5) in float a_sampler_type;\n"
        "layout (location = 6) in vec4 a_rect;\n"
        "layout (location = 7) in vec4 a_clip_rect;\n"
        "\n"
        "out vec2 v_texture_uv;\n"
        "out vec4 v_color;\n"
        "out float v_texture_slot_index;\n"
        "out float v_texture_array_index;\n"
        "out float v_sampler_type;\n"
        "out vec4 v_rect;\n"
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
            "v_texture_array_index = a_texture_array_index;\n"
            "v_sampler_type = a_sampler_type;\n"
            "v_rect = a_rect;\n"
            "v_clip_rect = a_clip_rect;\n"
        "}",
        fragment_shader.data()
    );

    shader.use();
    std::vector<i32> texture_indices;
    for (i32 i = AGRO_OPENGL_RESERVED_TEXTURE_SLOTS; i < max_texture_slots; i++) { texture_indices.push_back(i); }
    assert(texture_indices.size() == (u32)(max_texture_slots - AGRO_OPENGL_RESERVED_TEXTURE_SLOTS) && "The index count should be equivalent to the number of non-reserved texture slots.");
    auto loc = glGetUniformLocation(shader.ID, "textures");
    glUniform1iv(loc, texture_indices.size(), texture_indices.data());
    loc = glGetUniformLocation(shader.ID, "texture_array");
    glUniform1i(loc, AGRO_OPENGL_RESERVED_FOR_TEXTURE_ARRAY);
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
    current_texture_slot = AGRO_OPENGL_RESERVED_TEXTURE_SLOTS;
}

void Renderer::check() {
    if (index + QUAD_VERTEX_COUNT > MAX_BATCH_SIZE * QUAD_VERTEX_COUNT) render();
}

void Renderer::textCheck(std::shared_ptr<Font> font) {
    if (index + QUAD_VERTEX_COUNT > MAX_BATCH_SIZE * QUAD_VERTEX_COUNT) {
        render();
        glActiveTexture(gl_texture_begin + AGRO_OPENGL_RESERVED_FOR_TEXTURE_ARRAY);
        glBindTexture(GL_TEXTURE_2D_ARRAY, font->atlas_ID);
    }
}

void Renderer::fillText(std::shared_ptr<Font> font, Slice<const char> text, Point point, Color color, i32 tab_width, bool is_multiline, i32 line_spacing, Selection selection, Color selection_color) {
    if (selection.begin > selection.end) {
        auto temp = selection.end;
        selection.end = selection.begin;
        selection.begin = temp;
    }

    if (last_font != font.get()) {
        last_font = font.get();
        render();
    }
    glActiveTexture(gl_texture_begin + AGRO_OPENGL_RESERVED_FOR_TEXTURE_ARRAY);
    glBindTexture(GL_TEXTURE_2D_ARRAY, font->atlas_ID);
    i32 x = point.x;
    i32 base_bearing = font->max_bearing;
    i32 space_advance = font->get((u32)' ').advance;
    for (u64 i = 0; i < text.length;) {
        u8 c = text.data[i];
        u8 length = utf8::length(text.data + i);
        assert(length && "Invalid utf8 sequence start byte");
        textCheck(font);
        if (c == ' ') {
            x += space_advance;
        } else if (c == '\t') {
            x += space_advance * tab_width;
        } else if (c == '\n') {
            if (is_multiline) {
                point.y += font->maxHeight() + line_spacing;
                if (point.y >= clip_rect.y + clip_rect.h) { return; }
                x = point.x;
            }
        } else {
            Font::Character ch = font->get(utf8::decode(text.data + i, length));
            if (!is_multiline && x > clip_rect.x + clip_rect.w) { return; }
            auto _color = color;
            if (selection.begin != selection.end && (i >= selection.begin && i < selection.end)) { _color = selection_color; }
            if (x + ch.advance >= clip_rect.x && x <= clip_rect.x + clip_rect.w) {
                f32 xpos = x + ch.bearing.x;
                f32 ypos = point.y + (base_bearing - ch.bearing.y);

                f32 w = ch.size.w;
                f32 h = ch.size.h;

                // TOP LEFT
                vertices[index++] = {
                    {xpos, ypos + h},
                    {ch.texture_x, (h / font->atlas_height)},
                    {_color.r / 255.0f, _color.g / 255.0f, _color.b / 255.0f, _color.a / 255.0f},
                    (f32)AGRO_OPENGL_RESERVED_FOR_TEXTURE_ARRAY,
                    ch.texture_array_index,
                    (f32)Renderer::Sampler::Text,
                    {1.0f, 1.0f, 1.0f, 1.0f},
                    {(f32)clip_rect.x, (f32)clip_rect.y, (f32)clip_rect.w, (f32)clip_rect.h}
                };
                // BOTTOM LEFT
                vertices[index++] = {
                    {xpos, ypos},
                    {ch.texture_x, 0.0f},
                    {_color.r / 255.0f, _color.g / 255.0f, _color.b / 255.0f, _color.a / 255.0f},
                    (f32)AGRO_OPENGL_RESERVED_FOR_TEXTURE_ARRAY,
                    ch.texture_array_index,
                    (f32)Renderer::Sampler::Text,
                    {1.0f, 1.0f, 1.0f, 1.0f},
                    {(f32)clip_rect.x, (f32)clip_rect.y, (f32)clip_rect.w, (f32)clip_rect.h}
                };
                // BOTTOM RIGHT
                vertices[index++] = {
                    {xpos + w, ypos},
                    {ch.texture_x + (w / font->atlas_width), 0.0f},
                    {_color.r / 255.0f, _color.g / 255.0f, _color.b / 255.0f, _color.a / 255.0f},
                    (f32)AGRO_OPENGL_RESERVED_FOR_TEXTURE_ARRAY,
                    ch.texture_array_index,
                    (f32)Renderer::Sampler::Text,
                    {1.0f, 1.0f, 1.0f, 1.0f},
                    {(f32)clip_rect.x, (f32)clip_rect.y, (f32)clip_rect.w, (f32)clip_rect.h}
                };
                // TOP RIGHT
                vertices[index++] = {
                    {xpos + w, ypos + h},
                    {ch.texture_x + (w / font->atlas_width), (h / font->atlas_height)},
                    {_color.r / 255.0f, _color.g / 255.0f, _color.b / 255.0f, _color.a / 255.0f},
                    (f32)AGRO_OPENGL_RESERVED_FOR_TEXTURE_ARRAY,
                    ch.texture_array_index,
                    (f32)Renderer::Sampler::Text,
                    {1.0f, 1.0f, 1.0f, 1.0f},
                    {(f32)clip_rect.x, (f32)clip_rect.y, (f32)clip_rect.w, (f32)clip_rect.h}
                };
                quad_count++;
            }
            x += ch.advance;
        }
        i += length;
    }
}

Size Renderer::measureText(std::shared_ptr<Font> font, Slice<const char> text, i32 tab_width, bool is_multiline, i32 line_spacing) {
    Size size = Size(0, font->maxHeight());
    i32 line_width = 0;
    i32 space_advance = font->get((u32)' ').advance;
    for (u64 i = 0; i < text.length;) {
        u8 c = text.data[i];
        u8 length = utf8::length(text.data + i);
        assert(length != 0 && "Invalid utf8 sequence start byte");
        if (c == '\t') {
            line_width += space_advance * tab_width;
        } else if (c == '\n') {
            if (is_multiline) {
                size.h += font->maxHeight() + line_spacing;
                if (line_width > size.w) {
                    size.w = line_width;
                    line_width = 0;
                }
            }
        } else {
            line_width += font->get(utf8::decode(text.data + i, length)).advance;
        }
        i += length;
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
        {0.0f, 1.0f},
        {(f32)coords->top_left.x, (f32)coords->top_left.y},
        {color.r / 255.0f, color.g / 255.0f, color.b / 255.0f, color.a / 255.0f},
        (f32)current_texture_slot,
        0.0f,
        (f32)Renderer::Sampler::Texture,
        {(f32)point.x, (f32)point.y, (f32)size.w, (f32)size.h},
        {(f32)clip_rect.x, (f32)clip_rect.y, (f32)clip_rect.w, (f32)clip_rect.h}
    };
    // BOTTOM LEFT
    vertices[index++] = {
        {0.0f, 0.0f},
        {(f32)coords->bottom_left.x, (f32)coords->bottom_left.y},
        {color.r / 255.0f, color.g / 255.0f, color.b / 255.0f, color.a / 255.0f},
        (f32)current_texture_slot,
        0.0f,
        (f32)Renderer::Sampler::Texture,
        {(f32)point.x, (f32)point.y, (f32)size.w, (f32)size.h},
        {(f32)clip_rect.x, (f32)clip_rect.y, (f32)clip_rect.w, (f32)clip_rect.h}
    };
    // BOTTOM RIGHT
    vertices[index++] = {
        {1.0f, 0.0f},
        {(f32)coords->bottom_right.x, (f32)coords->bottom_right.y},
        {color.r / 255.0f, color.g / 255.0f, color.b / 255.0f, color.a / 255.0f},
        (f32)current_texture_slot,
        0.0f,
        (f32)Renderer::Sampler::Texture,
        {(f32)point.x, (f32)point.y, (f32)size.w, (f32)size.h},
        {(f32)clip_rect.x, (f32)clip_rect.y, (f32)clip_rect.w, (f32)clip_rect.h}
    };
    // TOP RIGHT
    vertices[index++] = {
        {1.0f, 1.0f},
        {(f32)coords->top_right.x, (f32)coords->top_right.y},
        {color.r / 255.0f, color.g / 255.0f, color.b / 255.0f, color.a / 255.0f},
        (f32)current_texture_slot,
        0.0f,
        (f32)Renderer::Sampler::Texture,
        {(f32)point.x, (f32)point.y, (f32)size.w, (f32)size.h},
        {(f32)clip_rect.x, (f32)clip_rect.y, (f32)clip_rect.w, (f32)clip_rect.h}
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
        {0.0f, 1.0f},
        {0.0f, 0.0f},
        {color.r / 255.0f, color.g / 255.0f, color.b / 255.0f, color.a / 255.0f},
        0.0f,
        0.0f,
        (f32)Renderer::Sampler::Color,
        {(f32)rect.x, (f32)rect.y, (f32)rect.w, (f32)rect.h},
        {(f32)clip_rect.x, (f32)clip_rect.y, (f32)clip_rect.w, (f32)clip_rect.h}
    };
    // BOTTOM LEFT
    vertices[index++] = {
        {0.0f, 0.0f},
        {0.0f, 0.0f},
        {color.r / 255.0f, color.g / 255.0f, color.b / 255.0f, color.a / 255.0f},
        0.0f,
        0.0f,
        (f32)Renderer::Sampler::Color,
        {(f32)rect.x, (f32)rect.y, (f32)rect.w, (f32)rect.h},
        {(f32)clip_rect.x, (f32)clip_rect.y, (f32)clip_rect.w, (f32)clip_rect.h}
    };
    // BOTTOM RIGHT
    vertices[index++] = {
        {1.0f, 0.0f},
        {0.0f, 0.0f},
        {color.r / 255.0f, color.g / 255.0f, color.b / 255.0f, color.a / 255.0f},
        0.0f,
        0.0f,
        (f32)Renderer::Sampler::Color,
        {(f32)rect.x, (f32)rect.y, (f32)rect.w, (f32)rect.h},
        {(f32)clip_rect.x, (f32)clip_rect.y, (f32)clip_rect.w, (f32)clip_rect.h}
    };
    // TOP RIGHT
    vertices[index++] = {
        {1.0f, 1.0f},
        {0.0f, 0.0f},
        {color.r / 255.0f, color.g / 255.0f, color.b / 255.0f, color.a / 255.0f},
        0.0f,
        0.0f,
        (f32)Renderer::Sampler::Color,
        {(f32)rect.x, (f32)rect.y, (f32)rect.w, (f32)rect.h},
        {(f32)clip_rect.x, (f32)clip_rect.y, (f32)clip_rect.w, (f32)clip_rect.h}
    };

    quad_count++;
}

void Renderer::fillRectWithGradient(Rect rect, Color fromColor, Color toColor, Gradient orientation) {
    check();

    switch (orientation) {
        case Gradient::TopToBottom: {
            // TOP LEFT
            vertices[index++] = {
                {0.0f, 1.0f},
                {0.0f, 0.0f},
                {toColor.r / 255.0f, toColor.g / 255.0f, toColor.b / 255.0f, toColor.a / 255.0f},
                0.0f,
                0.0f,
                (f32)Renderer::Sampler::Color,
                {(f32)rect.x, (f32)rect.y, (f32)rect.w, (f32)rect.h},
                {(f32)clip_rect.x, (f32)clip_rect.y, (f32)clip_rect.w, (f32)clip_rect.h}
            };
            // BOTTOM LEFT
            vertices[index++] = {
                {0.0f, 0.0f},
                {0.0f, 0.0f},
                {fromColor.r / 255.0f, fromColor.g / 255.0f, fromColor.b / 255.0f, fromColor.a / 255.0f},
                0.0f,
                0.0f,
                (f32)Renderer::Sampler::Color,
                {(f32)rect.x, (f32)rect.y, (f32)rect.w, (f32)rect.h},
                {(f32)clip_rect.x, (f32)clip_rect.y, (f32)clip_rect.w, (f32)clip_rect.h}
            };
            // BOTTOM RIGHT
            vertices[index++] = {
                {1.0f, 0.0f},
                {0.0f, 0.0f},
                {fromColor.r / 255.0f, fromColor.g / 255.0f, fromColor.b / 255.0f, fromColor.a / 255.0f},
                0.0f,
                0.0f,
                (f32)Renderer::Sampler::Color,
                {(f32)rect.x, (f32)rect.y, (f32)rect.w, (f32)rect.h},
                {(f32)clip_rect.x, (f32)clip_rect.y, (f32)clip_rect.w, (f32)clip_rect.h}
            };
            // TOP RIGHT
            vertices[index++] = {
                {1.0f, 1.0f},
                {0.0f, 0.0f},
                {toColor.r / 255.0f, toColor.g / 255.0f, toColor.b / 255.0f, toColor.a / 255.0f},
                0.0f,
                0.0f,
                (f32)Renderer::Sampler::Color,
                {(f32)rect.x, (f32)rect.y, (f32)rect.w, (f32)rect.h},
                {(f32)clip_rect.x, (f32)clip_rect.y, (f32)clip_rect.w, (f32)clip_rect.h}
            };
            break;
        }
        case Gradient::LeftToRight: {
            // TOP LEFT
            vertices[index++] = {
                {0.0f, 1.0f},
                {0.0f, 0.0f},
                {fromColor.r / 255.0f, fromColor.g / 255.0f, fromColor.b / 255.0f, fromColor.a / 255.0f},
                0.0f,
                0.0f,
                (f32)Renderer::Sampler::Color,
                {(f32)rect.x, (f32)rect.y, (f32)rect.w, (f32)rect.h},
                {(f32)clip_rect.x, (f32)clip_rect.y, (f32)clip_rect.w, (f32)clip_rect.h}
            };
            // BOTTOM LEFT
            vertices[index++] = {
                {0.0f, 0.0f},
                {0.0f, 0.0f},
                {fromColor.r / 255.0f, fromColor.g / 255.0f, fromColor.b / 255.0f, fromColor.a / 255.0f},
                0.0f,
                0.0f,
                (f32)Renderer::Sampler::Color,
                {(f32)rect.x, (f32)rect.y, (f32)rect.w, (f32)rect.h},
                {(f32)clip_rect.x, (f32)clip_rect.y, (f32)clip_rect.w, (f32)clip_rect.h}
            };
            // BOTTOM RIGHT
            vertices[index++] = {
                {1.0f, 0.0f},
                {0.0f, 0.0f},
                {toColor.r / 255.0f, toColor.g / 255.0f, toColor.b / 255.0f, toColor.a / 255.0f},
                0.0f,
                0.0f,
                (f32)Renderer::Sampler::Color,
                {(f32)rect.x, (f32)rect.y, (f32)rect.w, (f32)rect.h},
                {(f32)clip_rect.x, (f32)clip_rect.y, (f32)clip_rect.w, (f32)clip_rect.h}
            };
            // TOP RIGHT
            vertices[index++] = {
                {1.0f, 1.0f},
                {0.0f, 0.0f},
                {toColor.r / 255.0f, toColor.g / 255.0f, toColor.b / 255.0f, toColor.a / 255.0f},
                0.0f,
                0.0f,
                (f32)Renderer::Sampler::Color,
                {(f32)rect.x, (f32)rect.y, (f32)rect.w, (f32)rect.h},
                {(f32)clip_rect.x, (f32)clip_rect.y, (f32)clip_rect.w, (f32)clip_rect.h}
            };
            break;
        }
    }

    quad_count++;
}

void Renderer::drawDashedRect(Rect rect, Color color) {
    check();

    // TOP LEFT
    vertices[index++] = {
        {0.0f, 1.0f},
        {0.0f, 0.0f},
        {color.r / 255.0f, color.g / 255.0f, color.b / 255.0f, color.a / 255.0f},
        0.0f,
        0.0f,
        (f32)Renderer::Sampler::Dashed,
        {(f32)rect.x, (f32)rect.y, (f32)rect.w, (f32)rect.h},
        {(f32)clip_rect.x, (f32)clip_rect.y, (f32)clip_rect.w, (f32)clip_rect.h}
    };
    // BOTTOM LEFT
    vertices[index++] = {
        {0.0f, 0.0f},
        {0.0f, 0.0f},
        {color.r / 255.0f, color.g / 255.0f, color.b / 255.0f, color.a / 255.0f},
        0.0f,
        0.0f,
        (f32)Renderer::Sampler::Dashed,
        {(f32)rect.x, (f32)rect.y, (f32)rect.w, (f32)rect.h},
        {(f32)clip_rect.x, (f32)clip_rect.y, (f32)clip_rect.w, (f32)clip_rect.h}
    };
    // BOTTOM RIGHT
    vertices[index++] = {
        {1.0f, 0.0f},
        {0.0f, 0.0f},
        {color.r / 255.0f, color.g / 255.0f, color.b / 255.0f, color.a / 255.0f},
        0.0f,
        0.0f,
        (f32)Renderer::Sampler::Dashed,
        {(f32)rect.x, (f32)rect.y, (f32)rect.w, (f32)rect.h},
        {(f32)clip_rect.x, (f32)clip_rect.y, (f32)clip_rect.w, (f32)clip_rect.h}
    };
    // TOP RIGHT
    vertices[index++] = {
        {1.0f, 1.0f},
        {0.0f, 0.0f},
        {color.r / 255.0f, color.g / 255.0f, color.b / 255.0f, color.a / 255.0f},
        0.0f,
        0.0f,
        (f32)Renderer::Sampler::Dashed,
        {(f32)rect.x, (f32)rect.y, (f32)rect.w, (f32)rect.h},
        {(f32)clip_rect.x, (f32)clip_rect.y, (f32)clip_rect.w, (f32)clip_rect.h}
    };

    quad_count++;
}
