#ifndef SCROLLBAR_HPP
    #define SCROLLBAR_HPP

    #include "../app.hpp"
    #include "widget.hpp"
    #include "box.hpp"
    #include "button.hpp"
    #include "slider.hpp"
    #include "../renderer/drawing_context.hpp"

    void _onBeginClicked(Widget *scrollbar_begin_button, MouseEvent event);
    void _onEndClicked(Widget *scrollbar_begin_button, MouseEvent event);

    class ScrollBar : public Widget {
        public:
            Button *m_begin_button = nullptr;
            Slider *m_slider = nullptr;
            Button *m_end_button = nullptr;
            Align m_align_policy;
            std::string m_text;

            ScrollBar(Align alignment, std::string text = "");
            ~ScrollBar();
            const char* name();
            void draw(DrawingContext *dc, Rect<float> rect);
            Size<float> size_hint(DrawingContext *dc);
            bool is_layout();

        protected:
            const char *m_name = "ScrollBar";
    };  
#endif