#ifndef WIDGET_HPP
    #define WIDGET_HPP

    #include "../common/rect.hpp"
    #include "../common/fill.hpp"
    #include "../common/point.h"
    #include "../common/size.h"
    #include "../common/color.h"

    enum GuiLayout {
        GUI_LAYOUT_VERTICAL,
        GUI_LAYOUT_HORIZONTAL,
        GUI_LAYOUT_EXPAND_NONE,
        GUI_LAYOUT_EXPAND_BOTH,
        GUI_LAYOUT_EXPAND_VERTICAL,
        GUI_LAYOUT_EXPAND_HORIZONTAL,
    };

    class Widget {
        public:
            std::vector<Widget*> children;
            GuiLayout expand = GUI_LAYOUT_EXPAND_NONE;

            Widget() {}
            virtual ~Widget() {}

            virtual const char* name() {
                return this->m_name;
            }

            virtual void draw(SDL_Renderer *ren, Rect rect) {}

            Widget* append(Widget* widget, GuiLayout expand) {
                widget->set_expand(expand);
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
            const char *m_name = "Widget";
            Color fg = { 0, 0, 0, 255 };
            Color bg = { 200, 200, 200, 255 };
            bool m_is_visible = false;
    };
#endif