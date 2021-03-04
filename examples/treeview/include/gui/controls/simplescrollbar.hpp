#ifndef SIMPLE_SCROLLBAR_HPP
    #define SIMPLE_SCROLLBAR_HPP

    #include "widget.hpp"
    #include "slider.hpp"

    class SimpleScrollBarSlider : public Slider {
        public:
            SimpleScrollBarSlider(Align alignment);
            ~SimpleScrollBarSlider();
            virtual const char* name() override;
            virtual void draw(DrawingContext *dc, Rect rect) override;
    };

    class SimpleScrollBar : public Widget {
        public:
            SimpleScrollBarSlider *m_slider = nullptr;

            SimpleScrollBar(Align alignment);
            ~SimpleScrollBar();
            virtual const char* name() override;
            virtual void draw(DrawingContext *dc, Rect rect) override;
            virtual Size sizeHint(DrawingContext *dc) override;
            virtual bool isLayout() override;

        protected:
            Align m_align_policy;
    };  
#endif