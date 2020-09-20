#ifndef BUTTON_HPP
    #define BUTTON_HPP

    #include "../app.hpp"
    #include "widget.hpp"
    #include "../renderer/drawing_context.hpp"

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

            void draw(DrawingContext *dc, Rect rect) {
                this->dc = dc;
                this->rect = rect;
                Color color = Color(0, 0, 0, 0); 
                if (this->is_pressed() && this->is_hovered()) {
                    color = this->pressed_background(); 
                } else if (this->is_hovered()) {
                    color = this->hover_background();
                } else {
                    color = this->background();
                }
                
                this->dc->fillRect(this->rect, color);

                // SDL_SetRenderDrawColor(dc, 255, 255, 255, 255);
                // SDL_Point light_border[] = { SDL_Point { rect.x, rect.y + rect.h }, SDL_Point { rect.x, rect.y }, SDL_Point { rect.x + rect.w, rect.y } };
                // SDL_RenderDrawLines(dc, light_border, 3);

                // SDL_SetRenderDrawColor(dc, 0, 0, 0, 255);
                // SDL_Point dark_border[] = { SDL_Point { rect.x + rect.w, rect.y }, SDL_Point { rect.x + rect.w, rect.y + rect.h }, SDL_Point { rect.x, rect.y + rect.h } };
                // SDL_RenderDrawLines(dc, dark_border, 3);
            }

            Size size_hint() {
                return Size{40, 20};
            }

            Color background() {
                return this->bg;
            }

            Button* set_background(Color background) {
                this->bg = background;
                if (this->dc) this->update();

                return this;
            }

            void update() {
                // TODO we might be able to just copy the previous render to a texture
                // then use that as the base and only redraw the dirty area
                // this->m_app->show();
                // SDL_Texture *texture = SDL_CreateTexture(this->dc, SDL_PIXELFORMAT_RGBA8888, SDL_TEXTUREACCESS_TARGET, 400, 400);
                // SDL_SetRenderTarget(this->dc, texture);

                // SDL_SetRenderTarget(this->dc, NULL);
                // SDL_RenderCopy(this->dc, texture, NULL, NULL);
                // this->draw(this->dc, this->rect);
                // SDL_RenderPresent(this->dc);
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