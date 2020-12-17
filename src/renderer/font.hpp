#ifndef FONT_HPP
    #define FONT_HPP

    #include <map>
    #include <string>

    // TODO get rid of glm, we can just use size here and perhaps point for bearing? or another size
    #include <glm/glm.hpp>

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
#endif