#ifndef TEXTURE_HPP
    #define TEXTURE_HPP

    #include <string>

    #include <glad/glad.h>
    #include "stb_image.h"

    #include "../util.hpp"
    #include "../common/point.hpp"

    struct Texture {
        std::string file_path = "";
        int width = 0;
        int height = 0;
        int nr_channels = 0;
        unsigned char *data = nullptr;
        uint ID;
        Point top_left = Point(0.0, 1.0);
        Point bottom_left = Point(0.0, 0.0);
        Point bottom_right = Point(1.0, 0.0);
        Point top_right = Point(1.0, 1.0);

        Texture(std::string file_path) {
            this->file_path = file_path;
            this->data = stbi_load(
                file_path.c_str(), 
                &this->width, 
                &this->height, 
                &this->nr_channels, 
                0
            );
            if (this->data) {
                glActiveTexture(GL_TEXTURE0);
                glGenTextures(1, &this->ID);
                glBindTexture(GL_TEXTURE_2D, this->ID);

                glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_S, GL_REPEAT);
                glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_T, GL_REPEAT);

                glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_LINEAR);
                glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_LINEAR);
                // println("nr_channels: " + std::to_string(nr_channels)); // this tells us if we need rgb or rgba for example
                // TODO we might want to do GL_RGB for jpgs but GL_RGBA for pngs
                glTexImage2D(GL_TEXTURE_2D, 0, GL_RGBA, this->width, this->height, 0, GL_RGBA, GL_UNSIGNED_BYTE, this->data);
                glGenerateMipmap(GL_TEXTURE_2D);
            } else {
                println("Error: Failed to load texture! '" + file_path + "'");
            }
        }

        ~Texture() {
            free(this->data);
            glDeleteTextures(1, &this->ID);
        }

        Texture* flipHorizontally() {
            // if flipped reset to default
            if (top_left.x) {
                top_left.x = 0.0;
                bottom_left.x = 0.0;
                bottom_right.x = 1.0;
                top_right.x = 1.0;
            // flip x to the opposite side
            } else {
                top_left.x = 1.0;
                bottom_left.x = 1.0;
                bottom_right.x = 0.0;
                top_right.x = 0.0;
            }

            return this;
        }

        Texture* flipVertically() {
            // if flipped reset to default
            if (!top_left.y) {
                top_left.y = 1.0;
                bottom_left.y = 0.0;
                bottom_right.y = 0.0;
                top_right.y = 1.0;
            // flip y to the opposite side
            } else {
                top_left.y = 0.0;
                bottom_left.y = 1.0;
                bottom_right.y = 1.0;
                top_right.y = 0.0;
            }

            return this;
        }

        Texture* flipBoth() {
            this->flipHorizontally();
            this->flipVertically();

            return this;
        }
    };
#endif