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
            int isFocusable() override;
    };

    class Slider : public Box {
        public:
            double m_min = 0.0;
            double m_max = 1.0;
            double m_value = 0.0;
            double m_step = 0.1;
            int m_origin = 0;

            int m_slider_button_size = 0;
            SliderButton *m_slider_button = nullptr;
            EventListener<> onValueChanged = EventListener<>();

            Slider(Align alignment, double value = 0.0);
            ~Slider();
            virtual const char* name() override;
            virtual void draw(DrawingContext &dc, Rect rect, int state) override;
            virtual Size sizeHint(DrawingContext &dc) override;
            virtual bool handleScrollEvent(ScrollEvent event) override;
    };
#endif
