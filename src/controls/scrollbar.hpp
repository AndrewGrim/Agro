#ifndef SCROLLBAR_HPP
    #define SCROLLBAR_HPP

    #include "../app.hpp"
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
            virtual void draw(DrawingContext *dc, Rect rect) override;
            virtual Size sizeHint(DrawingContext *dc) override;
    };

    class ScrollBarSlider : public Slider {
        public:
            ScrollBarSlider(Align alignment);
            ~ScrollBarSlider();
            virtual const char* name() override;
            virtual void draw(DrawingContext *dc, Rect rect) override;
    };

    class ScrollBar : public Widget {
        // TODO could we just inherit from box instead??
        public:
            ScrollBarArrowButton *m_begin_button = nullptr;
            ScrollBarSlider *m_slider = nullptr;
            ScrollBarArrowButton *m_end_button = nullptr;

            ScrollBar(Align alignment);
            ~ScrollBar();
            virtual const char* name() override;
            virtual void draw(DrawingContext *dc, Rect rect) override;
            virtual Size sizeHint(DrawingContext *dc) override;
            virtual bool isLayout() override;

        protected:
            Align m_align_policy;
    };  
#endif