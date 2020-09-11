#ifndef BUTTON_HPP
    #define BUTTON_HPP

    #include "widget.hpp"

    class Button : public Widget {
        public:
            Button() {}
            ~Button() {}

            const char* name() {
                return this->m_name;
            }

            void draw(SDL_Renderer* ren, Rect rect) {
                this->rect = rect;
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
            const char *m_name = "Button";
            Color fg = {0, 0, 0, 255};
            Color bg = {200, 200, 200, 255};
    };
#endif