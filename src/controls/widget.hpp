#ifndef WIDGET_HPP
    #define WIDGET_HPP

    #include "../common/rect.hpp"
    #include "../common/fill.hpp"
    #include "../common/align.hpp"
    #include "../common/point.h"
    #include "../common/size.h"
    #include "../common/color.h"

    class Widget {
        public:
            SDL_Renderer *ren = nullptr;
            Rect rect = Rect { 0, 0, 0, 0 };
            std::vector<Widget*> children;
            void (*mouse_down_callback)(Widget*, MouseEvent) = nullptr;
            void (*mouse_up_callback)(Widget*, MouseEvent) = nullptr;

            Widget() {}
            virtual ~Widget() {}

            virtual const char* name() {
                return this->m_name;
            }

            virtual void draw(SDL_Renderer *ren, Rect rect) {
                this->ren = ren;
                this->rect = rect;
            }

            Widget* append(Widget* widget, Fill fill_policy) {
                widget->set_fill_policy(fill_policy);
                this->children.push_back(widget);

                return this;
            }

            virtual Size size_hint() {
                return Size { 0, 0 }; 
            }

            virtual Color background() {
                return this->bg;
            }

            virtual Widget* set_background(Color background) {
                this->bg = background;
                if (this->ren) this->update();

                return this;
            }

            Widget* set_fill_policy(Fill fill_policy) {
                this->m_fill_policy = fill_policy;

                return this;
            }

            Fill fill_policy() {
                return this->m_fill_policy;
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

            virtual bool is_layout() {
                return false;
            }

            void propagate_mouse_event(MouseEvent event) {
                for (Widget *child : this->children) {
                    if ((event.x >= child->rect.x && event.x <= child->rect.x + child->rect.w) &&
                        (event.y >= child->rect.y && event.y <= child->rect.y + child->rect.h)) {
                        if (child->is_layout()) {
                            child->propagate_mouse_event(event);
                        } else {
                            child->mouse_event(child, event);
                        }
                        break;
                    }
                }
            }

            virtual void update() {
                this->draw(this->ren, this->rect);
            }

            void mouse_event(Widget *child, MouseEvent event) {
                switch (event.type) {
                    case MouseEvent::Type::Down:
                        if (mouse_down_callback) mouse_down_callback(child, event);
                        break;
                    case MouseEvent::Type::Up:
                        if (mouse_up_callback) mouse_up_callback(child, event);
                        break;
                }
            }

        private:
            const char *m_name = "Widget";
            Color fg = { 0, 0, 0, 255 };
            Color bg = { 200, 200, 200, 255 };
            bool m_is_visible = false;
            Fill m_fill_policy = Fill::None;
    };
#endif