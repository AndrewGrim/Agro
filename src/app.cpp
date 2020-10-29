#include "app.hpp"

Application::Application(const char* title, Size<int> size) {
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
    dc->quadRenderer->shader.setMatrix4("projection", glm::ortho(
            0.0f, static_cast<float>(size.w),
            static_cast<float>(size.h), 0.0f,
            -1.0f, 1.0f
        ), 
        true
    );
    dc->textRenderer->shader.setMatrix4("projection", glm::ortho(
            0.0f, static_cast<float>(size.w),
            static_cast<float>(size.h), 0.0f,
            -1.0f, 1.0f
        ), 
        true
    );
}

Application::~Application() {
    // TODO recursively delete
    delete main_widget;
}

void Application::draw() {
    dc->quadRenderer->shader.setMatrix4("projection", glm::ortho(
            0.0f, static_cast<float>(this->m_size.w),
            static_cast<float>(this->m_size.h), 0.0f,
            -1.0f, 1.0f
        ), 
        true
    );
    dc->textRenderer->shader.setMatrix4("projection", glm::ortho(
            0.0f, static_cast<float>(this->m_size.w),
            static_cast<float>(this->m_size.h), 0.0f,
            -1.0f, 1.0f
        ), 
        true
    );
    this->dc->clear();
    this->main_widget->draw(this->dc, Rect<float>(0, 0, this->m_size.w, this->m_size.h));
    this->dc->render();
}

void Application::set_main_widget(Widget *widget) {
    this->main_widget = widget;
}

void Application::show() {
    this->draw();
    this->dc->swap_buffer(this->win);
}

int Application::next_id() {
    return this->id_counter++;
}

void Application::run() {
    dc->textRenderer->load("fonts/FreeSans.ttf", 14);
    dc->textRenderer->shader.use();
    dc->textRenderer->shader.setInt("text", 0);
    this->main_widget->m_app = (void*)this;
    for (Widget *child : this->main_widget->children) {
        child->m_app = (void*)this;
        child->attach_app((void*)this);
    }
    this->show();
    if (this->ready_callback) {
        this->ready_callback(this);
    }
    while (true) {
        SDL_Event event;
        if (SDL_WaitEvent(&event)) {
            int64_t time_since_last_event = std::chrono::duration_cast<std::chrono::milliseconds>(std::chrono::steady_clock::now() - this->m_last_event_time).count();
            switch (event.type) {
                case SDL_MOUSEBUTTONDOWN:
                    this->state->pressed = this->main_widget->propagate_mouse_event(this->state, MouseEvent(event.button, time_since_last_event));
                case SDL_MOUSEBUTTONUP:
                    this->main_widget->propagate_mouse_event(this->state, MouseEvent(event.button, time_since_last_event));
                    break;
                case SDL_MOUSEMOTION:
                    this->state->hovered = this->main_widget->propagate_mouse_event(this->state, MouseEvent(event.motion, time_since_last_event));
                    break;
                case SDL_WINDOWEVENT:
                    switch (event.window.event) {
                        case SDL_WINDOWEVENT_RESIZED:
                            this->m_size = Size<int> { event.window.data1, event.window.data2 };
                            int w, h;
                            SDL_GL_GetDrawableSize(this->win, &w, &h);
                            glViewport(0, 0, w, h);
                            this->m_needs_update = true;
                            this->m_layout_changed = true;
                            break;
                    }
                    break;
                case SDL_QUIT:
                    goto EXIT;
            }
            if (this->m_last_event.second == EventHandler::Accepted) {
                this->m_last_event_time = std::chrono::steady_clock::now();
            }
        }
        if (this->m_needs_update) {
            this->show();
            this->m_needs_update = false;
            this->m_layout_changed = false;
        }
    }

    EXIT:
        SDL_GL_DeleteContext(this->sdl_context);
        SDL_DestroyWindow(this->win);
        SDL_Quit();
}

Widget* Application::append(Widget* widget, Fill fill_policy) {
    widget->set_fill_policy(fill_policy);
    this->main_widget->children.push_back(widget);
    widget->m_app = this;

    return this->main_widget;
}