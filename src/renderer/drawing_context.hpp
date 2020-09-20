#ifndef DRAWING_CONTEXT_HPP
    #define DRAWING_CONTEXT_HPP

    #include "../common/color.hpp"
    #include "../common/rect.hpp"
    #include "my_shader.hpp"
    #include "quad.hpp"
    #include <glm/glm.hpp>
    #include <glm/gtc/matrix_transform.hpp>
    #include <glm/gtc/type_ptr.hpp>
    #include "../util.hpp"

    class DrawingContext {
        public:
            my_shader::Shader geometry_shader = my_shader::Shader("shaders/vshader.glsl", "shaders/fshader.glsl");
            glm::mat4 projection = glm::mat4(1.0f);
            
            DrawingContext() {
                glEnable(GL_BLEND);
                glBlendFunc(GL_SRC_ALPHA, GL_ONE_MINUS_SRC_ALPHA);
            }

            ~DrawingContext() {

            }

            void fillRect(Rect rect, Color color, float rotation = 0.0f) {
                geometry_shader.use();
                Quad quad = Quad();

                glm::mat4 model = glm::mat4(1.0f);
                model = glm::translate(model, glm::vec3(rect.x, rect.y, 0.0f));

                model = glm::translate(model, glm::vec3(0.5f * rect.w, 0.5f * rect.h, 0.0f)); 
                model = glm::rotate(model, glm::radians(rotation), glm::vec3(0.0f, 0.0f, 1.0f));
                model = glm::translate(model, glm::vec3(-0.5f * rect.w, -0.5f * rect.h, 0.0f));

                model = glm::scale(model, glm::vec3(rect.w, rect.h, 1.0f));

                this->geometry_shader.setMatrix4("model", model, true);

                // TODO this should probably be a vec4 so that we support transparency
                this->geometry_shader.setVector3f("spriteColor", glm::vec3(color.red, color.green, color.blue), true);

                quad.draw();
            }

            void clear(GLFWwindow *win) {
                glClearColor(0, 0, 0, 1);
                glClear(GL_COLOR_BUFFER_BIT);
            }

            void set_projection(glm::mat4 projection) {
                this->projection = projection;
                this->geometry_shader.setMatrix4("projection", projection, true);
            }

            void swap_buffer(GLFWwindow *win) {
                glfwSwapBuffers(win);
            }
    };
#endif