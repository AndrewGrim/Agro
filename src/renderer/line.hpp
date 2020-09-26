#ifndef LINE_HPP
    #define LINE_HPP

    #include "../util.hpp"
    #include "../common/color.hpp"
    #include "../common/rect.hpp"
    #include <glm/glm.hpp>
    #include<glad/glad.h>

    class Line {
        public:
            unsigned int VAO, VBO;
            float vertices[4] = { 
                -1.0f, 0.0f,
                1.0f, 0.0f,
            };

            Line() {
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

            ~Line() {
                glDeleteVertexArrays(1, &VAO);
                glDeleteBuffers(1, &VBO);
            }

            void draw() {
                glBindVertexArray(VAO);
                glDrawArrays(GL_LINES, 0, 2);
                glBindVertexArray(0);
            }
    };
#endif