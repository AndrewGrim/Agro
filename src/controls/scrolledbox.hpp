#ifndef SCROLLEDBOX_HPP
    #define SCROLLEDBOX_HPP

    #include "scrollable.hpp"

    class ScrolledBox : public Scrollable {
        public:
            ScrolledBox(Align align_policy, Size min_size = Size(100, 100));
            ~ScrolledBox();
            virtual const char* name() override;
            virtual void draw(DrawingContext *dc, Rect rect) override;
            virtual Size sizeHint(DrawingContext *dc) override;
            virtual bool isScrollable() override;
            void layoutChildren(DrawingContext *dc, Rect rect);
            virtual bool handleScrollEvent(ScrollEvent event) override;

        protected:
            Align m_align_policy = Align::Horizontal;
            unsigned int m_vertical_non_expandable = 0;
            unsigned int m_horizontal_non_expandable = 0;
            unsigned int m_visible_children = 0;
    };
#endif
