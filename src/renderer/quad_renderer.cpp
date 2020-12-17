#include "quad_renderer.hpp"

QuadRenderer::QuadRenderer(unsigned int *indices) {
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
    
    glVertexAttribPointer(1, 4, GL_FLOAT, GL_FALSE, sizeof(Vertex), (void*)offsetof(Vertex, color));
    glEnableVertexAttribArray(1);
    
    glVertexAttribPointer(2, 4, GL_FLOAT, GL_FALSE, sizeof(Vertex), (void*)offsetof(Vertex, rect));
    glEnableVertexAttribArray(2);
}

QuadRenderer::~QuadRenderer() {
    glDeleteVertexArrays(1, &VAO);
    glDeleteBuffers(1, &VBO);
    glDeleteBuffers(1, &EBO);
    delete[] vertices;
}

void QuadRenderer::reset() {
    index = 0;
    count = 0;
}

void QuadRenderer::check() {
    if (index + QUAD_VERTEX_COUNT > MAX_BATCH_SIZE * QUAD_VERTEX_COUNT) render();
}

void QuadRenderer::fillRect(Rect rect, Color color) {
    check();
    
    vertices[index++] = {
        {0.0,  1.0},
        {color.r, color.g, color.b, color.a},
        {rect.x, rect.y, rect.w, rect.h}
    };
    vertices[index++] = {
        {0.0,  0.0},
        {color.r, color.g, color.b, color.a},
        {rect.x, rect.y, rect.w, rect.h}
    };
    vertices[index++] = {
        {1.0,  0.0},
        {color.r, color.g, color.b, color.a},
        {rect.x, rect.y, rect.w, rect.h}
    };
    vertices[index++] = {
        {1.0,  1.0},
        {color.r, color.g, color.b, color.a},
        {rect.x, rect.y, rect.w, rect.h}
    };

    count++;
}

void QuadRenderer::fillGradientRect(Rect rect, Color fromColor, Color toColor, Gradient orientation) {
    check();
    
    switch (orientation) {
        case Gradient::TopToBottom: {
            vertices[index++] = {
                {0.0,  1.0},
                {toColor.r, toColor.g, toColor.b, toColor.a},
                {rect.x, rect.y, rect.w, rect.h}
            };
            vertices[index++] = {
                {0.0,  0.0},
                {fromColor.r, fromColor.g, fromColor.b, fromColor.a},
                {rect.x, rect.y, rect.w, rect.h}
            };
            vertices[index++] = {
                {1.0,  0.0},
                {fromColor.r, fromColor.g, fromColor.b, fromColor.a},
                {rect.x, rect.y, rect.w, rect.h}
            };
            vertices[index++] = {
                {1.0,  1.0},
                {toColor.r, toColor.g, toColor.b, toColor.a},
                {rect.x, rect.y, rect.w, rect.h}
            };
            break;
        }
        case Gradient::LeftToRight: {
            vertices[index++] = {
                {0.0,  1.0},
                {fromColor.r, fromColor.g, fromColor.b, fromColor.a},
                {rect.x, rect.y, rect.w, rect.h}
            };
            vertices[index++] = {
                {0.0,  0.0},
                {fromColor.r, fromColor.g, fromColor.b, fromColor.a},
                {rect.x, rect.y, rect.w, rect.h}
            };
            vertices[index++] = {
                {1.0,  0.0},
                {toColor.r, toColor.g, toColor.b, toColor.a},
                {rect.x, rect.y, rect.w, rect.h}
            };
            vertices[index++] = {
                {1.0,  1.0},
                {toColor.r, toColor.g, toColor.b, toColor.a},
                {rect.x, rect.y, rect.w, rect.h}
            };
            break;
        }
    }

    count++;
}

void QuadRenderer::render() {
    shader.use();
    glBindVertexArray(VAO);
    glBindBuffer(GL_ARRAY_BUFFER, VBO);
    glBufferSubData(GL_ARRAY_BUFFER, 0, sizeof(Vertex) * index, vertices);
    glDrawElements(GL_TRIANGLES, 6 * count, GL_UNSIGNED_INT, 0);

    reset();
}