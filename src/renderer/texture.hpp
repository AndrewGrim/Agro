#ifndef TEXTURE_HPP
    #define TEXTURE_HPP

    #include <string>
    #include <cassert>

    #include "glad.h"
    #include "stb_image.h"
    #include "../util.hpp"
    #include "../common/point.hpp"

    struct TextureCoordinates {
        // Default
        Point top_left = Point(0.0, 1.0);
        Point bottom_left = Point(0.0, 0.0);
        Point bottom_right = Point(1.0, 0.0);
        Point top_right = Point(1.0, 1.0);

        TextureCoordinates() {

        }

        void flipHorizontally() {
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
        }

        void flipVertically() {
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
        }

        void flipBoth() {
            this->flipHorizontally();
            this->flipVertically();
        }

        void clockwise90() {
            top_left = Point(1.0, 1.0);
            bottom_left = Point(0.0, 1.0);
            bottom_right = Point(0.0, 0.0);
            top_right = Point(1.0, 0.0);
        }

        void counterClockwise90() {
            top_left = Point(0.0, 0.0);
            bottom_left = Point(1.0, 0.0);
            bottom_right = Point(1.0, 1.0);
            top_right = Point(0.0, 1.0);
        }
    };

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
            makeGLTexture(data, width, height, nr_channels, file_path);
        }

        Texture(const unsigned char *image_data, int length) {
            assert(image_data && "Null image data!");
            unsigned char *data = stbi_load_from_memory(
                image_data,
                length,
                &width,
                &height,
                &nr_channels,
                0
            );
            makeGLTexture(data, width, height, nr_channels, ":memory:");
        }

        ~Texture() {
            glDeleteTextures(1, &this->ID);
        }

        void makeGLTexture(unsigned char *data, int width, int height, int nr_channels, std::string file_path) {
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
                fail("FAILED_TO_LOAD_TEXTURE", file_path);
            }
        }
    };
#endif
