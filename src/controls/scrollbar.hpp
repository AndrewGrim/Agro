#ifndef SCROLLBAR_HPP
    #define SCROLLBAR_HPP

    #include "../app.hpp"
    #include "widget.hpp"
    #include "box.hpp"
    #include "button.hpp"
    #include "slider.hpp"
    #include "../renderer/drawing_context.hpp"

    void _onBeginClicked(Widget *scrollbar_begin_button, MouseEvent event);
    void _onEndClicked(Widget *scrollbar_begin_button, MouseEvent event);

    class ScrollBar : public Widget {
        public:
            Button *m_begin_button = nullptr;
            Slider *m_slider = nullptr;
            Button *m_end_button = nullptr;
            Align m_align_policy;
            std::string m_text;

            ScrollBar(Align alignment, std::string text = "") {
                this->m_align_policy = alignment;
                this->m_text = text;

                m_begin_button = new Button("<");
                m_begin_button->m_parent = this;
                m_begin_button->mouse_click_callback = _onBeginClicked;
                this->append(m_begin_button, Fill::None);

                m_slider = new Slider(alignment, text);
                this->append(m_slider, Fill::Vertical);

                m_end_button = new Button(">");
                m_end_button->m_parent = this;
                m_end_button->mouse_click_callback = _onEndClicked;
                this->append(m_end_button, Fill::None);
            }
            
            ~ScrollBar() {}

            const char* name() {
                return this->m_name;
            }

            void draw(DrawingContext *dc, Rect<float> rect) {
                this->rect = rect;
                Size<float> button_size = this->m_begin_button->size_hint(dc);
                this->m_begin_button->draw(dc, Rect<float>(rect.x, rect.y, button_size.w, button_size.h));
                rect.y += button_size.h;
                rect.h -= button_size.h * 2;
                this->m_slider->draw(dc, rect);
                this->m_end_button->draw(dc, Rect<float>(rect.x, rect.y + rect.h, button_size.w, button_size.h));
            }

            Size<float> size_hint(DrawingContext *dc) {
                Size<float> size = this->m_slider->m_slider_button->size_hint(dc);
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

            bool is_layout() {
                return true;
            }

        protected:
            const char *m_name = "ScrollBar";
    };  
#endif