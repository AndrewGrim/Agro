#ifndef COMBO_BOX_HPP
    #define COMBO_BOX_HPP

    #include "drop_down.hpp"
    #include "text_edit.hpp"

    class ComboBox : public DropDown {
        public:
            ComboBox(std::function<bool(String query, CellRenderer *cell)> predicate, Cells items = Cells(), Size min_size = Size(100, 100));
            ~ComboBox();
            virtual const char* name() override;
            virtual void draw(DrawingContext &dc, Rect rect, i32 state) override;

            std::function<bool(String query, CellRenderer *cell)> m_predicate = nullptr;
            TextEdit *m_filter = nullptr;
    };
#endif
