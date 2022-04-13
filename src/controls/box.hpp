#ifndef BOX_HPP
    #define BOX_HPP

    #include "widget.hpp"

    class Box : public Widget {
        public:
            Box(Align align_policy);
            ~Box();
            virtual const char* name() override;
            virtual void draw(DrawingContext &dc, Rect rect, i32 state) override;
            virtual void layoutChildren(DrawingContext &dc, Rect rect);
            virtual Size sizeHint(DrawingContext &dc) override;
            void setAlignPolicy(Align align_policy);
            Align alignPolicy();
            virtual bool isLayout() override;
            virtual i32 isFocusable() override;
            virtual Widget* handleFocusEvent(FocusEvent event, State *state, FocusPropagationData data) override;

            /// Align policy dictates in on which axis
            /// the child Widgets will be layed out
            Align m_align_policy;
            u32 m_visible_children = 0;
            u32 m_vertical_non_expandable = 0;
            u32 m_horizontal_non_expandable = 0;

            Size m_widgets_only; // The layout's size without padding, border, margin etc.
    };
#endif
