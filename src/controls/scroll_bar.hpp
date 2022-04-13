#ifndef SCROLLBAR_HPP
    #define SCROLLBAR_HPP

    #include "widget.hpp"
    #include "box.hpp"
    #include "icon_button.hpp"
    #include "slider.hpp"
    #include "../renderer/drawing_context.hpp"

    class ScrollBarSlider : public Slider {
        public:
            ScrollBarSlider(Align alignment, f64 value = 0.0);
            ~ScrollBarSlider();
            virtual const char* name() override;
            virtual void draw(DrawingContext &dc, Rect rect, i32 state) override;
    };

    class SimpleScrollBar : public Box {
        public:
            ScrollBarSlider *m_slider = nullptr;

            SimpleScrollBar(Align alignment, Size min_size = Size(10, 10));
            ~SimpleScrollBar();
            virtual const char* name() override;
            virtual void draw(DrawingContext &dc, Rect rect, i32 state) override;
            virtual Size sizeHint(DrawingContext &dc) override;
    };

    class ScrollBar : public Box {
        public:
            IconButton *m_begin_button = nullptr;
            ScrollBarSlider *m_slider = nullptr;
            IconButton *m_end_button = nullptr;

            ScrollBar(Align alignment);
            ~ScrollBar();
            virtual const char* name() override;
            virtual void draw(DrawingContext &dc, Rect rect, i32 state) override;
            virtual Size sizeHint(DrawingContext &dc) override;
    };
#endif
