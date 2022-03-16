#ifndef SCROLLABLE_HPP
    #define SCROLLABLE_HPP

    #include "widget.hpp"
    #include "scroll_bar.hpp"
    #include "../option.hpp"

    struct BinarySearchData {
        size_t position;
        size_t length;
    };

    template <typename T> struct BinarySearchResult {
        size_t index;
        Option<T> value;
    };

    class Scrollable : public Widget {
        public:
            ScrollBar *m_horizontal_scrollbar = nullptr;
            ScrollBar *m_vertical_scrollbar = nullptr;

            Scrollable(Size min_size = Size(400, 400));
            ~Scrollable();
            virtual const char* name() override;
            virtual void draw(DrawingContext &dc, Rect rect, int state) override;
            virtual Size sizeHint(DrawingContext &dc) override;
            virtual bool isLayout() override;
            virtual bool isScrollable() override;
            virtual int isFocusable() override;
            virtual Widget* propagateMouseEvent(Window *window, State *state, MouseEvent event) override;
            virtual bool handleScrollEvent(ScrollEvent event) override;
            virtual Widget* handleFocusEvent(FocusEvent event, State *state, FocusPropagationData data) override;
            Point automaticallyAddOrRemoveScrollBars(DrawingContext &dc, Rect &rect, const Size &virtual_size);
            void drawScrollBars(DrawingContext &dc, Rect &rect, const Size &virtual_size);
            Size minSize();
            void setMinSize(Size min_size);
            Rect clip();

            Size m_viewport = Size();
    };
#endif
