#ifndef SCROLLBAR_HPP
    #define SCROLLBAR_HPP

    #include "../app.hpp"
    #include "widget.hpp"
    #include "box.hpp"
    #include "button.hpp"
    #include "slider.hpp"
    #include "../renderer/drawing_context.hpp"

    class ScrollBar : public Widget {
        // TODO could we just inherit from box instead??
        public:
            Button *m_begin_button = nullptr;
            Slider *m_slider = nullptr;
            Button *m_end_button = nullptr;

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