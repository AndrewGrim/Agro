#ifndef LIST_HPP
    #define LIST_HPP

    #include "scrollable.hpp"
    #include "cell_renderer.hpp"

    class List : public Scrollable {
        public:
            EventListener<Widget*, CellRenderer*, i32> onItemSelected = EventListener<Widget*, CellRenderer*, i32>();

            List(Cells items = Cells(), Size min_size = Size(100, 100));
            ~List();
            const char* name() override ;
            void draw(DrawingContext &dc, Rect rect, i32 state) override;
            Size sizeHint(DrawingContext &dc) override;
            i32 isFocusable() override;
            i32 current();
            void setCurrent(i32 index);
            i32 appendItem(CellRenderer *cell);
            CellRenderer* getItem(i32 index);
            void clear();

            Cells m_items;
            i32 m_focused = -1;
            i32 m_hovered = -1;
            std::function<bool(String query, CellRenderer *cell)> m_predicate = nullptr;
            String m_query;
    };
#endif
