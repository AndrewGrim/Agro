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
            enum class State {
                Disabled,
                Active,
                Focused,
            };

            int m_id = -1;
            bool m_is_hovered = false;
            bool m_is_pressed = false;
            SDL_Renderer *ren = nullptr;
            Rect rect = Rect { 0, 0, 0, 0 };
            std::vector<Widget*> children;
            void (*mouse_down_callback)(Widget*, MouseEvent) = nullptr;
            void (*mouse_up_callback)(Widget*, MouseEvent) = nullptr;
            void (*mouse_left_callback)(Widget*, MouseEvent) = nullptr;
            void (*mouse_entered_callback)(Widget*, MouseEvent) = nullptr;
            void (*mouse_motion_callback)(Widget*, MouseEvent) = nullptr;

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

            virtual Color hover_background() {
                return this->hover_bg;
            }

            virtual Widget* set_hover_background(Color background) {
                this->hover_bg = background;
                if (this->ren) this->update();

                return this;
            }

            virtual Color pressed_background() {
                return this->pressed_bg;
            }

            virtual Widget* set_pressed_background(Color background) {
                this->pressed_bg = background;
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

            bool is_hovered() {
                return this->m_is_hovered;
            }
            
            void set_hovered(bool hover) {
                this->m_is_hovered = hover;

                this->update();
            }

            bool is_pressed() {
                return this->m_is_pressed;
            }
            
            void set_pressed(bool pressed) {
                this->m_is_pressed = pressed;

                this->update();
            }

            Widget* propagate_mouse_event(Widget *last_mouse_widget, MouseEvent event) {
                for (Widget *child : this->children) {
                    if ((event.x >= child->rect.x && event.x <= child->rect.x + child->rect.w) &&
                        (event.y >= child->rect.y && event.y <= child->rect.y + child->rect.h)) {
                        Widget *last;
                        if (child->is_layout()) {
                            last = child->propagate_mouse_event(last_mouse_widget, event);
                        } else {
                            child->mouse_event(last_mouse_widget, child, event);
                            last = child;
                        }
                        return last;
                    }
                }

                if (last_mouse_widget) {
                    last_mouse_widget->set_pressed(false);
                    last_mouse_widget->set_hovered(false);
                }
                return nullptr;
            }

            virtual void update() {
                // this->draw(this->ren, this->rect);
            }

            void mouse_event(Widget *last_mouse_widget, Widget *child, MouseEvent event) {
                // TODO we dont need the child since "this" is the child!
                // TODO warning both set_pressed and set_hovered issue an update
                // TODO the button pressed behaviour is almost there but there is one more thing:
                // when you press down on a button while the button is not released every hover
                // over said button should show it as pressed
                // and even when the mouse is released elsewhere the button should changed to released
                // will probably need to track the pressed button in app
                switch (event.type) {
                    case MouseEvent::Type::Down:
                        this->set_pressed(true);
                        if (this->mouse_down_callback) this->mouse_down_callback(child, event);
                        break;
                    case MouseEvent::Type::Up:
                        this->set_pressed(false);
                        if (this->mouse_up_callback) this->mouse_up_callback(child, event);
                        break;
                    case MouseEvent::Type::Motion:
                        if (last_mouse_widget) {
                            if (child->m_id != last_mouse_widget->m_id) {
                                last_mouse_widget->set_pressed(false);
                                last_mouse_widget->set_hovered(false);
                                if (last_mouse_widget->mouse_left_callback) {
                                    last_mouse_widget->mouse_left_callback(last_mouse_widget, event);
                                }
                                this->set_hovered(true);
                                if (this->mouse_entered_callback) {
                                    this->mouse_entered_callback(child, event);
                                }
                            }
                        } else {
                            this->set_hovered(true);
                            if (this->mouse_entered_callback) {
                                this->mouse_entered_callback(child, event);
                            }
                        }
                        if (this->mouse_motion_callback) this->mouse_motion_callback(child, event);
                        break;
                }
            }

        private:
            const char *m_name = "Widget";
            Color fg = { 0, 0, 0, 255 };
            Color bg = { 200, 200, 200, 255 };
            Color hover_bg = { 175, 175, 175, 255 };
            Color pressed_bg = { 125, 125, 125, 255 };
            bool m_is_visible = false;
            Fill m_fill_policy = Fill::None;
    };
#endif