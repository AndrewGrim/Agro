#ifndef SPIN_BOX_HPP
    #define SPIN_BOX_HPP

    #include "line_edit.hpp"
    #include "icon_button.hpp"
    #include "../application.hpp"


    class SpinBoxIconButton : public IconButton {
        public:
            SpinBoxIconButton(Image *image);
            ~SpinBoxIconButton();
            void draw(DrawingContext &dc, Rect rect, int state) override;
            const char* name() override;

    };

    class SpinBox : public LineEdit {
        public:
            SpinBox(int value = 0, int min_length = 50);
            ~SpinBox();
            virtual const char* name() override;
            virtual void draw(DrawingContext &dc, Rect rect, int state) override;
            virtual Size sizeHint(DrawingContext &dc) override;
            void handleTextEvent(DrawingContext &dc, const char *text) override;
            bool isLayout() override;

            SpinBoxIconButton *m_up_arrow = new SpinBoxIconButton((new Image(Application::get()->icons["up_arrow"]))->setMinSize(Size(8, 8)));
            SpinBoxIconButton *m_down_arrow = new SpinBoxIconButton((new Image(Application::get()->icons["up_arrow"]))->setMinSize(Size(8, 8))->flipVertically());
    };
#endif
