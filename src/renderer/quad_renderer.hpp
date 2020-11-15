#ifndef QUAD_RENDERER_HPP
    #define QUAD_RENDERER_HPP

    #include <glad/glad.h>
    #include <SDL2/SDL.h>

    #include "../common/color.hpp"
    #include "../common/rect.hpp"
    #include "../common/size.hpp"

    #include "shader.hpp"
    #include "batch.hpp"

    struct QuadRenderer {
        enum class Gradient {
            TopToBottom,
            LeftToRight,
        };

        struct Vertex {
            float position[2];
            float color[4];
            float rect[4];
        };

        unsigned int index = 0;
        unsigned int count = 0;
        Vertex vertices[MAX_BATCH_SIZE * QUAD_VERTEX_COUNT];
        unsigned int VAO, VBO, EBO;
        Shader shader = Shader("shaders/batch_colored_quad.glsl.vert", "shaders/batch_colored_quad.glsl.frag");

        QuadRenderer(unsigned int *indices);
        ~QuadRenderer();
        void reset();
        void check();
        void fillRect(Rect<float> rect, Color color);
        void fillGradientRect(Rect<float> rect, Color fromColor, Color toColor, Gradient orientation);
        void render();
    };
#endif