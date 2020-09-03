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

        void set_expand(GuiLayout expand) {
            this->expand = expand;
        }

    private:
        const GuiElement _id = GUI_ELEMENT_WIDGET;
        Color fg = {0, 0, 0, 255};
        Color bg = {200, 200, 200, 255};
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
            Size size = BoxLayout::size_hint();
            SDL_SetRenderDrawColor(ren, this->bg.red, this->bg.green, this->bg.blue, this->bg.alpha);
            SDL_RenderFillRect(ren,  Rect(x, y, size.width, size.height).get());
        }

        Size size_hint() {
            return Size{200, 200};
        }

        Color background() {
            return this->bg;
        }

    private:
        const GuiElement _id = GUI_ELEMENT_LAYOUT;
        Color fg = {0, 0, 0, 255};
        Color bg = {255, 100, 100, 255};
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

            this->main_widget->draw(this->ren, 0, 0);
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
        sizer->append(std::make_shared<Button>(), GUI_LAYOUT_EXPAND_BOTH);

    app.set_main_widget(sizer);
	app.show();
    app.run();

    return 0; 
} 
