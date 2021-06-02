#ifndef SCROLLABLE_HPP
    #define SCROLLABLE_HPP

    #include "widget.hpp"
    #include "scroll_bar.hpp"

    class Scrollable : public Widget {
        public:
            ScrollBar *m_horizontal_scrollbar = nullptr;
            ScrollBar *m_vertical_scrollbar = nullptr;

            Scrollable(Size min_size = Size(400, 400));
            ~Scrollable();
            virtual const char* name() override;
            virtual void draw(DrawingContext &dc, Rect rect) override;
            virtual Size sizeHint(DrawingContext &dc) override;
            virtual bool isLayout() override;
            virtual bool isScrollable() override;
            virtual void* propagateMouseEvent(Window *window, State *state, MouseEvent event) override;
            virtual bool handleScrollEvent(ScrollEvent event) override;
            Point automaticallyAddOrRemoveScrollBars(DrawingContext &dc, Rect &rect, const Size &virtual_size);
            void drawScrollBars(DrawingContext &dc, Rect &rect, const Size &virtual_size);
            Size minSize();
            void setMinSize(Size min_size);
            Rect clip();

        protected:
            Size m_viewport = Size();
    };
#endif
