#include <iostream>
#include <vector>
#include <memory>

#include <SDL2/SDL.h>

#include "util.hpp"
#include "controls/box.hpp"
#include "controls/button.hpp"

class Application {
    public:
        SDL_Window *win;
        SDL_Renderer *ren;
        Color bg = {155, 155, 155, 255};
        Widget *main_widget;
        Size m_size;

        Application(const char* title = "Application", Size size = Size { 400, 400 }) {
            this->m_size = size;

            SDL_Init(SDL_INIT_VIDEO);
            this->win = SDL_CreateWindow(
                title, 
                SDL_WINDOWPOS_CENTERED, 
                SDL_WINDOWPOS_CENTERED, 
                size.width, size.height,
                SDL_WINDOW_RESIZABLE
            ); 
            this->ren = SDL_CreateRenderer(this->win, -1, SDL_RENDERER_ACCELERATED | SDL_RENDERER_PRESENTVSYNC);
            SDL_SetRenderDrawBlendMode(this->ren, SDL_BLENDMODE_BLEND);
        }

        ~Application() {
            // TODO recursively delete
            delete main_widget;
        }

        void draw() {
            SDL_SetRenderDrawColor(this->ren, this->bg.red, this->bg.green, this->bg.blue, this->bg.alpha);
            SDL_RenderClear(this->ren);

            this->main_widget->draw(this->ren, Rect { 0, 0, this->m_size.width, this->m_size.height });
        }

        void set_main_widget(Widget *widget) {
            this->main_widget = widget;
        }

        void show() {
            this->draw();
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
                            std::cout << "Mouse(" << x << ", " << y << ")\n";
                            break;
                        case SDL_WINDOWEVENT:
                            switch (event.window.event) {
                                case SDL_WINDOWEVENT_RESIZED:
                                    this->m_size = Size { event.window.data1, event.window.data2 };
                                    this->show(); // TODO probably some sort of update function instead of using show manually
                                    break;
                            }
                            break;
                        case SDL_QUIT:
                            goto EXIT;
                    }
                }
            }

            EXIT:
                SDL_DestroyWindow(this->win);
                SDL_DestroyRenderer(this->ren);
                SDL_Quit();
        }
};

int main() { 
    auto app = Application();
    auto sizer = new Box(Align::Vertical);
        Box *inner = new Box(Align::Horizontal);
            inner->append((new Button())->set_background(Color{50, 0, 0, 255}), Fill::Both);
            inner->append((new Button())->set_background(Color{100, 0, 0, 255}), Fill::Both);
            inner->append((new Button())->set_background(Color{150, 0, 0, 255}), Fill::Both);
            inner->append((new Button())->set_background(Color{200, 0, 0, 255}), Fill::Both);
            inner->append((new Button())->set_background(Color{255, 0, 0, 255}), Fill::Both);
        sizer->append(inner, Fill::Vertical);
        sizer->append((new Button())->set_background(Color{0, 255, 0, 255}), Fill::Horizontal);
        sizer->append((new Button())->set_background(Color{0, 0, 255, 255}), Fill::Both);
        sizer->append((new Button())->set_background(Color{255, 0, 255, 255}), Fill::None);

    app.set_main_widget(sizer);
	app.show();
    app.run();

    return 0; 
} 
