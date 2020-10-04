#ifndef SLIDER_HPP
    #define SLIDER_HPP

    #include "../app.hpp"
    #include "widget.hpp"
    #include "box.hpp"
    #include "button.hpp"
    #include "../renderer/drawing_context.hpp"

    void onMouseMove(Widget *button, MouseEvent event);

    class SliderButton : public Button {
        public:
            SliderButton(std::string text) : Button(text) {
                this->set_background(Color(0.7f, 0.7f, 0.7f));
            }

            virtual const char* name() {
                return this->m_name;
            }

            void draw(DrawingContext *dc, Rect<float> rect) {
                this->rect = rect;
                Color color = Color(0, 0, 0, 0); 
                if (this->is_pressed()) {
                    color = this->pressed_background(); 
                } else if (this->is_hovered()) {
                    color = this->hover_background();
                } else {
                    color = this->background();
                }
                
                dc->fillRect(rect, color);
                dc->drawTextAligned(this->m_text, this->m_text_align, rect, this->m_padding + this->m_border_width / 2);
            }

        protected:
            const char *m_name = "SliderButton";
    };

    class Slider : public Box {
        public:
            float m_min = 0.0f;
            float m_max = 1.0f;
            float m_value = 0.5f;
            void (*value_changed_callback)(Slider*) = nullptr;

            Slider(Align alignment, std::string text = "") : Box(alignment) {
                this->m_slider_button = new SliderButton(text);
                this->append(this->m_slider_button, Fill::None);
                this->m_slider_button->m_parent = this;
                this->m_slider_button->mouse_motion_callback = onMouseMove;
            }
            
            ~Slider() {}

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
                
                dc->fillRect(rect, color);
                Size<float> sizehint = this->m_slider_button->size_hint(dc);
                float size;
                if (this->m_align_policy == Align::Horizontal) size = sizehint.w;
                else size = sizehint.h;
                float *align_rect[2] = {};
                if (this->m_align_policy == Align::Horizontal) {
                    align_rect[0] = &rect.x;
                    align_rect[1] = &rect.w;
                } else {
                    align_rect[0] = &rect.y;
                    align_rect[1] = &rect.h;
                }

                float result = (*align_rect[1] * this->m_value) - (size / 2);
                if (result < 0) {
                } else if (result > (*align_rect[1] - size)) {
                    *align_rect[0] += (*align_rect[1] - size);
                } else {
                    *align_rect[0] += result;
                }
                this->m_slider_button->draw(dc, Rect<float>(rect.x, rect.y, sizehint.w, sizehint.h));
            }

            Size<float> size_hint(DrawingContext *dc) {
                Size<float> size = this->m_slider_button->size_hint(dc);
                    if (this->m_align_policy == Align::Horizontal) size.w *= 2;
                    else size.h *= 2;
                return size;
            }

            Slider* set_foreground(Color foreground) {
                this->bg = foreground;
                this->update();

                return this;
            }

            Slider* set_background(Color background) {
                this->bg = background;
                this->update();

                return this;
            }

        private:
            const char *m_name = "Slider";
            SliderButton *m_slider_button = nullptr;
    };

    void onMouseMove(Widget *slider_button, MouseEvent event) {
        if (slider_button && slider_button->is_pressed()) {
            if (slider_button->m_parent && slider_button->m_parent->name() == "Slider") {
                Slider *parent = (Slider*)slider_button->m_parent;
                Rect<float> rect = parent->rect;
                if (parent->m_align_policy == Align::Horizontal) {
                    float value = parent->m_max - ((rect.x + rect.w - event.x) / (rect.x + rect.w));
                    if (value > parent->m_max) value = parent->m_max;
                    else if (value < parent->m_min) value = parent->m_min;
                    parent->m_value = value;
                } else {
                    float value = parent->m_max - ((rect.y + rect.h - event.y) / (rect.y + rect.h));
                    if (value > parent->m_max) value = parent->m_max;
                    else if (value < parent->m_min) value = parent->m_min;
                    parent->m_value = value;
                }
                if (parent->value_changed_callback) parent->value_changed_callback(parent);
                if (slider_button->m_app) ((Application*)slider_button->m_app)->m_needs_update = true;
            }
        }
    }
#endif