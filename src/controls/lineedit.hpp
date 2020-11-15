#ifndef LINEEDIT_HPP
    #define LINEEDIT_HPP

    #include "../app.hpp"
    #include "widget.hpp"
    #include "box.hpp"
    #include "button.hpp"
    #include "../renderer/drawing_context.hpp"

    class LineEdit : public Button {
        public:

            LineEdit(std::string text = "") : Button(text) {
            }
            
            ~LineEdit() {}

            const char* name() {
                return this->m_name;
            }

            virtual void draw(DrawingContext *dc, Rect<float> rect) {
                this->rect = rect;

                dc->fillRect(
                    rect, 
                    Color()
                );

                // resize rectangle to account for border
                rect = Rect<float>(
                    rect.x + this->m_border_width, 
                    rect.y + this->m_border_width, 
                    rect.w - this->m_border_width * 2, 
                    rect.h - this->m_border_width * 2
                );

                dc->fillRect(rect, this->background());
                dc->fillTextAligned(this->text(), this->m_text_align, rect, this->m_padding);
            }

            LineEdit* set_foreground(Color foreground) {
                this->bg = foreground;
                this->update();

                return this;
            }

            LineEdit* set_background(Color background) {
                this->bg = background;
                this->update();

                return this;
            }

            Color background() override {
                return this->bg;
            }

            LineEdit* set_text(std::string text) {
                this->m_text = text;
                this->size_changed = true;
                this->update();

                return this;
            }

        protected:
            const char *m_name = "LineEdit";
            TextAlignment m_text_align = TextAlignment::Left;
            unsigned int m_border_width = 2;
            Color bg = Color(1.0f, 1.0f, 1.0f);
    };
#endif