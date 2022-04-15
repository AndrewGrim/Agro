#ifndef TEXT_EDIT_HPP
    #define TEXT_EDIT_HPP

    #include "widget.hpp"
    #include "../core/string.hpp"

    struct TextEdit : public Widget {
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
            String text;
            Selection selection;

            HistoryItem(Action action, String text, Selection selection) :
                        action{action}, text{text}, selection{selection} {}
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

        EventListener<> onTextChanged = EventListener<>();

        TextEdit(String text = "", String placeholder = "", i32 min_length = 50);
        ~TextEdit();
        virtual const char* name() override;
        virtual void draw(DrawingContext &dc, Rect rect, i32 state) override;
        virtual Size sizeHint(DrawingContext &dc) override;
        virtual void handleTextEvent(DrawingContext &dc, const char *text) override;
        TextEdit* setText(String text);
        String text();
        i32 minLength();
        TextEdit* setMinLength(i32 length);
        TextEdit* moveCursorLeft();
        TextEdit* moveCursorRight();
        TextEdit* moveCursorBegin();
        TextEdit* moveCursorEnd();
        TextEdit* deleteAt(u64 index, bool skip = false);
        TextEdit* clear();
        TextEdit* setPlaceholderText(String text);
        String placeholderText();
        TextEdit* updateView();
        TextEdit* jumpWordLeft();
        TextEdit* jumpWordRight();
        bool isShiftPressed();
        void deleteSelection(bool skip = false);
        void selectAll();
        void swapSelection();
        void insert(u64 index, const char *text, bool skip = false);
        void setCursor(u64 index);
        void undo();
        void redo();
        i32 isFocusable() override;

        String m_text;
        String m_placeholder_text;
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
