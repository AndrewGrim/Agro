#ifndef BUTTON_HPP
    #define BUTTON_HPP

    #include "../app.hpp"
    #include "widget.hpp"
    #include "../renderer/drawing_context.hpp"

    class Button : public Widget {
        public:
            Application *m_app = nullptr;
            Button(Application *app, std::string text) {
                this->m_id = app->next_id();
                this->m_app = app;
                this->set_text(text);
            }
            
            ~Button() {}

            const char* name() {
                return this->m_name;
            }

            void draw(DrawingContext *dc, Rect<float> rect) {
                // TODO get rid of app, and init with dc, this could be done by messing with the widget in append.
                this->dc = dc;
                this->rect = rect;
                this->set_size(this->dc->measureText(text()));
                Color color = Color(0, 0, 0, 0); 
                if (this->is_pressed() && this->is_hovered()) {
                    color = this->pressed_background(); 
                } else if (this->is_hovered()) {
                    color = this->hover_background();
                } else {
                    color = this->background();
                }
                
                // shadow rectangle
                dc->fillRect(this->rect, Color(0, 0, 0, 1.0f));
                this->rect = Rect<float>(rect.x, rect.y, rect.w - 1, rect.h - 1);
                // actual rectangle
                dc->fillRect(this->rect, color);
                // centered text
                dc->drawText(
                    this->m_text,
                    rect.x + (rect.w / 2) - (this->dc->measureText(this->m_text).w / 2),
                    rect.y + (rect.h / 2) - (this->dc->measureText(this->m_text).h / 2)
                );
            }

            Size<float> size_hint() {
                Size<float> size = this->m_size;
                    if (this->m_text_align == TextAlignment::Center) size.w *= 1.5;
                    size.w += this->m_padding * 2;
                    size.h += this->m_padding * 2;
                return size;
            }

            Color background() {
                return this->bg;
            }

            Button* set_background(Color background) {
                this->bg = background;
                if (this->dc) this->update();

                return this;
            }

            std::string text() {
                return this->m_text;
            }

            Button* set_text(std::string text) {
                this->m_text = text;
                // if (this->dc) this->update();

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
            std::string m_text;
            Size<float> m_size;
            TextAlignment m_text_align = TextAlignment::Center;
            unsigned int m_padding = 10;
            Color fg = {0, 0, 0, 255};
            Color bg = {200, 200, 200, 255};

            Button* set_size(Size<float> size) {
                this->m_size = size;

                return this;
            }
    };
#endif
