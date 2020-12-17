#ifndef TEXTURE_HPP
    #define TEXTURE_HPP

    #include <string>

    #include <glad/glad.h>
    #include "stb_image.h"

    #include "../util.hpp"

    struct Texture {
        std::string file_path;
        int width;
        int height;
        int nr_channels;
        unsigned char *data;
        unsigned int ID;

        Texture(std::string file_path) {
            this->file_path = file_path;
            stbi_set_flip_vertically_on_load(true); // tell stb_image.h to flip loaded texture's on the y-axis.
            this->data = stbi_load(
                file_path.c_str(), 
                &this->width, 
                &this->height, 
                &this->nr_channels, 
                0
            );
            if (this->data) {
                // TODO this is likely part of the problem with images
                // i assume that this overrides the currently assigned font texture
                // what im thinking is:
                // reserve the first 2 texture slots
                // the first one for a white texture - this will be used to draw "normal" quads once we join the buffers into one
                // the second one could be used for loading any random textures
                // then instead of reseting the texture count to 0 just reset to 2 instead!

                // TODO also we need to draw the image at the specified position
                // and optionally at a different size
                // and i notice that only quad renderer has a rect parameter
                glActiveTexture(GL_TEXTURE0);
                glGenTextures(1, &ID);
                glBindTexture(GL_TEXTURE_2D, ID);

                glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_S, GL_REPEAT);
                glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_T, GL_REPEAT);

                glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_LINEAR);
                glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_LINEAR);
                // println("nr_channels: " + std::to_string(nr_channels)); // this tells us if we need rgb or rgba for example
                // TODO we might want to do GL_RGB for jpgs but GL_RGBA for pngs
                glTexImage2D(GL_TEXTURE_2D, 0, GL_RGBA, width, height, 0, GL_RGBA, GL_UNSIGNED_BYTE, this->data);
                glGenerateMipmap(GL_TEXTURE_2D);
            } else {
                println("Error: Failed to load texture! '" + file_path + "'");
            }
        }

        ~Texture() {
            free(this->data);
            glDeleteTextures(1, &this->ID);
        }
    };
#endif