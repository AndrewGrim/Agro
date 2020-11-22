#ifndef SLIDER_HPP
    #define SLIDER_HPP

    #include "../app.hpp"
    #include "widget.hpp"
    #include "box.hpp"
    #include "button.hpp"
    #include "../renderer/drawing_context.hpp"

    class SliderButton : public Button {
        public:
            SliderButton(std::string text);
            virtual const char* name();
            void draw(DrawingContext *dc, Rect<float> rect);

        protected:
            const char *m_name = "SliderButton";
    };

    class Slider : public Box {
        public:
            float m_min = 0.0f;
            float m_max = 1.0f;
            float m_value;
            float m_slider_button_size;
            SliderButton *m_slider_button = nullptr;
            std::function<void(Slider*)> value_changed_callback = nullptr;

            Slider(Align alignment, std::string text = "", float value = 0.0);
            ~Slider();
            const char* name();
            virtual void draw(DrawingContext *dc, Rect<float> rect);
            Size<float> size_hint(DrawingContext *dc);
            Slider* set_foreground(Color foreground);
            Slider* set_background(Color background);

        protected:
            const char *m_name = "Slider";
    };
#endif