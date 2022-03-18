#ifndef LIST_HPP
    #define LIST_HPP

    #include "scrollable.hpp"
    #include "cell_renderer.hpp"

    class List : public Scrollable {
        public:
            EventListener<Widget*, CellRenderer*, int> onItemSelected = EventListener<Widget*, CellRenderer*, int>();

            List(Size min_size = Size(100, 100));
            ~List();
            const char* name() override ;
            void draw(DrawingContext &dc, Rect rect, int state) override;
            Size sizeHint(DrawingContext &dc) override;
            int isFocusable() override;
            int current();
            void setCurrent(int index);
            int appendItem(CellRenderer *cell);
            CellRenderer* getItem(int index);
            void clear();

            std::vector<CellRenderer*> m_items;
            int m_focused = -1;
            int m_hovered = -1;
    };
#endif
