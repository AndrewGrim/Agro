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

            ScrolledBox(Align align_policy, Size min_size = Size(50, 50));
            ~ScrolledBox();
            virtual const char* name() override;
            virtual void draw(DrawingContext *dc, Rect rect) override;
            virtual Size sizeHint(DrawingContext *dc) override;
            virtual bool isScrollable() override;
            void layoutChildren(DrawingContext *dc, Rect rect);
            void addScrollBar(Align alignment);
            void removeScrollBar(Align alignment);
            bool hasScrollBar(Align alignment);

        protected:
            Size m_viewport = Size();
    };
#endif
