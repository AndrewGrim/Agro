#ifndef CUSTOM_WIDGET_HPP
    #define CUSTOM_WIDGET_HPP

    #include "controls/widget.hpp"
    #include "controls/image.hpp"

    class CustomWidget : public Widget {
        public:
            Font *mono = new Font("fonts/DejaVu/DejaVuSansMono.ttf", 16, Font::Type::Mono);
            Font *small = new Font("fonts/DejaVu/DejaVuSans.ttf", 12, Font::Type::Sans);
            Font *big = new Font("fonts/DejaVu/DejaVuSans-Bold.ttf", 22, Font::Type::Sans);
            Image *lena = new Image("lena.png");

            CustomWidget() {

            }

            ~CustomWidget() {
                delete lena;
                delete mono;
                delete small;
                delete big;
            }


            const char* name() {
                return "CustomWidget";
            }

            void draw(DrawingContext *dc, Rect rect) {
                this->rect = rect;
                lena->flipBoth();
                dc->fillRect(rect, Color(0.7, 0.7, 0.7));
                dc->fillRectWithGradient(Rect(rect.x, rect.y, 125, 100), Color(), Color(1, 0.1), Gradient::LeftToRight);
                dc->fillRectWithGradient(Rect(rect.x + 125, rect.y, 125, 100), Color(), Color(0.2, 0.7, 0.9), Gradient::TopToBottom);
                dc->fillTextAligned(dc->default_font, "Left Aligned Text", HorizontalAlignment::Left, VerticalAlignment::Top, Rect(rect.x, rect.y, 250, 100), 10, Color(1, 1, 1));
                dc->fillTextAligned(dc->default_font, "Centered Aligned Text", HorizontalAlignment::Center, VerticalAlignment::Center, Rect(rect.x, rect.y, 250, 100), 10, Color(1, 1, 1));
                dc->fillTextAligned(dc->default_font, "Right Aligned Text", HorizontalAlignment::Right, VerticalAlignment::Bottom, Rect(rect.x, rect.y, 250, 100), 10, Color(1, 1, 1));
                dc->fillText(dc->default_font, "lena.png", Point(rect.x + 25, rect.y + 130));
                dc->drawTexture(Point(rect.x + 50, rect.y + 150), Size(24, 24), lena->_texture(), lena->coords());
                dc->drawTexture(Point(rect.x + 125, rect.y + 100), Size(72, 72), lena->_texture(), lena->coords());
                dc->fillRect(Rect(rect.x, rect.y + 175, rect.w, 20), Color());
                for (int i = 0; i < rect.w - 5; i += 10) {
                    dc->drawPoint(Point(rect.x + i, rect.y + 180), Color(0, 1));
                    dc->drawPoint(Point(rect.x + i + 5, rect.y + 190), Color(1, 1, 1));
                }
                dc->drawTexture(Point(rect.x, rect.y + 200), Size(48, 48), lena->_texture(), lena->coords());
                dc->drawTexture(Point(rect.x + 60, rect.y + 200), Size(48, 48), lena->_texture(), lena->coords(), Color(1, 0));
                dc->drawTexture(Point(rect.x + 120, rect.y + 200), Size(48, 48), lena->_texture(), lena->coords(), Color(0, 1));
                dc->drawTexture(Point(rect.x + 180, rect.y + 200), Size(48, 48), lena->_texture(), lena->coords(), Color(0, 0, 1));
            }

            Size sizeHint(DrawingContext *dc) {
                return Size(250, 250);
            }
    };
#endif