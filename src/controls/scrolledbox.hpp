#ifndef SCROLLEDBOX_HPP
    #define SCROLLEDBOX_HPP

    #include "widget.hpp"
    #include "box.hpp"
    #include "scrollbar.hpp"
    #include "../renderer/drawing_context.hpp"

    class ScrolledBox : public Box {
        public:
            ScrollBar *m_vertical_scrollbar = nullptr;
            ScrollBar *m_horizontal_scrollbar = nullptr;

            ScrolledBox(Align align_policy, Size<float> min_size = Size<float>(50, 50));
            ~ScrolledBox();
            const char* name();
            void draw(DrawingContext *dc, Rect<float> rect);
            void layout_children(DrawingContext *dc, Rect<float> rect);
            void add_scrollbar(Align alignment);
            void remove_scrollbar(Align alignment);
            bool has_scrollbar(Align alignment);
            bool is_scrollable();
            Size<float> size_hint(DrawingContext *dc) override;

        protected:
            const char *m_name = "ScrolledBox";
    };
#endif
