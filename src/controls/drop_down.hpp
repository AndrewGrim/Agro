#ifndef DROPDOWN_HPP
    #define DROPDOWN_HPP

    #include "../application.hpp"
    #include "widget.hpp"

    // TODO could probably go into its own file as List or drawable list
    class DropDownList : public Scrollable {
        public:
            DropDownList(Size min_size = Size(100, 100));
            ~DropDownList();
            const char* name() override ;
            void draw(DrawingContext &dc, Rect rect, int state) override;
            Size sizeHint(DrawingContext &dc) override;
            int isFocusable() override;

            std::vector<Drawable*> m_items;
            int m_focused = -1;
            int m_hovered = -1;
    };

    class DropDown : public Widget {
        public:
            EventListener<Widget*, Drawable*, int> onItemSelected = EventListener<Widget*, Drawable*, int>();

            DropDown();
            ~DropDown();
            virtual const char* name() override;
            virtual void draw(DrawingContext &dc, Rect rect, int state) override;
            virtual Size sizeHint(DrawingContext &dc) override;
            int isFocusable() override;
            int current();
            void setCurrent(int index);
            int appendItem(Drawable *drawable);
            Drawable* getItem(int index);
            void clear();

            bool m_is_open = false;
            Image *m_open_close = (new Image(Application::get()->icons["up_arrow"]))->flipVertically()->setMinSize(Size(12, 12));
            DropDownList *m_list = new DropDownList();
            int m_current = -1;
            Size m_biggest_item;
            Window *m_window = nullptr;

            // TODO make icon size controllable from application
            // like scroll amount

            // TODO make sure to try to create the window within the bounds of the screen
            // otherwise it will most likely be moved by the window manager

            // TODO something to consider is whether to restrict the widget
            // to only use cellrenderers for example?
            // at some level i dont want to use widgets because
            // there is always the expectation of being able to respond to events
            // and in this case we want the content to be purely visual and not interactive
    };
#endif
