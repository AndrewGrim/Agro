#ifndef DRAWING_CONTEXT_HPP
    #define DRAWING_CONTEXT_HPP

    #include <string>
    #include "../common/color.hpp"
    #include "../common/rect.hpp"
    #include "my_shader.hpp"
    #include "quad.hpp"
    #include "line.hpp"
    #include <glm/glm.hpp>
    #include <glm/gtc/matrix_transform.hpp>
    #include <glm/gtc/type_ptr.hpp>
    #include "../util.hpp"
    #include "text_renderer.h"

    class DrawingContext {
        public:
            my_shader::Shader geometry_shader = my_shader::Shader("shaders/vshader.glsl", "shaders/fshader.glsl");
            glm::mat4 projection = glm::mat4(1.0f);
            TextRenderer text_renderer = TextRenderer();
            
            DrawingContext() {
                glEnable(GL_BLEND);
                glBlendFunc(GL_SRC_ALPHA, GL_ONE_MINUS_SRC_ALPHA);
            }

            ~DrawingContext() {

            }

            void fillRect(Rect<float> rect, Color color, float rotation = 0.0f) {
                geometry_shader.use();
                Quad quad = Quad();

                glm::mat4 model = glm::mat4(1.0f);

                model = glm::translate(model, glm::vec3(rect.x, rect.y, 0.0f));
                model = glm::translate(model, glm::vec3(0.5f * rect.w, 0.5f * rect.h, 0.0f)); 
                model = glm::rotate(model, glm::radians(rotation), glm::vec3(0.0f, 0.0f, 1.0f));
                model = glm::translate(model, glm::vec3(-0.5f * rect.w, -0.5f * rect.h, 0.0f));
                model = glm::scale(model, glm::vec3(rect.w, rect.h, 1.0f));

                this->geometry_shader.setMatrix4("model", model, true);
                this->geometry_shader.setVector3f("spriteColor", glm::vec3(color.red, color.green, color.blue), true);

                quad.draw();
            }

            // TODO this is not nice to use
            void drawLine(Rect<float> rect, Color color, float line_width = 1.0f, float rotation = 0.0f) {
                geometry_shader.use();
                glLineWidth(line_width);
                Line line = Line();

                glm::mat4 model = glm::mat4(1.0f);

                model = glm::translate(model, glm::vec3(rect.x, rect.y, 0.0f));
                model = glm::rotate(model, glm::radians(rotation), glm::vec3(0.0f, 0.0f, 1.0f));
                model = glm::scale(model, glm::vec3(rect.w, rect.h, 1.0f));

                this->geometry_shader.setMatrix4("model", model, true);
                this->geometry_shader.setVector3f("spriteColor", glm::vec3(color.red, color.green, color.blue), true);

                line.draw();
            }

            void loadFont(std::string path, unsigned int font_size) {
                this->text_renderer.Load(path, font_size);
            }

            Size<float> measureText(std::string text, float scale = 1.0f) {
                return this->text_renderer.MeasureText(text, scale);
            }

            void drawText(std::string text, float x, float y, Color color = Color(0.0f, 0.0f, 0.0f, 1.0f), float scale = 1.0f) {
                this->text_renderer.RenderText(text, x, y, scale, glm::vec3(color.red, color.green, color.blue));
            }

            void drawTextAligned(std::string text, TextAlignment alignment, Rect<float> rect, int padding) {
                switch (alignment) {
                    case TextAlignment::Center:
                        this->drawText(
                            text,
                            rect.x + (rect.w / 2) - (this->measureText(text).w / 2),
                            rect.y + (rect.h / 2) - (this->measureText(text).h / 2)
                        );
                        break;
                    case TextAlignment::Right:
                        this->drawText(
                            text, 
                            (rect.x + rect.w) - (this->measureText(text).w + padding), 
                            rect.y + (rect.h / 2) - (this->measureText(text).h / 2)
                        );
                        break;
                    default:
                        this->drawText(
                            text, 
                            rect.x + padding, 
                            rect.y + (rect.h / 2) - (this->measureText(text).h / 2)
                        );
                }
            }

            Rect<float> drawBorder(Rect<float> rect, int border_width, Color rect_color) {
                // light border
                {
                    // bottom layer of the top & left border : white, drawn first so that the top layer will paint over some extra pixels from here
                    this->fillRect(
                        Rect<float>(rect.x,
                            rect.y + border_width / 2,
                            rect.w,
                            border_width / 2
                        ), Color(1.0f, 1.0f, 1.0f)
                    );
                    this->fillRect(
                        Rect<float>(rect.x + border_width / 2,
                            rect.y,
                            border_width / 2,
                            rect.h
                        ), Color(1.0f, 1.0f, 1.0f)
                    );
                    // top layer of the top & left border : background
                    this->fillRect(
                        Rect<float>(rect.x,
                            rect.y,
                            rect.w,
                            border_width / 2
                        ), rect_color
                    );
                    this->fillRect(
                        Rect<float>(rect.x,
                            rect.y,
                            border_width / 2,
                            rect.h
                        ), rect_color
                    );
                }
                // dark border
                {
                    // top layer of the bottom & right border : dark grey, drawn first so that the bottom layer will paint over some extra pixels from here
                    this->fillRect(
                        Rect<float>(rect.x + border_width / 2,
                            rect.y + rect.h - border_width,
                            rect.w - border_width / 2,
                            border_width / 2
                        ), Color(0.4f, 0.4f, 0.4f)
                    );
                    this->fillRect(
                        Rect<float>(rect.x + rect.w - border_width,
                            rect.y + border_width / 2,
                            border_width / 2,
                            rect.h - border_width / 2
                        ), Color(0.4f, 0.4f, 0.4f)
                    );
                    // bottom layer of the bottom & right border : black
                    this->fillRect(
                        Rect<float>(rect.x,
                            rect.y + rect.h - border_width / 2,
                            rect.w,
                            border_width / 2
                        ), Color(0.0f, 0.0f, 0.0f)
                    );
                    this->fillRect(
                        Rect<float>(rect.x + rect.w - border_width / 2,
                            rect.y,
                            border_width / 2,
                            rect.h
                        ), Color(0.0f, 0.0f, 0.0f)
                    );
                }

                // resize rectangle to account for border
                rect = Rect<float>(
                    rect.x + border_width, 
                    rect.y + border_width, 
                    rect.w - border_width * 2, 
                    rect.h - border_width * 2
                );

                return rect;
            }

            void clear() {
                glClearColor(0, 0, 0, 1);
                glClear(GL_COLOR_BUFFER_BIT);
            }

            void set_projection(glm::mat4 projection) {
                this->projection = projection;
                this->geometry_shader.setMatrix4("projection", projection, true);
                this->text_renderer.set_projection(projection);
            }

            void swap_buffer(SDL_Window *win) {
                SDL_GL_SwapWindow(win);
            }
    };
#endif