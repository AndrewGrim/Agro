#ifndef SPIN_BOX_HPP
    #define SPIN_BOX_HPP

    #include "line_edit.hpp"
    #include "icon_button.hpp"
    #include "../application.hpp"
    #include "../result.hpp"


    class SpinBoxIconButton : public IconButton {
        public:
            SpinBoxIconButton(Image *image);
            ~SpinBoxIconButton();
            void draw(DrawingContext &dc, Rect rect, i32 state) override;
            const char* name() override;

    };

    class SpinBox : public LineEdit {
        public:
            enum class Error {
                InvalidArgument,
                OutOfRange,
            };

            EventListener<Widget*, Error> onParsingError = EventListener<Widget*, Error>();

            SpinBox(f64 value = 0, i32 min_length = 50);
            ~SpinBox();
            virtual const char* name() override;
            virtual void draw(DrawingContext &dc, Rect rect, i32 state) override;
            virtual Size sizeHint(DrawingContext &dc) override;
            bool isLayout() override;
            Result<SpinBox::Error, f64> value();

            SpinBoxIconButton *m_up_arrow = new SpinBoxIconButton((new Image(Application::get()->icons["up_arrow"]))->setMinSize(Size(8, 8)));
            SpinBoxIconButton *m_down_arrow = new SpinBoxIconButton((new Image(Application::get()->icons["up_arrow"]))->setMinSize(Size(8, 8))->flipVertically());
    };
#endif
