#include <iostream>
#include <vector>

#include <SDL2/SDL.h>

class Rect {
    public:
        Rect(int x, int y, int w, int h) {
            this->sdl_rect = SDL_Rect {
                x,
                y,
                w,
                h
            };
        }

        SDL_Rect* get() {
            return &this->sdl_rect;
        }

    private:
        SDL_Rect sdl_rect;
};

typedef struct Point {
    int x;
    int y;
} Point;

typedef struct Size {
    int width;
    int height;
} Size;

typedef struct Color {
    int red;
    int green;
    int blue;
    int alpha;
} Color;

class Button {
    public:
        Color bg = {200, 200, 200, 255};

        Button() {
        }

        Size size_hint() {
            return Size{40, 20};
        }

        void draw(SDL_Renderer* ren, int x = 0, int y = 0) {
            Size size = Button::size_hint();
            SDL_SetRenderDrawColor(ren, this->bg.red, this->bg.green, this->bg.blue, this->bg.alpha);
            SDL_RenderFillRect(ren,  Rect(x, y, size.width, size.height).get());
        }
};

class Application {
    public:
        SDL_Window *win;
        SDL_Renderer *ren;
        std::vector<int> events;
        Color bg = {155, 155, 155, 255};
        std::vector<Button> widgets;

        Application(const char* title = "Application", int width = 400, int height = 400) {
            SDL_Init(SDL_INIT_VIDEO);
            this->win = SDL_CreateWindow(
                title, 
                SDL_WINDOWPOS_CENTERED, 
                SDL_WINDOWPOS_CENTERED, 
                width, height, 0
            ); 
            this->ren = SDL_CreateRenderer(this->win, -1, SDL_RENDERER_ACCELERATED | SDL_RENDERER_PRESENTVSYNC);
            SDL_SetRenderDrawBlendMode(this->ren, SDL_BLENDMODE_BLEND);
        }

        void append(Button button) {
            this->widgets.push_back(button);
        }

        void draw() {
            SDL_SetRenderDrawColor(this->ren, this->bg.red, this->bg.green, this->bg.blue, this->bg.alpha);
            SDL_RenderClear(this->ren);
        }

        void draw_at(int index, int x, int y) {
            this->widgets[index].draw(this->ren, x, y);

            SDL_RenderPresent(this->ren);
        }

        void show() {
            this->draw();

            int x = 0;
            int y = 0;
            for (int i = 0; i < this->widgets.size(); i++) {
                this->widgets[i].draw(this->ren, x, y);
                Size size = this->widgets[i].size_hint();
                x += size.width;
                y += size.height;
            }

            SDL_RenderPresent(this->ren);
        }

        void run() {
            while (true) {
                SDL_Event event;
                if (SDL_WaitEvent(&event)) {
                    switch (event.type) {
                        case SDL_MOUSEBUTTONDOWN:
                            int x, y;
                            SDL_GetMouseState(&x, &y);
                            this->append(Button());
                            this->draw_at(this->widgets.size() - 1, x, y);
                            break;
                        case SDL_QUIT:
                            goto EXIT;
                    }
                }

                if (this->events.size()) {
                    std::cout << this->events.size() << "\n";
                }
            }

            EXIT:
                SDL_DestroyWindow(this->win);
                SDL_DestroyRenderer(this->ren);
                SDL_Quit();
        }
};

int main() { 
    Application app = Application();
        app.append(Button());
        app.append(Button());
        app.show();
        app.run();

    return 0; 
} 
