#ifndef TEXT_RENDERER_HPP
    #define TEXT_RENDERER_HPP

    #include <map>
    #include <string>
    #include <vector>

    #include <glad/glad.h>
    #include <glm/glm.hpp>
    #include <ft2build.h>
    #include FT_FREETYPE_H

    #include "../common/color.hpp"
    #include "../common/rect.hpp"
    #include "../common/size.hpp"

    #include "shader.hpp"
    #include "batch.hpp"
    #include "texture.hpp"

    struct Font {
        enum class Type {
            Sans,
            Serif,
            Mono,
        };

        struct Character {
            glm::ivec2 size;
            glm::ivec2 bearing;
            long int advance;
            float textureX;
        };

        std::map<char, Font::Character> characters;
        unsigned int max_height = 0;
        unsigned int pixel_size = 0;
        unsigned int atlas_width = 0;
        unsigned int atlas_height = 0;
        unsigned int tab_width = 4;
        Type type;
        std::string file_path;
        unsigned int atlas_ID;

        Font(std::string file_path, unsigned int pixel_size, Type type);
        ~Font();

        private:
            void load();
    };

    struct TextRenderer {
        struct Vertex {
            float position[2];
            float textureUV[2];
            float color[4];
            float texture_index;
        };
        
        std::vector<Font> fonts;
        void *m_app = nullptr;
        int max_texture_slots;
        int current_texture_slot = 2;
        unsigned int gl_texture_begin = GL_TEXTURE0;
        Shader shader;
        unsigned int index = 0;
        unsigned int count = 0;
        Vertex *vertices = new Vertex[MAX_BATCH_SIZE * QUAD_VERTEX_COUNT];
        unsigned int VAO, VBO, EBO;

        TextRenderer(unsigned int *indices, void *app);
        ~TextRenderer();
        void fillText(Font *font, std::string text, float x, float y, Color color = {0, 0, 0, 1}, float scale = 1.0f);
        Size measureText(Font *font, std::string text, float scale = 1.0f);
        Size measureText(Font *font, char c, float scale = 1.0f);
        void drawImage(Texture *texture, Color color = Color());
        void check();
        void render();
        
        private:
            void reset();
    };
#endif
