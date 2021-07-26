#ifndef SPIN_BOX_HPP
    #define SPIN_BOX_HPP

    #include "line_edit.hpp"
    #include "icon_button.hpp"
    #include "../application.hpp"


    // TODO we will want to use a custom icon button like
    // widget for the buttons
    // because we want to style it a particular way
    class SpinBox : public LineEdit {
        public:
            SpinBox(int value = 0, int min_length = 50);
            ~SpinBox();
            virtual const char* name() override;
            virtual void draw(DrawingContext &dc, Rect rect, int state) override;
            virtual Size sizeHint(DrawingContext &dc) override;
            void handleTextEvent(DrawingContext &dc, const char *text) override;
            bool isLayout() override;

            IconButton *m_up_arrow = new IconButton((new Image(Application::get()->icons["up_arrow"]))->setMinSize(Size(8, 8)));
            IconButton *m_down_arrow = new IconButton((new Image(Application::get()->icons["up_arrow"]))->setMinSize(Size(8, 8))->flipVertically());
    };
#endif
