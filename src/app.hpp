#ifndef APP_HPP
    #define APP_HPP

    #include <iostream>

    #include <glad/glad.h>
    #include <SDL2/SDL.h>

    #include "event.hpp"
    #include "state.hpp"
    #include "controls/widget.hpp"
    #include "renderer/drawing_context.hpp"
    #include "renderer/text_renderer.h"
    
    class Application {
        public:
            int id_counter = 0;
            SDL_Window *win;
            SDL_GLContext sdl_context;
            DrawingContext *dc;
            Color bg = Color(1.0f, 1.0f, 1.0f);
            Widget *main_widget;
            Size<int> m_size;
            State *state = new State();
            bool m_needs_update = true;

            Application(const char* title = "Application", Size<int> size = Size<int>(400, 400)) {
                this->m_size = size;

                SDL_Init(SDL_INIT_VIDEO);
                this->win = SDL_CreateWindow(
                    title, 
                    SDL_WINDOWPOS_CENTERED, 
                    SDL_WINDOWPOS_CENTERED, 
                    size.w, size.h,
                    SDL_WINDOW_OPENGL | SDL_WINDOW_RESIZABLE
                );
                sdl_context = SDL_GL_CreateContext(this->win);
                SDL_GL_MakeCurrent(this->win, this->sdl_context);
                SDL_GL_SetAttribute(SDL_GL_CONTEXT_PROFILE_MASK, SDL_GL_CONTEXT_PROFILE_CORE);
                SDL_GL_SetAttribute(SDL_GL_CONTEXT_MAJOR_VERSION, 3);
                SDL_GL_SetAttribute(SDL_GL_CONTEXT_MINOR_VERSION, 3);
                SDL_GL_SetAttribute(SDL_GL_DOUBLEBUFFER, 1);
                SDL_GL_SetSwapInterval(0);
                if (!gladLoadGLLoader((GLADloadproc)SDL_GL_GetProcAddress)) {
                    println("Failed to initialize GLAD");
                }

                dc = new DrawingContext();
                this->dc->set_projection(glm::ortho(
                    0.0f, static_cast<float>(size.w),
                    static_cast<float>(size.h), 0.0f,
                    -1.0f, 1.0f
                ));
            }

            ~Application() {
                // TODO recursively delete
                delete main_widget;
            }

            void draw() {
                this->dc->set_projection(glm::ortho(
                    0.0f, static_cast<float>(this->m_size.w),
                    static_cast<float>(this->m_size.h), 0.0f,
                    -1.0f, 1.0f
                ));
                this->dc->clear();
                this->main_widget->draw(this->dc, Rect<float>(0, 0, this->m_size.w, this->m_size.h));
            }

            void set_main_widget(Widget *widget) {
                this->main_widget = widget;
            }

            void show() {
                this->draw();
                this->dc->swap_buffer(this->win);
            }

            int next_id() {
                return this->id_counter++;
            }

            void run() {
                this->dc->loadFont("fonts/FreeSans.ttf", 14);
                this->main_widget->m_app = (void*)this;
                this->main_widget->dc = this->dc;
                this->main_widget->m_id = this->next_id();
                for (Widget *child : this->main_widget->children) {
                    child->init((void*)this, this->dc);
                }
                this->show();
                while (true) {
                    SDL_Event event;
                    if (SDL_WaitEvent(&event)) {
                        switch (event.type) {
                            case SDL_MOUSEBUTTONDOWN:
                                this->state->pressed = this->main_widget->propagate_mouse_event(this->state, MouseEvent(event.button));
                            case SDL_MOUSEBUTTONUP:
                                this->main_widget->propagate_mouse_event(this->state, MouseEvent(event.button));
                                break;
                            case SDL_MOUSEMOTION:
                                this->state->hovered = this->main_widget->propagate_mouse_event(this->state, MouseEvent(event.motion));
                                break;
                            case SDL_WINDOWEVENT:
                                switch (event.window.event) {
                                    case SDL_WINDOWEVENT_RESIZED:
                                        this->m_size = Size<int> { event.window.data1, event.window.data2 };
                                        int w, h;
                                        SDL_GL_GetDrawableSize(this->win, &w, &h);
                                        glViewport(0, 0, w, h);
                                        this->m_needs_update = true;
                                        break;
                                }
                                break;
                            case SDL_QUIT:
                                goto EXIT;
                        }
                    }
                    if (this->m_needs_update) {
                        this->show();
                        this->m_needs_update = false;
                    }
                }

                EXIT:
                    SDL_GL_DeleteContext(this->sdl_context);
                    SDL_DestroyWindow(this->win);
                    SDL_Quit();
            }
    };
#endif