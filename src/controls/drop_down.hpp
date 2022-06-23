#ifndef DROPDOWN_HPP
    #define DROPDOWN_HPP

    #include "../application.hpp"
    #include "list.hpp"

    class DropDown : public Widget {
        public:
            EventListener<Widget*, CellRenderer*, i32> onItemSelected = EventListener<Widget*, CellRenderer*, i32>();

            DropDown(Cells items = Cells());
            ~DropDown();
            virtual const char* name() override;
            virtual void draw(DrawingContext &dc, Rect rect, i32 state) override;
            virtual Size sizeHint(DrawingContext &dc) override;
            i32 isFocusable() override;
            i32 current();
            void setCurrent(i32 index);
            i32 appendItem(CellRenderer *cell);
            CellRenderer* getItem(i32 index);
            void clear();

            bool m_must_close = false;
            Image *m_open_close = (new Image(Application::get()->icons["up_arrow"]))->flipVertically()->setMinSize(Size(12, 12));
            List *m_list = nullptr;
            Size m_biggest_item;
            Window *m_window = nullptr;
    };
#endif
