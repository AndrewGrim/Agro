#ifndef LINEEDIT_HPP
    #define LINEEDIT_HPP

    #include "../app.hpp"
    #include "widget.hpp"
    #include "box.hpp"
    #include "button.hpp"
    #include "../renderer/drawing_context.hpp"

    void _onLineEditClicked(Widget *lineedit, MouseEvent event);

    class LineEdit : public Button {
        public:
            LineEdit(std::string text = "") : Button(text) {
                this->mouse_click_callback = [&](Widget *lineedit, MouseEvent event) {
                    if (!this->text().length()) {
                        this->set_cursor_position(this->padding() + this->border_width() / 2);
                    } else {
                        this->m_last_mouse_x = event.x;
                        this->m_process_mouse_event = true;
                    }
                };
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

                if (this->m_process_mouse_event) {
                    float x = this->padding() + this->border_width() / 2;
                    unsigned int index = 0;
                    for (char c : this->text()) {
                        float w = dc->measureText(c).w;
                        if (x + w > this->m_last_mouse_x) {
                            break;
                        }
                        x += w;
                        index++;
                    }
                    this->set_cursor_position(x);
                    this->m_process_mouse_event = false;
                    this->m_cursor_index = index;
                    // TODO draw the cursor (this will happen within the draw method, based on whether the widget is focused or not and where)
                }
                float text_height = this->m_size.h - this->m_padding;
                dc->fillRect(
                    Rect<float>(
                        rect.x + this->m_cursor_position, 
                        rect.y + (rect.h / 2) - (text_height / 2), 
                        1, 
                        text_height
                    ), 
                    Color(1, 0, 1)
                );
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

            TextAlignment text_alignment() {
                return this->m_text_align;
            }

            unsigned int border_width() {
                return this->m_border_width;
            }

            unsigned int padding() {
                return this->m_padding;
            }

            LineEdit* set_cursor_position(float x) {
                this->m_cursor_position = x;
                return this;
            }

        protected:
            const char *m_name = "LineEdit";
            TextAlignment m_text_align = TextAlignment::Left;
            unsigned int m_border_width = 2;
            float m_cursor_position = this->padding() + this->border_width() / 2;
            Color bg = Color(1.0f, 1.0f, 1.0f);
            unsigned int m_last_mouse_x;
            unsigned int m_cursor_index;
            bool m_process_mouse_event = false;
    };
#endif