#ifndef LINE_EDIT_HPP
    #define LINE_EDIT_HPP

    #include <string>

    #include "widget.hpp"

    struct Selection {
        u64 begin = 0;
        u64 end = 0;
        i32 x_begin = 0;
        i32 x_end = 0;
        bool mouse_selection = false;

        bool hasSelection() {
            return begin != end;
        }
    };

    struct HistoryItem {
        enum class Action {
            Delete,
            Insert,
        };

        Action action;
        std::string text = "";
        Selection selection;
    };

    struct History {
        std::vector<HistoryItem> items;
        u64 index = 0;
        bool undo_end = true;
        bool redo_end = true;

        History() {

        }

        HistoryItem get(u64 index) {
            HistoryItem item = items[index];
            return item;
        }

        void append(HistoryItem item) {
            if (index && index < items.size() - 1) {
                items.erase(items.begin() + index + 1, items.end());
            } else if (undo_end) {
                items.erase(items.begin(), items.end());
            }

            items.push_back(item);
            index = items.size() - 1;
            undo_end = false;
        }
    };

    class LineEdit : public Widget {
        public:
            EventListener<> onTextChanged = EventListener<>();

            LineEdit(std::string text = "", std::string placeholder = "", i32 min_length = 50);
            ~LineEdit();
            virtual const char* name() override;
            virtual void draw(DrawingContext &dc, Rect rect, i32 state) override;
            virtual Size sizeHint(DrawingContext &dc) override;
            virtual void handleTextEvent(DrawingContext &dc, const char *text) override;
            LineEdit* setText(std::string text);
            std::string text();
            i32 minLength();
            LineEdit* setMinLength(i32 length);
            LineEdit* moveCursorLeft();
            LineEdit* moveCursorRight();
            LineEdit* moveCursorBegin();
            LineEdit* moveCursorEnd();
            LineEdit* deleteAt(u64 index, bool skip = false);
            LineEdit* clear();
            LineEdit* setPlaceholderText(std::string text);
            std::string placeholderText();
            LineEdit* updateView();
            LineEdit* jumpWordLeft();
            LineEdit* jumpWordRight();
            bool isShiftPressed();
            void deleteSelection(bool skip = false);
            void selectAll();
            void swapSelection();
            void insert(u64 index, const char *text, bool skip = false);
            void setCursor(u64 index);
            void undo();
            void redo();
            i32 isFocusable() override;

            std::string m_text;
            std::string m_placeholder_text;
            f64 m_min_length = 50;
            f64 m_min_view = 0.0;
            f64 m_max_view = 1.0;
            f64 m_current_view = m_min_view;
            Size m_virtual_size = Size();
            i32 m_text_height = 0;
            bool m_text_changed = false;
            i32 m_cursor_width = 1;
            i32 m_tab_width = 4;
            Selection m_selection;
            History m_history;
    };
#endif
