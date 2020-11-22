#ifndef WIDGET_HPP
    #define WIDGET_HPP

    #include <vector>
    #include <functional>

    #include "../state.hpp"
    #include "../event.hpp"
    #include "../common/rect.hpp"
    #include "../common/fill.hpp"
    #include "../common/align.hpp"
    #include "../common/point.hpp"
    #include "../common/size.hpp"
    #include "../common/color.hpp"

    #include "../renderer/drawing_context.hpp"

    class Widget {
        public:
            void *m_app = nullptr;
            bool m_is_hovered = false;
            bool m_is_pressed = false;
            Widget *m_parent = nullptr;
            Rect<float> rect = Rect<float>(0, 0, 0, 0);
            Size<float> m_size;
            Size<float> m_min_size;
            bool size_changed = true;
            std::vector<Widget*> children;
            std::function<void(MouseEvent)> mouse_down_callback = nullptr;
            std::function<void(MouseEvent)> mouse_up_callback = nullptr;
            std::function<void(MouseEvent)> mouse_click_callback = nullptr;
            std::function<void(MouseEvent)> mouse_left_callback = nullptr;
            std::function<void(MouseEvent)> mouse_entered_callback = nullptr;
            std::function<void(MouseEvent)> mouse_motion_callback = nullptr;

            Widget();
            virtual ~Widget();
            virtual const char* name();
            virtual void draw(DrawingContext *dc, Rect<float> rect);
            Widget* append(Widget* widget, Fill fill_policy);
            virtual Size<float> size_hint(DrawingContext *dc);
            virtual Color background();
            virtual Widget* set_background(Color background);
            virtual Color hover_background();
            virtual Widget* set_hover_background(Color background);
            virtual Color pressed_background();
            virtual Widget* set_pressed_background(Color background);
            Widget* set_fill_policy(Fill fill_policy);
            Fill fill_policy();
            void show();
            void hide();
            bool is_visible();
            virtual bool is_layout();
            virtual bool is_scrollable();
            bool is_hovered();
            void set_hovered(bool hover);
            bool is_pressed();
            void set_pressed(bool pressed);
            void update();
            void* propagate_mouse_event(State *state, MouseEvent event);
            void mouse_event(State *state, MouseEvent event);
            void attach_app(void *app);

        protected:
            const char *m_name = "Widget";
            Color fg = Color();
            Color bg = Color();
            Color hover_bg = Color(0.8f, 0.8f, 0.8f);
            Color pressed_bg = Color(0.6f, 0.6f, 0.6f);
            bool m_is_visible = false;
            Fill m_fill_policy = Fill::None;
    };
#endif