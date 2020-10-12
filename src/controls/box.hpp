#ifndef BOX_HPP
    #define BOX_HPP

    #include "widget.hpp"
    #include "../renderer/drawing_context.hpp"

    class Box : public Widget {
        public:
            Align m_align_policy;

            Box(Align align_policy);
            ~Box();
            const char* name();
            void draw(DrawingContext *dc, Rect<float> rect);
            void layout_children(DrawingContext *dc, Rect<float> rect);
            virtual Size<float> size_hint(DrawingContext *dc) override;
            Color background();
            Box* set_background(Color background);
            bool is_layout();

        protected:
            const char *m_name = "Box";
            Color fg = Color();
            Color bg = Color(0.50f, 0.50f, 0.50f);
    };
#endif