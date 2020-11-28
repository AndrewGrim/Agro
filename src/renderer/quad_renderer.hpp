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
        Shader shader = Shader(
            "#version 330 core\n"
            "layout (location = 0) in vec2 aPos;\n"
            "layout (location = 1) in vec4 aColor;\n"
            "layout (location = 2) in vec4 aRect;\n"
            "\n"
            "out vec4 vColor;\n"
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
                "gl_Position = projection * model * vec4(aPos, 1.0, 1.0);\n"
                "vColor = aColor;\n"
            "}",
            "#version 330 core\n"
            "in vec4 vColor;\n"
            "\n"
            "out vec4 FragColor;\n"
            "\n"
            "void main()\n"
            "{\n"
                "FragColor = vColor;\n"
            "}"
        );

        QuadRenderer(unsigned int *indices);
        ~QuadRenderer();
        void reset();
        void check();
        void fillRect(Rect rect, Color color);
        void fillGradientRect(Rect rect, Color fromColor, Color toColor, Gradient orientation);
        void render();
    };
#endif