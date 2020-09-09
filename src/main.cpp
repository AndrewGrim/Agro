#include <iostream>
#include <vector>
#include <memory>

#include <SDL2/SDL.h>

#include "util.hpp"

enum GuiElement {
    GUI_ELEMENT_WIDGET,
    GUI_ELEMENT_BUTTON,
    GUI_ELEMENT_LAYOUT,
};

enum GuiLayout {
	GUI_LAYOUT_VERTICAL,
	GUI_LAYOUT_HORIZONTAL,
    GUI_LAYOUT_EXPAND_NONE,
	GUI_LAYOUT_EXPAND_BOTH,
    GUI_LAYOUT_EXPAND_VERTICAL,
    GUI_LAYOUT_EXPAND_HORIZONTAL,
};

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

class Widget {
    public:
        std::vector<std::shared_ptr<Widget>> children;
        GuiLayout expand = GUI_LAYOUT_EXPAND_NONE;

        Widget() {}
        virtual ~Widget() {}

        virtual const GuiElement id() {
            return this->_id;
        }

        virtual void draw(SDL_Renderer *ren, int x, int y) {}

        void append(std::shared_ptr<Widget> widget, GuiLayout expand) {
            widget.get()->set_expand(expand);
            this->children.push_back(widget);
        }

        virtual Size size_hint() {
            return Size{0, 0}; 
        }

        virtual Color background() {
            return this->bg;
        }

        virtual void set_background(Color background) {
            this->bg = background;
        }

        void set_expand(GuiLayout expand) {
            this->expand = expand;
        }

        GuiLayout get_expand() {
            return this->expand;
        }

        void show() {
            this->m_is_visible = true;
        }

        void hide() {
            this->m_is_visible = false;
        }
        
        bool is_visible() {
            return this->m_is_visible;
        }

    private:
        const GuiElement _id = GUI_ELEMENT_WIDGET;
        Color fg = {0, 0, 0, 255};
        Color bg = {200, 200, 200, 255};
        bool m_is_visible = false;
};

class Button : public Widget {
    public:
        Button() {}
        ~Button() {}

        const GuiElement id() {
            return this->_id;
        }

        void draw(SDL_Renderer* ren, int x = 0, int y = 0) {
            Size size = Button::size_hint();
            SDL_SetRenderDrawColor(ren, this->bg.red, this->bg.green, this->bg.blue, this->bg.alpha);
            SDL_RenderFillRect(ren,  Rect(x, y, size.width, size.height).get());
        }

        Size size_hint() {
            return Size{40, 20};
        }

        Color background() {
            return this->bg;
        }

        void set_background(Color background) {
            this->bg = background;
        }

    private:
        const GuiElement _id = GUI_ELEMENT_BUTTON;
        Color fg = {0, 0, 0, 255};
        Color bg = {200, 200, 200, 255};
};

class BoxLayout : public Widget {
    public:
        GuiLayout layout_direction;

        BoxLayout(GuiLayout layout_direction) {
            this->layout_direction = layout_direction;
            this->expand = expand;
        }

        ~BoxLayout() {}

        const GuiElement id() {
            return this->_id;
        }

        void draw(SDL_Renderer* ren, int x = 0, int y = 0) {
            SDL_SetRenderDrawColor(ren, this->bg.red, this->bg.green, this->bg.blue, this->bg.alpha);
            SDL_RenderFillRect(ren,  Rect(0, 0, x, y).get());

            int extra = 100;
            Point pos = Point { 0, 0 };
            for (std::shared_ptr<Widget> child : this->children) {
                Size size;
                child.get()->set_background(Color { extra, extra, extra, 255});
                extra += 20;
                switch (child.get()->get_expand()) {
                    case GUI_LAYOUT_EXPAND_BOTH:
                        size = Size { x, y / this->children.size() }; // TODO wrong?
                        break;
                    case GUI_LAYOUT_EXPAND_VERTICAL:
                        size = Size { child.get()->size_hint().width, y / this->children.size() };
                        break;
                    case GUI_LAYOUT_EXPAND_HORIZONTAL:
                        size = Size { x, child.get()->size_hint().height }; // TODO wrong?
                        break;
                    case GUI_LAYOUT_EXPAND_NONE:
                    default:
                        size = child.get()->size_hint();
                }
                Color bg = child.get()->background();
                SDL_SetRenderDrawColor(ren, bg.red , bg.green , bg.blue , bg.alpha);
                SDL_RenderFillRect(ren,  Rect(pos.x, pos.y, size.width, size.height).get());
                printf("x: %d, y: %d, w: %d, h: %d\n", pos.x, pos.y, size.width, size.height);
                switch (this->layout_direction) {
                    case GUI_LAYOUT_VERTICAL:
                        pos.y += size.height;
                        break;
                    case GUI_LAYOUT_HORIZONTAL:
                        pos.x += size.width;
                        break;
                }
            }
            // SDL_RenderPresent(ren);
        }

        Size size_hint() {
            return Size{200, 200};
        }

        Color background() {
            return this->bg;
        }

        void set_background(Color background) {
            this->bg = background;
        }

    private:
        const GuiElement _id = GUI_ELEMENT_LAYOUT;
        Color fg = {0, 0, 0, 255};
        Color bg = {220, 220, 220, 255};
};

class Application {
    public:
        SDL_Window *win;
        SDL_Renderer *ren;
        std::vector<int> events;
        Color bg = {155, 155, 155, 255};
        Widget *main_widget;

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

        ~Application() {
            // TODO recursively delete
            delete main_widget;
        }

        void draw() {
            SDL_SetRenderDrawColor(this->ren, this->bg.red, this->bg.green, this->bg.blue, this->bg.alpha);
            SDL_RenderClear(this->ren);

            this->main_widget->draw(this->ren, 400, 400);
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
    auto app = Application();
    // for (std::shared_ptr<Widget> widget : app.model) {
    //     println(widget.get()->id());
    // }
    auto sizer = new BoxLayout(GUI_LAYOUT_VERTICAL);
        sizer->append(std::make_shared<Button>(), GUI_LAYOUT_EXPAND_VERTICAL);
        sizer->append(std::make_shared<Button>(), GUI_LAYOUT_EXPAND_BOTH);
        sizer->append(std::make_shared<Button>(), GUI_LAYOUT_EXPAND_BOTH);
        sizer->append(std::make_shared<Button>(), GUI_LAYOUT_EXPAND_HORIZONTAL);

    app.set_main_widget(sizer);
	app.show();
    app.run();

    return 0; 
} 
