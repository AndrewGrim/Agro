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
	GUI_LAYOUT_EXPAND,
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
        Color fg = {0, 0, 0, 255};
        Color bg = {200, 200, 200, 255};

        Widget() {}
        virtual ~Widget() {}

        virtual const Gui id() {
            return this->_id;
        }

        virtual void draw(SDL_Renderer *ren, int x, int y) {}

        virtual Size size_hint() {
            return Size{0, 0}; 
        }

    private:
        const Gui _id = GUI_WIDGET;
};

class Button : public Widget {
    public:
        Button() {}
        ~Button() {}

        const Gui id() {
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

    private:
        const Gui _id = GUI_BUTTON;
};

class TreeIter {
    public:
        std::vector<int> path;

        TreeIter(std::vector<int> path) {
            this->path = path;
        }
};

template <class T> class TreeNode {
    public:
        T item;
        std::vector<TreeNode<T>> children;
        TreeNode<T> *parent;
        // is_visible: boolean = true;
        // is_collapsed: boolean = false;
        TreeIter iter = TreeIter(std::vector<int>());

        TreeNode(T item) {
            this->item = item;
        }
};

template <class T> class Tree {
    public:
        std::vector<TreeNode<T>> tree;

        TreeIter append(TreeIter tree_iter, TreeNode<T> item) {
            if (!tree_iter.path.size()) {
                item.iter = TreeIter(std::vector<int>(this->tree.size()));
                item.parent = nullptr;
                this->tree.push_back(item); 
                
                return item.iter;
            } else {
                TreeNode<T> root = this->tree[tree_iter.path[0]];
                for (int i = 1; i < tree_iter.path.size(); i++) {
                    if (root.children.size()) {
                        root = root.children[tree_iter.path[i]];
                    } else {
                        break;
                    }
                }

                root.children.push_back(item);
                int last_index = root.children.size() - 1; // TODO not sure here
                TreeIter new_iter = TreeIter(tree_iter.path);
                new_iter.path.push_back(last_index);
                TreeIter iter = TreeIter(new_iter.path);
                item.iter = iter;
                item.parent = &root;

                return iter;
            }
        }
};

class Application {
    public:
        SDL_Window *win;
        SDL_Renderer *ren;
        std::vector<int> events;
        Color bg = {155, 155, 155, 255};
        Tree<std::shared_ptr<Widget>> model;
        

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

        void append(std::shared_ptr<Widget> widget) {
            this->model.tree.push_back(TreeNode<std::shared_ptr<Widget>>(widget));
        }

        void draw() {
            SDL_SetRenderDrawColor(this->ren, this->bg.red, this->bg.green, this->bg.blue, this->bg.alpha);
            SDL_RenderClear(this->ren);
        }

        void draw_at(int index, int x, int y) {
            this->model.tree[index].item->draw(this->ren, x, y);

            SDL_RenderPresent(this->ren);
        }

        void show() {
            this->draw();

            int x = 0;
            int y = 0;
            for (int i = 0; i < this->model.tree.size(); i++) {
                this->model.tree[i].item->draw(this->ren, x, y);
                Size size = this->model.tree[i].item->size_hint();
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
                            this->append(std::make_shared<Button>());
                            this->draw_at(this->model.tree.size() - 1, x, y);
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
        app.append(std::make_shared<Button>());
        app.append(std::make_shared<Button>());
        app.show();
        app.run();

    return 0; 
} 
