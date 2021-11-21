#ifndef LINE_EDIT_HPP
    #define LINE_EDIT_HPP

    #include <string>

    #include "widget.hpp"

    struct Selection {
        size_t begin = 0;
        size_t end = 0;
        int x_begin = 0;
        int x_end = 0;
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
        size_t index = 0;
        bool undo_end = true;
        bool redo_end = true;

        History() {

        }

        HistoryItem get(size_t index) {
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

            LineEdit(std::string text = "", std::string placeholder = "", int min_length = 50);
            ~LineEdit();
            virtual const char* name() override;
            virtual void draw(DrawingContext &dc, Rect rect, int state) override;
            virtual Size sizeHint(DrawingContext &dc) override;
            virtual void handleTextEvent(DrawingContext &dc, const char *text) override;
            LineEdit* setText(std::string text);
            std::string text();
            int minLength();
            LineEdit* setMinLength(int length);
            LineEdit* moveCursorLeft();
            LineEdit* moveCursorRight();
            LineEdit* moveCursorBegin();
            LineEdit* moveCursorEnd();
            LineEdit* deleteAt(size_t index, bool skip = false);
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
            void insert(size_t index, const char *text, bool skip = false);
            void setCursor(size_t index);
            void undo();
            void redo();
            bool isFocusable() override;

            std::string m_text;
            std::string m_placeholder_text;
            double m_min_length = 50;
            double m_min_view = 0.0;
            double m_max_view = 1.0;
            double m_current_view = m_min_view;
            Size m_virtual_size = Size();
            int m_text_height = 0;
            bool m_text_changed = false;
            int m_cursor_width = 1;
            int m_tab_width = 4;
            Selection m_selection;
            History m_history;
    };
#endif
