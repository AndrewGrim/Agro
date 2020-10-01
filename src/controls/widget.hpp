#ifndef WIDGET_HPP
    #define WIDGET_HPP

    #include <vector>

    #include <glad/glad.h>

    #include "../app.hpp"
    #include "../state.hpp"

    #include "../common/rect.hpp"
    #include "../common/fill.hpp"
    #include "../common/align.hpp"
    #include "../common/point.hpp"
    #include "../common/size.hpp"
    #include "../common/color.hpp"

    #include "../renderer/drawing_context.hpp"

    class Widget {
        public:
            int m_id = -1;
            bool m_is_hovered = false;
            bool m_is_pressed = false;
            void *m_app = nullptr;
            DrawingContext *dc = nullptr;
            Rect<float> rect = Rect<float>(0, 0, 0, 0);
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

            virtual void draw(DrawingContext *dc, Rect<float> rect) {
                this->dc = dc;
                this->rect = rect;
            }

            virtual Widget* append(Widget* widget, Fill fill_policy) {
                widget->set_fill_policy(fill_policy);
                this->children.push_back(widget);
                widget->m_app = this->m_app;
                widget->dc = this->dc;

                return this;
            }

            virtual Size<float> size_hint() {
                return Size<float>(0, 0); 
            }

            virtual Color background() {
                return this->bg;
            }

            virtual Widget* set_background(Color background) {
                this->bg = background;
                if (this->dc) this->update();

                return this;
            }

            virtual Color hover_background() {
                return this->hover_bg;
            }

            virtual Widget* set_hover_background(Color background) {
                this->hover_bg = background;
                if (this->dc) this->update();

                return this;
            }

            virtual Color pressed_background() {
                return this->pressed_bg;
            }

            virtual Widget* set_pressed_background(Color background) {
                this->pressed_bg = background;
                if (this->dc) this->update();

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

            virtual void update() {}

            void* propagate_mouse_event(State *state, MouseEvent event) {
                for (Widget *child : this->children) {
                    if ((event.x >= child->rect.x && event.x <= child->rect.x + child->rect.w) &&
                        (event.y >= child->rect.y && event.y <= child->rect.y + child->rect.h)) {
                        void *last;
                        if (child->is_layout()) {
                            last = (void*)child->propagate_mouse_event(state, event);
                        } else {
                            child->mouse_event(state, event);
                            last = (void*)child;
                        }
                        return last;
                    }
                }

                if (event.type == MouseEvent::Type::Up && state->pressed) {
                    ((Widget*)state->pressed)->set_pressed(false);
                    ((Widget*)state->pressed)->set_hovered(false);
                    state->hovered = nullptr;
                    state->pressed = nullptr;
                }
                if (event.type == MouseEvent::Type::Motion && state->hovered) {
                    ((Widget*)state->hovered)->set_hovered(false);
                    state->hovered = nullptr;
                }
                return nullptr;
            }

            void mouse_event(State *state, MouseEvent event) {
                // TODO when the user clicks outside the window
                // hovered and pressed should be reset
                switch (event.type) {
                    case MouseEvent::Type::Down:
                        this->set_pressed(true);
                        if (this->mouse_down_callback) this->mouse_down_callback(this, event);
                        break;
                    case MouseEvent::Type::Up:
                        ((Widget*)state->pressed)->set_pressed(false);
                        ((Widget*)state->pressed)->set_hovered(false);
                        this->set_hovered(true);
                        state->hovered = this;
                        state->pressed = nullptr;
                        if (this->mouse_up_callback) this->mouse_up_callback(this, event);
                        // TODO we could add a click event here
                        // simply check if pressed and this are the same
                        // then we know that both the down and up events were on the same target
                        break;
                    case MouseEvent::Type::Motion:
                        if (!state->pressed) {
                            if (state->hovered) {
                                if (this->m_id != ((Widget*)state->hovered)->m_id) {
                                    ((Widget*)state->hovered)->set_hovered(false);
                                    if (((Widget*)state->hovered)->mouse_left_callback) {
                                        ((Widget*)state->hovered)->mouse_left_callback((Widget*)state->hovered, event);
                                    }
                                    this->set_hovered(true);
                                    if (this->mouse_entered_callback) {
                                        this->mouse_entered_callback(this, event);
                                    }
                                }
                            } else {
                                this->set_hovered(true);
                                if (this->mouse_entered_callback) {
                                    this->mouse_entered_callback(this, event);
                                }
                            }
                            if (this->mouse_motion_callback) this->mouse_motion_callback(this, event);
                        } else {
                            if (state->pressed == state->hovered) {
                                ((Widget*)state->pressed)->set_hovered(true);
                            } else {
                                ((Widget*)state->pressed)->set_hovered(false);
                            }
                        }
                        break;
                }
            }

        private:
            const char *m_name = "Widget";
            Color fg = Color();
            Color bg = Color();
            Color hover_bg = Color(0.8f, 0.8f, 0.8f);
            Color pressed_bg = Color(0.6f, 0.6f, 0.6f);
            bool m_is_visible = false;
            Fill m_fill_policy = Fill::None;
    };
#endif