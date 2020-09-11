#ifndef APP_HPP
    #define APP_HPP

    #include <iostream>

    #include "event.hpp"
    #include "controls/widget.hpp"
    
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

            void fire_mouse_event(MouseEvent event) {
                for (Widget *child : this->main_widget->children) {
                    if ((event.x >= child->rect.x && event.x <= child->rect.x + child->rect.w) &&
                        (event.y >= child->rect.y && event.y <= child->rect.y + child->rect.h)) {
                        printf("mouse x:%d y:%d :: rect x:%d, y:%d, w:%d, h:%d\n", event.x, event.y, child->rect.x, child->rect.y, child->rect.w, child->rect.h);
                        // TODO check if widgets has children an propagate the even through them
                    }
                }
            }

            void run() {
                while (true) {
                    SDL_Event event;
                    if (SDL_WaitEvent(&event)) {
                        switch (event.type) {
                            case SDL_MOUSEBUTTONDOWN:
                            case SDL_MOUSEBUTTONUP:
                                this->fire_mouse_event(MouseEvent(event.button));
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
#endif