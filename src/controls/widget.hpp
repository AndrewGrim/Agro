#ifndef WIDGET_HPP
    #define WIDGET_HPP

    #include "../common/rect.h"
    #include "../common/size.h"
    #include "../common/color.h"
    #include "../common/fill.h"

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
            const GuiElement _id = GUI_ELEMENT_WIDGET;
            Color fg = { 0, 0, 0, 255 };
            Color bg = { 200, 200, 200, 255 };
            bool m_is_visible = false;
    };
#endif