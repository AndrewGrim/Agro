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
            std::vector<Widget*> children;

            Widget() {}
            virtual ~Widget() {}

            virtual const char* name() {
                return this->m_name;
            }

            virtual void draw(SDL_Renderer *ren, Rect rect) {}

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

        private:
            const char *m_name = "Widget";
            Color fg = { 0, 0, 0, 255 };
            Color bg = { 200, 200, 200, 255 };
            bool m_is_visible = false;
            Fill m_fill_policy = Fill::None;
    };
#endif