#ifndef RECT_HPP
    #define RECT_HPP

    #include <SDL2/SDL.h>

    class Rect {
        public:
            int x;
            int y;
            int w;
            int h;

            Rect(int x, int y, int w, int h) {
                this->x = x;
                this->y = y;
                this->w = w;
                this->h = h;
            }

            SDL_Rect* to_SDL_Rect() {
                this->sdl_rect = SDL_Rect {
                    this->x,
                    this->y,
                    this->w,
                    this->h
                };

                return &(this->sdl_rect);
            }

        private:
            SDL_Rect sdl_rect;
    };
#endif