#ifndef FONT_HPP
    #define FONT_HPP

    #include <map>
    #include <string>

    // TODO get rid of glm, we can just use size here and perhaps point for bearing? or another size
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
        uint max_height = 0;
        uint pixel_size = 0;
        uint atlas_width = 0;
        uint atlas_height = 0;
        uint tab_width = 4;
        Type type;
        std::string file_path;
        uint atlas_ID;

        Font(std::string file_path, uint pixel_size, Type type);
        ~Font();

        private:
            void load();
    };
#endif