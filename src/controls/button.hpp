#ifndef BUTTON_HPP
    #define BUTTON_HPP

    #include "../common/rect.h"
    #include "../common/size.h"
    #include "../common/color.h"
    #include "../common/fill.h"
    
    #include "widget.hpp"

    class Button : public Widget {
        public:
            Button() {}
            ~Button() {}

            const GuiElement id() {
                return this->_id;
            }

            void draw(SDL_Renderer* ren, Rect rect) {
                SDL_SetRenderDrawColor(ren, this->bg.red, this->bg.green, this->bg.blue, this->bg.alpha);
                SDL_RenderFillRect(ren,  rect.to_SDL_Rect());
            }

            Size size_hint() {
                return Size{40, 20};
            }

            Color background() {
                return this->bg;
            }

            Button* set_background(Color background) {
                this->bg = background;

                return this;
            }

        private:
            const GuiElement _id = GUI_ELEMENT_BUTTON;
            Color fg = {0, 0, 0, 255};
            Color bg = {200, 200, 200, 255};
    };
#endif