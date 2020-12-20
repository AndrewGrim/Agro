#ifndef CUSTOM_WIDGET_HPP
    #define CUSTOM_WIDGET_HPP

    #include <string>

    #include "controls/widget.hpp"

    class CustomWidget : public Widget {
        public:
            CustomWidget() {

            }

            ~CustomWidget() {
                delete m_texture;
            }

            Texture *m_texture = new Texture("Teostra.png");

            const char* name() {
                return "CustomWidget";
            }

            void draw(DrawingContext *dc, Rect rect) {
                this->rect = rect;
                dc->fillRect(rect, Color(0.7, 0.7, 0.7));
                dc->fillRectWithGradient(Rect(rect.x, rect.y, 125, 100), Color(), Color(1, 0.1), Renderer::Gradient::LeftToRight);
                dc->fillRectWithGradient(Rect(rect.x + 125, rect.y, 125, 100), Color(), Color(0.2, 0.7, 0.9), Renderer::Gradient::TopToBottom);
                dc->fillTextAligned(dc->default_font, "Left Aligned Text", TextAlignment::Left, Rect(rect.x, rect.y, 250, 20), 10, Color(1, 1, 1));
                dc->fillTextAligned(dc->default_font, "Centered Aligned Text", TextAlignment::Center, Rect(rect.x, rect.y, 250, 100), 10, Color(1, 1, 1));
                dc->fillTextAligned(dc->default_font, "Right Aligned Text", TextAlignment::Right, Rect(rect.x, rect.y + 80, 250, 20), 10, Color(1, 1, 1));
                dc->fillText(dc->default_font, "Teostra.png", Point(rect.x + 25, rect.y + 130));
                dc->drawImageAtSize(Point(rect.x + 50, rect.y + 140), Size(24, 24), m_texture);
                dc->drawImageAtSize(Point(rect.x + 125, rect.y + 90), Size(96, 96), m_texture);
                dc->fillRect(Rect(rect.x, rect.y + 175, rect.w, 20), Color());
                for (int i = 0; i < rect.w - 5; i += 10) {
                    dc->drawPoint(Point(rect.x + i, rect.y + 180), Color(0, 1));
                    dc->drawPoint(Point(rect.x + i + 5, rect.y + 190), Color(1, 1, 1));
                }
                dc->drawImage(Point(rect.x, rect.y + 200), m_texture);
                dc->drawImage(Point(rect.x + 60, rect.y + 200), m_texture, Color(1, 0));
                dc->drawImage(Point(rect.x + 120, rect.y + 200), m_texture, Color(0, 1));
                dc->drawImage(Point(rect.x + 180, rect.y + 200), m_texture, Color(0, 0, 1));
            }

            Size sizeHint(DrawingContext *dc) {
                return Size(250, 250);
            }
    };
#endif