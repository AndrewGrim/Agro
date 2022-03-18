#ifndef DROPDOWN_HPP
    #define DROPDOWN_HPP

    #include "../application.hpp"
    #include "list.hpp"

    class DropDown : public Widget {
        public:
            EventListener<Widget*, CellRenderer*, int> onItemSelected = EventListener<Widget*, CellRenderer*, int>();

            DropDown();
            ~DropDown();
            virtual const char* name() override;
            virtual void draw(DrawingContext &dc, Rect rect, int state) override;
            virtual Size sizeHint(DrawingContext &dc) override;
            int isFocusable() override;
            int current();
            void setCurrent(int index);
            int appendItem(CellRenderer *cell);
            CellRenderer* getItem(int index);
            void clear();

            bool m_is_open = false;
            Image *m_open_close = (new Image(Application::get()->icons["up_arrow"]))->flipVertically()->setMinSize(Size(12, 12));
            List *m_list = new List();
            Size m_biggest_item;
            Window *m_window = nullptr;
    };
#endif
