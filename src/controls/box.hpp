#ifndef BOX_HPP
    #define BOX_HPP

    #include "widget.hpp"
    #include "../common/align.hpp"

    class Box : public Widget {
        public:
            Box(Align align_policy);
            ~Box();
            virtual const char* name() override;
            virtual void draw(DrawingContext *dc, Rect rect) override;
            virtual void layoutChildren(DrawingContext *dc, Rect rect);
            virtual Size sizeHint(DrawingContext *dc) override;
            virtual Box* setBackground(Color background) override;
            virtual Box* setForeground(Color background) override;
            void setAlignPolicy(Align align_policy);
            Align alignPolicy();
            virtual bool isLayout() override;

        protected:
            /// Align policy dictates in on which axis
            /// the child Widgets will be layed out
            Align m_align_policy;
            unsigned int m_visible_children = 0;
            unsigned int m_vertical_non_expandable = 0;
            unsigned int m_horizontal_non_expandable = 0;
    };
#endif