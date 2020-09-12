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

                SDL_SetRenderDrawColor(ren, 255, 255, 255, 255);
                SDL_Point light_border[] = { SDL_Point { rect.x, rect.y + rect.h }, SDL_Point { rect.x, rect.y }, SDL_Point { rect.x + rect.w, rect.y } };
                SDL_RenderDrawLines(ren, light_border, 3);

                SDL_SetRenderDrawColor(ren, 0, 0, 0, 255);
                SDL_Point dark_border[] = { SDL_Point { rect.x + rect.w, rect.y }, SDL_Point { rect.x + rect.w, rect.y + rect.h }, SDL_Point { rect.x, rect.y + rect.h } };
                SDL_RenderDrawLines(ren, dark_border, 3);
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
                // this->m_app->show();
                // SDL_Texture *texture = SDL_CreateTexture(this->ren, SDL_PIXELFORMAT_RGBA8888, SDL_TEXTUREACCESS_TARGET, 400, 400);
                // SDL_SetRenderTarget(this->ren, texture);

                // SDL_SetRenderTarget(this->ren, NULL);
                // SDL_RenderCopy(this->ren, texture, NULL, NULL);
                // this->draw(this->ren, this->rect);
                // SDL_RenderPresent(this->ren);
                this->m_app->show();
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