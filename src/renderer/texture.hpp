#ifndef TEXTURE_HPP
    #define TEXTURE_HPP

    #include <cassert>

#ifdef __EMSCRIPTEN__
    #include <GLES3/gl3.h>
#else
    #include "glad.h"
#endif

    #include "stb_image.h"
    #include "../util.hpp"
    #include "../common/point.hpp"
    #include "../core/string.hpp"

    struct TextureCoordinates {
        // Default
        Point top_left = Point(0.0, 1.0);
        Point bottom_left = Point(0.0, 0.0);
        Point bottom_right = Point(1.0, 0.0);
        Point top_right = Point(1.0, 1.0);

        TextureCoordinates() {}

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
        i32 width = -1;
        i32 height = -1;
        i32 nr_channels = -1;
        u32 ID;

        Texture(String file_path) {
            u8 *data = stbi_load(
                file_path.data(),
                &width,
                &height,
                &nr_channels,
                0
            );
            makeGLTexture(data, width, height, nr_channels, file_path);
        }

        Texture(const u8 *image_data, i32 length) {
            assert(image_data && "Null image data!");
            u8 *data = stbi_load_from_memory(
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

        void makeGLTexture(u8 *data, i32 width, i32 height, i32 nr_channels, String file_path) {
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
                fail("FAILED_TO_LOAD_TEXTURE", file_path.data());
            }
        }
    };
#endif
