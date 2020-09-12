#ifndef BUTTON_HPP
    #define BUTTON_HPP

    #include "widget.hpp"

    class Button : public Widget {
        public:
            Button(Application *app) {
                this->m_id = app->next_id();
            }
            
            ~Button() {}

            const char* name() {
                return this->m_name;
            }

            void draw(SDL_Renderer* ren, Rect rect) {
                this->rect = rect;
                this->ren = ren;
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
                if (this->ren) this->update();

                return this;
            }

            void update() {
                this->draw(this->ren, this->rect);
                SDL_RenderPresent(this->ren);
            }

            bool is_layout() {
                return false;
            }

        private:
            const char *m_name = "Button";
            Color fg = {0, 0, 0, 255};
            Color bg = {200, 200, 200, 255};
    };
#endif