#ifndef SCROLLBAR_HPP
    #define SCROLLBAR_HPP

    #include "widget.hpp"
    #include "box.hpp"
    #include "button.hpp"
    #include "slider.hpp"
    #include "../renderer/drawing_context.hpp"

    class ScrollBarArrowButton : public Button {
        public:
            ScrollBarArrowButton(Image *image);
            ~ScrollBarArrowButton();
            virtual const char* name() override;
            virtual void draw(DrawingContext &dc, Rect rect, int state) override;
            virtual Size sizeHint(DrawingContext &dc) override;
    };

    class ScrollBarSlider : public Slider {
        public:
            ScrollBarSlider(Align alignment, double value = 0.0);
            ~ScrollBarSlider();
            virtual const char* name() override;
            virtual void draw(DrawingContext &dc, Rect rect, int state) override;
    };

    class SimpleScrollBar : public Box {
        public:
            ScrollBarSlider *m_slider = nullptr;

            SimpleScrollBar(Align alignment, Size min_size = Size(10, 10));
            ~SimpleScrollBar();
            virtual const char* name() override;
            virtual void draw(DrawingContext &dc, Rect rect, int state) override;
            virtual Size sizeHint(DrawingContext &dc) override;
    };

    class ScrollBar : public Box {
        public:
            ScrollBarArrowButton *m_begin_button = nullptr;
            ScrollBarSlider *m_slider = nullptr;
            ScrollBarArrowButton *m_end_button = nullptr;

            ScrollBar(Align alignment);
            ~ScrollBar();
            virtual const char* name() override;
            virtual void draw(DrawingContext &dc, Rect rect, int state) override;
            virtual Size sizeHint(DrawingContext &dc) override;
    };
#endif
