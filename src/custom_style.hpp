#ifndef CUSTOM_STYLE_HPP
    #define CUSTOM_STYLE_HPP

    #include "controls/widget.hpp"

    class CustomStyle : public Widget {
        public:
            CustomStyle() {
                style.margin.type = STYLE_ALL;
                style.border.type = STYLE_ALL;
                style.padding.type = STYLE_ALL;

                style.margin.top = 10;
                style.margin.bottom = 20;
                style.margin.left = 30;
                style.margin.right = 40;

                style.border.top = 10;
                style.border.bottom = 20;
                style.border.left = 30;
                style.border.right = 40;
                
                // TODO not working
                style.border.color_top = Color(0.2, 0.4, 0.7);

                style.padding.top = 10;
                style.padding.bottom = 20;
                style.padding.left = 30;
                style.padding.right = 40;
            }

            ~CustomStyle() {
                
            }

            const char* name() override {
                return "CustomStyle";
            }

            void draw(DrawingContext *dc, Rect rect) {
                this->rect = rect;

                dc->fillRect(rect, Color(1, 0, 0, 0.5));
                dc->margin(rect, style);
                dc->drawBorder(rect, style);
                dc->fillRect(rect, Color(0, 1, 0, 0.5));
                dc->padding(rect, style);
                this->inner_rect = rect;

                dc->fillRect(rect, Color(0, 0, 1, 0.5));
            }

            Size sizeHint(DrawingContext *dc) {
                Size s = Size(96, 96);
                dc->sizeHintMargin(s, style);
                dc->sizeHintBorder(s, style);
                dc->sizeHintPadding(s, style);
                return s;
            }
    };
#endif