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
            my_shader::Shader shader = my_shader::Shader("shaders/vshader.glsl", "shaders/fshader.glsl");
            
            DrawingContext() {
                
            }

            ~DrawingContext() {

            }

            void fillRect(Rect rect, Color color, float rotation = 0.0f) {
                shader.use();
                Quad quad = Quad();

                glm::mat4 model = glm::mat4(1.0f);
                model = glm::translate(model, glm::vec3(rect.x, rect.y, 0.0f));

                model = glm::translate(model, glm::vec3(0.5f * rect.w, 0.5f * rect.h, 0.0f)); // move origin of rotation to center of quad
                model = glm::rotate(model, glm::radians(rotation), glm::vec3(0.0f, 0.0f, 1.0f)); // then rotate
                model = glm::translate(model, glm::vec3(-0.5f * rect.w, -0.5f * rect.h, 0.0f)); // move origin back

                model = glm::scale(model, glm::vec3(rect.w, rect.h, 1.0f)); // last scale

                this->shader.setMatrix4("model", model, true);

                // render textured quad
                this->shader.setVector3f("spriteColor", glm::vec3(color.red, color.green, color.blue), true);

                quad.draw();
            }
    };
#endif