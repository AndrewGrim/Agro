#ifndef TEXT_RENDERER_HPP
    #define TEXT_RENDERER_HPP

    #include <map>

    #include <glad/glad.h>
    #include <glm/glm.hpp>
    #include <ft2build.h>
    #include FT_FREETYPE_H

    #include "../common/color.hpp"
    #include "../common/rect.hpp"
    #include "../common/size.hpp"

    #include "shader.hpp"
    #include "batch.hpp"

    struct TextRenderer {
        struct TextCharacter {
            glm::ivec2 size;
            glm::ivec2 bearing;
            long int advance;
            float textureX;
        };

        struct Vertex {
            float position[2];
            float textureUV[2];
            float color[4];
        };
        
        std::map<char, TextCharacter> characters;
        void *m_app = nullptr;
        int atlasWidth = 0;
        int atlasHeight = 0;
        unsigned int atlasID;
        Shader shader = Shader(
            "#version 330 core\n"
            "layout (location = 0) in vec2 position;\n"
            "layout (location = 1) in vec2 textureUV;\n"
            "layout (location = 2) in vec4 aColor;\n"
            "\n"
            "out vec2 TexCoords;\n"
            "out vec4 color;\n"
            "\n"
            "uniform mat4 projection;\n"
            "\n"
            "void main()\n"
            "{\n"
                "gl_Position = projection * vec4(position, 0.0, 1.0);\n"
                "TexCoords = textureUV;\n"
                "color = aColor;\n"
            "}", 
            "#version 330 core\n"
            "in vec2 TexCoords;\n"
            "in vec4 color;\n"
            "\n"
            "out vec4 fColor;\n"
            "\n"
            "uniform sampler2D text;\n"
            "\n"
            "void main()\n"
            "{    \n"
                "vec4 sampled = vec4(1.0, 1.0, 1.0, texture(text, TexCoords).r);\n"
                "fColor = color * sampled;\n"
            "}"  
        );
        unsigned int index = 0;
        unsigned int count = 0;
        Vertex vertices[MAX_BATCH_SIZE * QUAD_VERTEX_COUNT];
        unsigned int VAO, VBO, EBO;

        TextRenderer(unsigned int *indices, void *app);
        ~TextRenderer();
        void load(std::string font, unsigned int fontSize);
        void fillText(std::string text, float x, float y, Color color = {0, 0, 0, 1}, float scale = 1.0f);
        Size measureText(std::string text, float scale = 1.0f);
        Size measureText(char c, float scale = 1.0f);
        void reset();
        void check();
        void render();
    };
#endif
