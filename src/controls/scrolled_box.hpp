#ifndef SCROLLEDBOX_HPP
    #define SCROLLEDBOX_HPP

    #include "scrollable.hpp"
    #include "../option.hpp"

    struct BinarySearchData {
        size_t position;
        size_t length;
    };

    struct BinarySearchResult {
        size_t index;
        Option<Widget*> value;
    };

    class ScrolledBox : public Scrollable {
        public:
            ScrolledBox(Align align_policy, Size min_size = Size(100, 100));
            ~ScrolledBox();
            virtual const char* name() override;
            virtual void draw(DrawingContext &dc, Rect rect, int state) override;
            virtual Size sizeHint(DrawingContext &dc) override;
            void layoutChildren(DrawingContext &dc, Rect rect);
            virtual bool handleScrollEvent(ScrollEvent event) override;
            void* propagateMouseEvent(Window *window, State *state, MouseEvent event) override;
            void setAlignPolicy(Align align_policy);
            Align alignPolicy();
            Size calculateChildSize(Size child_hint, float expandable_length, float rect_opposite_length, Widget *child);

            Align m_align_policy = Align::Horizontal;
            unsigned int m_vertical_non_expandable = 0;
            unsigned int m_horizontal_non_expandable = 0;
            unsigned int m_visible_children = 0;

            Size m_widgets_only; // The layout's size without padding, border, margin etc.
            std::vector<BinarySearchData> m_children_positions;
            BinarySearchResult binarySearch(size_t position);
    };
#endif
