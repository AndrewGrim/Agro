#ifndef BOX_HPP
    #define BOX_HPP

    #include "widget.hpp"
    #include "../common/align.hpp"

    class Box : public Widget {
        public:
            Box(Align align_policy);
            ~Box();
            virtual const char* name() override;
            void draw(DrawingContext *dc, Rect rect) override;
            void layoutChildren(DrawingContext *dc, Rect rect);
            virtual Size sizeHint(DrawingContext *dc) override;
            virtual Box* setBackground(Color background) override;
            virtual Box* setForeground(Color background) override;
            void setAlignPolicy(Align align_policy);
            Align alignPolicy();
            virtual bool isLayout() override;

        protected:
            // TODO the color variables likely need to go into the constructor
            // because if we dont override the getter for them they will return
            // the base class variables
            Color m_fg = Color();
            Color m_bg = Color(0.50f, 0.50f, 0.50f);

            /// Align policy dictates in on which axis
            /// the child Widgets will be layed out
            Align m_align_policy;
    };
#endif