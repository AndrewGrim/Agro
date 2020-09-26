#ifndef QUAD_HPP
    #define QUAD_HPP

    #include "../util.hpp"
    #include "../common/color.hpp"
    #include "../common/rect.hpp"
    #include <glm/glm.hpp>
    #include<glad/glad.h>

    class Quad {
        public:
            unsigned int VAO, VBO;
            float vertices[12] = { 
                0.0f, 1.0f,
                1.0f, 0.0f,
                0.0f, 0.0f,

                0.0f, 1.0f,
                1.0f, 1.0f,
                1.0f, 0.0f,
            };

            Quad() {
                glGenVertexArrays(1, &VAO);
                glGenBuffers(1, &VBO);

                glBindBuffer(GL_ARRAY_BUFFER, VBO);
                glBufferData(GL_ARRAY_BUFFER, sizeof(vertices), vertices, GL_STATIC_DRAW);

                glBindVertexArray(VAO);
                glEnableVertexAttribArray(0);
                glVertexAttribPointer(0, 2, GL_FLOAT, GL_FALSE, 2 * sizeof(float), (void*)0);
                glBindBuffer(GL_ARRAY_BUFFER, 0);
                glBindVertexArray(0);
            }

            ~Quad() {
                glDeleteVertexArrays(1, &VAO);
                glDeleteBuffers(1, &VBO);
            }

            void draw() {
                glBindVertexArray(VAO);
                glDrawArrays(GL_TRIANGLES, 0, 6);
                glBindVertexArray(0);
            }
    };
#endif