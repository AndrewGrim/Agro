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
        std::vector<Widget*> children;
        GuiLayout expand = GUI_LAYOUT_EXPAND_NONE;

        Widget() {}
        virtual ~Widget() {}

        virtual const GuiElement id() {
            return this->_id;
        }

        virtual void draw(SDL_Renderer *ren, Rect rect) {}

        void append(Widget* widget, GuiLayout expand) {
            widget->set_expand(expand);
            this->children.push_back(widget);
        }

        virtual Size size_hint() { // TODO instances of overriden size_hint will need to return non static values, based on the widget contents
        // so a button that says "ok" and a button that says "click here for a suprise" should return different sizes, depeneding on the font as well etc.
            return Size{0, 0}; 
        }

        virtual Color background() {
            return this->bg;
        }

        virtual Widget* set_background(Color background) {
            this->bg = background;

            return this;
        }

        Widget* set_expand(GuiLayout expand) {
            this->expand = expand;

            return this;
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

        void draw(SDL_Renderer* ren, Rect rect) {
            SDL_SetRenderDrawColor(ren, this->bg.red, this->bg.green, this->bg.blue, this->bg.alpha);
            SDL_RenderFillRect(ren,  rect.to_SDL_Rect());
        }

        Size size_hint() {
            return Size{40, 20};
        }

        Color background() {
            return this->bg;
        }

        Button* set_background(Color background) {
            this->bg = background;

            return this;
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

        void draw(SDL_Renderer* ren, Rect rect) {
            SDL_SetRenderDrawColor(ren, this->bg.red, this->bg.green, this->bg.blue, this->bg.alpha);
            SDL_RenderFillRect(ren,  rect.to_SDL_Rect());

            layout_children(ren, rect);
        }

        void layout_children(SDL_Renderer *ren, Rect rect) {
            int non_expandable_widgets = 0;
            int reserved_x = 0;
            int reserved_y = 0;
            GuiLayout parent_layout = this->layout_direction;
            for (Widget* child : this->children) {
                GuiLayout child_layout = child->get_expand();
                if (child_layout == GUI_LAYOUT_EXPAND_HORIZONTAL && parent_layout == GUI_LAYOUT_VERTICAL) {
                    non_expandable_widgets += 1;
                    reserved_y += child->size_hint().height;
                } else if (child_layout == GUI_LAYOUT_EXPAND_VERTICAL && parent_layout == GUI_LAYOUT_HORIZONTAL) {
                    non_expandable_widgets += 1;
                    reserved_x += child->size_hint().width;
                } else if (child_layout == GUI_LAYOUT_EXPAND_NONE) {
                    non_expandable_widgets += 1;
                    if (parent_layout == GUI_LAYOUT_HORIZONTAL) reserved_x += child->size_hint().width;
                    else if (parent_layout == GUI_LAYOUT_VERTICAL) reserved_y += child->size_hint().height;
                }
            }

            int child_count = this->children.size() - non_expandable_widgets;
            rect.w -= reserved_x;
            rect.h -= reserved_y;
            Point pos = Point { rect.x, rect.y };
            switch (this->layout_direction) {
                case GUI_LAYOUT_VERTICAL:
                    for (Widget* child : this->children) {
                        Size size;
                        switch (child->get_expand()) {
                            case GUI_LAYOUT_EXPAND_BOTH:
                                size = Size { rect.w, rect.h / child_count };
                                break;
                            case GUI_LAYOUT_EXPAND_VERTICAL:
                                size = Size { child->size_hint().width, rect.h / child_count };
                                break;
                            case GUI_LAYOUT_EXPAND_HORIZONTAL:
                                size = Size { rect.w, child->size_hint().height };
                                break;
                            case GUI_LAYOUT_EXPAND_NONE:
                            default:
                                size = child->size_hint();
                        }
                        child->draw(ren, Rect { pos.x, pos.y, size.width, size.height });
                        printf("x: %d, y: %d, w: %d, h: %d\n", pos.x, pos.y, size.width, size.height);
                        pos.y += size.height;
                    }
                    break;
                case GUI_LAYOUT_HORIZONTAL:
                    for (Widget* child : this->children) {
                        Size size;
                        switch (child->get_expand()) {
                            case GUI_LAYOUT_EXPAND_BOTH:
                                size = Size { rect.w / child_count, rect.h };
                                break;
                            case GUI_LAYOUT_EXPAND_VERTICAL:
                                size = Size { child->size_hint().width, rect.h };
                                break;
                            case GUI_LAYOUT_EXPAND_HORIZONTAL:
                                size = Size { rect.w / child_count, child->size_hint().height };
                                break;
                            case GUI_LAYOUT_EXPAND_NONE:
                            default:
                                size = child->size_hint();
                        }
                        child->draw(ren, Rect { pos.x, pos.y, size.width, size.height });
                        printf("x: %d, y: %d, w: %d, h: %d\n", pos.x, pos.y, size.width, size.height);
                        pos.x += size.width;
                    }
                    break;
            }
        }

        Size size_hint() {
            return Size{200, 200};
        }

        Color background() {
            return this->bg;
        }

        BoxLayout* set_background(Color background) {
            this->bg = background;

            return this;
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
        Color bg = {155, 155, 155, 255};
        Widget *main_widget;

        Application(const char* title = "Application", int width = 400, int height = 400) {
            SDL_Init(SDL_INIT_VIDEO);
            this->win = SDL_CreateWindow(
                title, 
                SDL_WINDOWPOS_CENTERED, 
                SDL_WINDOWPOS_CENTERED, 
                width, height,
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

            this->main_widget->draw(this->ren, Rect { 0, 0, 400, 400 }); // TODO this need the window size
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
                            this->show(); // TODO probably some sort of update function instead of using show manually
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
    auto sizer = new BoxLayout(GUI_LAYOUT_VERTICAL);
        sizer->append((new Button())->set_background(Color{255, 0, 0, 255}), GUI_LAYOUT_EXPAND_VERTICAL);
        sizer->append((new Button())->set_background(Color{0, 255, 0, 255}), GUI_LAYOUT_EXPAND_HORIZONTAL);
        sizer->append((new Button())->set_background(Color{0, 0, 255, 255}), GUI_LAYOUT_EXPAND_BOTH);
        sizer->append((new Button())->set_background(Color{255, 0, 255, 255}), GUI_LAYOUT_EXPAND_NONE);

    app.set_main_widget(sizer);
	app.show();
    app.run();

    return 0; 
} 
