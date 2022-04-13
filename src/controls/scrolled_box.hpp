#ifndef SCROLLEDBOX_HPP
    #define SCROLLEDBOX_HPP

    #include "scrollable.hpp"
    #include "../option.hpp"

    class ScrolledBox : public Scrollable {
        public:
            ScrolledBox(Align align_policy, Size min_size = Size(100, 100));
            ~ScrolledBox();
            virtual const char* name() override;
            virtual void draw(DrawingContext &dc, Rect rect, i32 state) override;
            virtual Size sizeHint(DrawingContext &dc) override;
            void layoutChildren(DrawingContext &dc, Rect rect);
            virtual bool handleScrollEvent(ScrollEvent event) override;
            Widget* propagateMouseEvent(Window *window, State *state, MouseEvent event) override;
            void setAlignPolicy(Align align_policy);
            Align alignPolicy();
            Size calculateChildSize(Size child_hint, i32 expandable_length, i32 rect_opposite_length, Widget *child, i32 &remainder);

            Align m_align_policy = Align::Horizontal;
            u32 m_vertical_non_expandable = 0;
            u32 m_horizontal_non_expandable = 0;
            u32 m_visible_children = 0;

            Size m_widgets_only; // The layout's size without padding, border, margin etc.
            std::vector<BinarySearchData> m_children_positions;
            BinarySearchResult<Widget*> binarySearch(u64 position);
    };
#endif
