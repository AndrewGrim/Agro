#ifndef SCROLLBAR_HPP
    #define SCROLLBAR_HPP

    #include "../app.hpp"
    #include "widget.hpp"
    #include "box.hpp"
    #include "button.hpp"
    #include "slider.hpp"
    #include "../renderer/drawing_context.hpp"

    class ScrollBar : public Slider {
        public:
            ScrollBar(Align alignment, std::string text = "") : Slider(alignment, text) {
                m_begin_button = new Button("<");
                this->children.insert(this->children.begin(), m_begin_button);
                if (this->m_app) m_begin_button->m_app = this->m_app;

                m_end_button = new Button(">");
                this->children.push_back(m_end_button);
                if (this->m_app) m_end_button->m_app = this->m_app;
            }
            
            ~ScrollBar() {}

            const char* name() {
                return this->m_name;
            }

            void draw(DrawingContext *dc, Rect<float> rect) {
                this->rect = rect;
                Size<float> button_size = this->m_begin_button->size_hint(dc);
                this->children[0]->draw(dc, Rect<float>(rect.x, rect.y, button_size.w, button_size.h));
                rect.y += button_size.h;
                rect.h -= button_size.h * 2;
                Slider::draw(dc, rect);
                this->children[2]->draw(dc, Rect<float>(rect.x, rect.y + rect.h, button_size.w, button_size.h));
            }

            Size<float> size_hint(DrawingContext *dc) {
                Size<float> size = this->m_slider_button->size_hint(dc);
                Size<float> button_size = this->m_begin_button->size_hint(dc);
                    if (this->m_align_policy == Align::Horizontal) {
                        size.w *= 2 * 2;
                        size.w += button_size.w;
                        if (button_size.h > size.h) size.h = button_size.h;
                    }
                    else {
                        size.h *= 2;
                        size.h += button_size.h * 2;
                        if (button_size.w > size.w) size.w = button_size.w;
                    }
                return size;
            }

        protected:
            const char *m_name = "ScrollBar";
            Button *m_begin_button = nullptr;
            Button *m_end_button = nullptr;
    };
#endif