#ifndef BUTTON_HPP
    #define BUTTON_HPP

    #include "widget.hpp"

    class Button : public Widget {
        public:
            Application *m_app = nullptr;
            Button(Application *app) {
                this->m_id = app->next_id();
                m_app = app;
            }
            
            ~Button() {}

            const char* name() {
                return this->m_name;
            }

            void draw(SDL_Renderer* ren, Rect rect) {
                this->rect = rect;
                this->ren = ren;
                Color color = this->is_hovered() ? this->hover_background() : this->background();
                SDL_SetRenderDrawColor(ren, color.red, color.green, color.blue, color.alpha);
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
                // TODO we might be able to just copy the previous render to a texture
                // then use that as the base and only redraw the dirty area
                this->m_app->show();
                // SDL_RenderPresent(this->ren);
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