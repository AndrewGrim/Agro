#ifndef SCROLLABLE_HPP
    #define SCROLLABLE_HPP

    #include "widget.hpp"
    #include "simplescrollbar.hpp"

    class Scrollable : public Widget {
        public:
            SimpleScrollBar *m_horizontal_scrollbar = nullptr;
            SimpleScrollBar *m_vertical_scrollbar = nullptr;

            virtual const char* name() override;
            virtual void draw(DrawingContext *dc, Rect rect) override;
            virtual Size sizeHint(DrawingContext *dc) override;
            virtual bool isLayout() override;
            Point automaticallyAddOrRemoveScrollBars(DrawingContext *dc, Rect &rect, Size virtual_size);
            void drawScrollBars(DrawingContext *dc, Rect &rect, Size virtual_size);
    };
#endif