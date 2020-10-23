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
        int atlasWidth = 0;
        int atlasHeight = 0;
        unsigned int atlasID;
        Shader shader = Shader("shaders/batch_text_quad.glsl.vert", "shaders/batch_text_quad.glsl.frag");
        unsigned int index = 0;
        unsigned int count = 0;
        Vertex vertices[MAX_BATCH_SIZE * QUAD_VERTEX_COUNT];
        unsigned int VAO, VBO, EBO;

        TextRenderer(unsigned int *indices);
        ~TextRenderer();
        void load(std::string font, unsigned int fontSize);
        void fillText(std::string text, float x, float y, Color color = {0, 0, 0, 1}, float scale = 1.0f);
        Size<float> measureText(std::string text, float scale = 1.0f);
        void reset();
        void check();
        void render();
    };
#endif
