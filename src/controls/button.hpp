#ifndef BUTTON_HPP
    #define BUTTON_HPP

    #include "../app.hpp"
    #include "widget.hpp"
    #include "../renderer/drawing_context.hpp"

    class Button : public Widget {
        public:
            Button(std::string text) {
                this->set_text(text);
            }
            
            ~Button() {}

            const char* name() {
                return this->m_name;
            }

            void draw(DrawingContext *dc, Rect<float> rect) {
                this->rect = rect;
                Color color = Color(0, 0, 0, 0); 
                if (this->is_pressed() && this->is_hovered()) {
                    color = this->pressed_background(); 
                } else if (this->is_hovered()) {
                    color = this->hover_background();
                } else {
                    color = this->background();
                }
                
                // draw border and shrink rectangle to prevent drawing over the border
                rect = dc->drawBorder(rect, this->m_border_width, color);
                dc->fillRect(rect, color);
                dc->drawTextAligned(this->m_text, this->m_text_align, rect, this->m_padding + this->m_border_width / 2);
            }

            Size<float> size_hint(DrawingContext *dc) {
                Size<float> size = dc->measureText(text());
                    size.w += this->m_padding * 2;
                    size.h += this->m_padding * 2;
                return size;
            }

            Color background() {
                return this->bg;
            }

            Button* set_background(Color background) {
                this->bg = background;
                this->update();

                return this;
            }

            std::string text() {
                return this->m_text;
            }

            Button* set_text(std::string text) {
                this->m_text = text;
                this->update();

                return this;
            }

            bool is_layout() {
                return false;
            }

        private:
            const char *m_name = "Button";
            std::string m_text;
            TextAlignment m_text_align = TextAlignment::Center;
            unsigned int m_padding = 10;
            unsigned int m_border_width = 4;
            Color fg = Color();
            Color bg = Color(0.9f, 0.9f, 0.9f);
    };
#endif
