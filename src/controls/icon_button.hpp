#ifndef ICON_BUTTON_HPP
    #define ICON_BUTTON_HPP

    #include "button.hpp"

    class IconButton : public Button {
        public:
            IconButton(Image *image);
            ~IconButton();
            const char* name() override;
            void draw(DrawingContext &dc, Rect rect, i32 state) override;
            Size sizeHint(DrawingContext &dc) override;
    };
#endif
