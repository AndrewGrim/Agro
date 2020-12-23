#ifndef SLIDER_HPP
    #define SLIDER_HPP

    #include "../app.hpp"
    #include "widget.hpp"
    #include "box.hpp"
    #include "button.hpp"
    #include "../renderer/drawing_context.hpp"

    class SliderButton : public Widget {
        public:
            SliderButton();
            ~SliderButton();
            virtual const char* name() override;
            virtual void draw(DrawingContext *dc, Rect rect) override;
            virtual Size sizeHint(DrawingContext *dc) override; 
    };

    class Slider : public Box {
        public:
            float m_min = 0.0f;
            float m_max = 1.0f;
            float m_value;
            float m_slider_button_size = 0;
            SliderButton *m_slider_button = nullptr;
            std::function<void(void)> onValueChanged = nullptr;

            Slider(Align alignment, float value = 0.0);
            ~Slider();
            virtual const char* name() override;
            virtual void draw(DrawingContext *dc, Rect rect) override;
            virtual Size sizeHint(DrawingContext *dc) override;
            virtual Slider* setBackground(Color background) override;
            virtual Slider* setForeground(Color foreground) override;
    };
#endif