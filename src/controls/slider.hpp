#ifndef SLIDER_HPP
    #define SLIDER_HPP

    #include "box.hpp"

    class SliderButton : public Widget {
        public:
            SliderButton(Size min_size = Size(10, 10));
            ~SliderButton();
            virtual const char* name() override;
            virtual void draw(DrawingContext &dc, Rect rect, int state) override;
            virtual Size sizeHint(DrawingContext &dc) override;
            SliderButton* setMinSize(Size size);
            Size minSize();
    };

    class Slider : public Box {
        public:
            float m_min = 0.0f;
            float m_max = 1.0f;
            float m_value = 0.0f;
            float m_step = 0.1f;
            int m_origin = 0;

            float m_slider_button_size = 0;
            SliderButton *m_slider_button = nullptr;
            std::function<void(void)> onValueChanged = nullptr;

            Slider(Align alignment, float value = 0.0f);
            ~Slider();
            virtual const char* name() override;
            virtual void draw(DrawingContext &dc, Rect rect, int state) override;
            virtual Size sizeHint(DrawingContext &dc) override;
            virtual bool handleScrollEvent(ScrollEvent event) override;
    };
#endif
