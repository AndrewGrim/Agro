#ifndef BUTTON_HPP
    #define BUTTON_HPP

    #include "../app.hpp"
    #include "widget.hpp"
    #include "../renderer/drawing_context.hpp"

    class Button : public Widget {
        public:
            Application *m_app = nullptr;
            Button(Application *app, std::string text) {
                this->m_id = app->next_id();
                this->m_app = app;
                this->set_text(text);
            }
            
            ~Button() {}

            const char* name() {
                return this->m_name;
            }

            void draw(DrawingContext *dc, Rect<float> rect) {
                // TODO get rid of app, and init with dc, this could be done by messing with the widget in append.
                this->dc = dc;
                this->rect = rect;
                this->set_size(this->dc->measureText(text()));
                Color color = Color(0, 0, 0, 0); 
                if (this->is_pressed() && this->is_hovered()) {
                    color = this->pressed_background(); 
                } else if (this->is_hovered()) {
                    color = this->hover_background();
                } else {
                    color = this->background();
                }
                
                // light border
                {
                    // bottom layer of the top & left border : white, drawn first so that the top layer will paint over some extra pixels from here
                    dc->fillRect(
                        Rect<float>(rect.x,
                            rect.y + this->m_border_width / 2,
                            rect.w,
                            this->m_border_width / 2
                        ), Color(1.0f, 1.0f, 1.0f)
                    );
                    dc->fillRect(
                        Rect<float>(rect.x + this->m_border_width / 2,
                            rect.y,
                            this->m_border_width / 2,
                            rect.h
                        ), Color(1.0f, 1.0f, 1.0f)
                    );
                    // top layer of the top & left border : background
                    dc->fillRect(
                        Rect<float>(rect.x,
                            rect.y,
                            rect.w,
                            this->m_border_width / 2
                        ), this->background()
                    );
                    dc->fillRect(
                        Rect<float>(rect.x,
                            rect.y,
                            this->m_border_width / 2,
                            rect.h
                        ), this->background()
                    );
                }
                // dark border
                {
                    // top layer of the bottom & right border : dark grey, drawn first so that the bottom layer will paint over some extra pixels from here
                    dc->fillRect(
                        Rect<float>(rect.x + this->m_border_width / 2,
                            rect.y + rect.h - this->m_border_width,
                            rect.w - this->m_border_width / 2,
                            this->m_border_width / 2
                        ), Color(0.4f, 0.4f, 0.4f)
                    );
                    dc->fillRect(
                        Rect<float>(rect.x + rect.w - this->m_border_width,
                            rect.y + this->m_border_width / 2,
                            this->m_border_width / 2,
                            rect.h - this->m_border_width / 2
                        ), Color(0.4f, 0.4f, 0.4f)
                    );
                    // bottom layer of the bottom & right border : black
                    dc->fillRect(
                        Rect<float>(rect.x,
                            rect.y + rect.h - this->m_border_width / 2,
                            rect.w,
                            this->m_border_width / 2
                        ), Color(0.0f, 0.0f, 0.0f)
                    );
                    dc->fillRect(
                        Rect<float>(rect.x + rect.w - this->m_border_width / 2,
                            rect.y,
                            this->m_border_width / 2,
                            rect.h
                        ), Color(0.0f, 0.0f, 0.0f)
                    );
                }

                // resize rectangle to account for border
                this->rect = Rect<float>(
                    rect.x + this->m_border_width, 
                    rect.y + this->m_border_width, 
                    rect.w - this->m_border_width * 2, 
                    rect.h - this->m_border_width * 2
                );
                // actual rectangle
                dc->fillRect(this->rect, color);

                // draw text
                dc->drawTextAligned(this->m_text, this->m_text_align, rect, this->m_padding + this->m_border_width / 2);
            }

            Size<float> size_hint() {
                Size<float> size = this->m_size;
                    size.w += this->m_padding * 2;
                    size.h += this->m_padding * 2;
                return size;
            }

            Color background() {
                return this->bg;
            }

            Button* set_background(Color background) {
                this->bg = background;
                if (this->dc) this->update();

                return this;
            }

            std::string text() {
                return this->m_text;
            }

            Button* set_text(std::string text) {
                this->m_text = text;
                if (this->dc) this->update();

                return this;
            }

            void update() {
                this->m_app->m_needs_update = true;
            }

            bool is_layout() {
                return false;
            }

        private:
            const char *m_name = "Button";
            std::string m_text;
            Size<float> m_size;
            TextAlignment m_text_align = TextAlignment::Center;
            unsigned int m_padding = 10;
            unsigned int m_border_width = 4;
            Color fg = Color();
            Color bg = Color(0.90f, 0.90f, 0.90f);

            Button* set_size(Size<float> size) {
                this->m_size = size;

                return this;
            }
    };
#endif
