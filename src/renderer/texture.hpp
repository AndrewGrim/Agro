#ifndef TEXTURE_HPP
    #define TEXTURE_HPP

    #include <string>
    #include <cassert>

    #include <glad/glad.h>
    #include "stb_image.h"

    #include "../util.hpp"
    #include "../common/point.hpp"

    struct Texture {
        int width = -1;
        int height = -1;
        int nr_channels = -1;
        unsigned int ID;
        
        Texture(std::string file_path) {
            unsigned char *data = stbi_load(
                file_path.c_str(), 
                &width, 
                &height, 
                &nr_channels, 
                0
            );
            if (data) {
                glActiveTexture(GL_TEXTURE0);
                glGenTextures(1, &ID);
                glBindTexture(GL_TEXTURE_2D, ID);

                glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_S, GL_REPEAT);
                glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_T, GL_REPEAT);

                glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_LINEAR);
                glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_LINEAR);
                // TODO get some grayscale, and grayscale + alpha images to test 1 and 2 channel textures
                assert((nr_channels == 3 || nr_channels == 4) && "Unsupported number of channels!");
                if (nr_channels == 4) {
                    glTexImage2D(GL_TEXTURE_2D, 0, GL_RGBA, width, height, 0, GL_RGBA, GL_UNSIGNED_BYTE, data);
                } else {
                    glTexImage2D(GL_TEXTURE_2D, 0, GL_RGB, width, height, 0, GL_RGB, GL_UNSIGNED_BYTE, data);
                }
                glGenerateMipmap(GL_TEXTURE_2D);
                stbi_image_free(data);
            } else {
                println("Error: Failed to load texture! '" + file_path + "'");
            }
        }

        Texture(bool from_memory, const unsigned char *image_data, int length) {
            assert(image_data && "Null image data!");
            unsigned char *data = stbi_load_from_memory(
                image_data, 
                length, 
                &width, 
                &height, 
                &nr_channels, 
                0
            );
            if (data) {
                glActiveTexture(GL_TEXTURE0);
                glGenTextures(1, &ID);
                glBindTexture(GL_TEXTURE_2D, ID);

                glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_S, GL_REPEAT);
                glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_T, GL_REPEAT);

                glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_LINEAR);
                glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_LINEAR);
                // TODO get some grayscale, and grayscale + alpha images to test 1 and 2 channel textures
                assert((nr_channels == 3 || nr_channels == 4) && "Unsupported number of channels!");
                if (nr_channels == 4) {
                    glTexImage2D(GL_TEXTURE_2D, 0, GL_RGBA, width, height, 0, GL_RGBA, GL_UNSIGNED_BYTE, data);
                } else {
                    glTexImage2D(GL_TEXTURE_2D, 0, GL_RGB, width, height, 0, GL_RGB, GL_UNSIGNED_BYTE, data);
                }
                glGenerateMipmap(GL_TEXTURE_2D);
                stbi_image_free(data);
            } else {
                println("Error: Failed to load texture from memory!");
            }
        }

        ~Texture() {
            glDeleteTextures(1, &this->ID);
        }
    };
#endif