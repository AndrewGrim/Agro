#ifndef TEXT_EDIT_HPP
    #define TEXT_EDIT_HPP

    #include "scrollable.hpp"
    #include "../core/string.hpp"
    #include "../timer.hpp"

    struct TextEdit : public Scrollable {
        struct Selection {
            u64 begin = 0;
            u64 end = 0;
            u64 line_begin = 0;
            u64 line_end = 0;

            // Note: These variables work based on absolute text length and
            // are not relative to anything such as the scroll position.
            // Therefore when using them horizontal scroll needs to be accounted for.
            // And in the case of mouse event handlers it needs to be accounted for
            // when comparing text extents against the event but then removed when setting
            // the actual x_begin or x_end members.
            i32 x_begin = 0;
            i32 x_end = 0;

            bool mouse_selection = false;

            bool hasSelection() {
                return (line_begin != line_end) || (begin != end);
            }
        };

        struct HistoryItem {
            enum class Action {
                Delete,
                Backspace,
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

        enum class Mode {
            SingleLine,
            MultiLine
        };

        EventListener<> onTextChanged = EventListener<>();

        TextEdit(String text = "", String placeholder = "", Mode mode = Mode::SingleLine, Size min_size = Size(100, 100));
        ~TextEdit();
        virtual const char* name() override;
        virtual void draw(DrawingContext &dc, Rect rect, i32 state) override;
        virtual Size sizeHint(DrawingContext &dc) override;
        virtual void handleTextEvent(DrawingContext &dc, const char *text) override;
        TextEdit* setText(String text);
        String text();
        void _beginSelection();
        void _endSelection();
        void _updateView(DrawingContext &dc);
        void _moveLeft(DrawingContext &dc);
        TextEdit* moveCursorLeft();
        void _moveRight(DrawingContext &dc);
        TextEdit* moveCursorRight();
        void _moveUp(DrawingContext &dc);
        TextEdit* moveCursorUp();
        void _moveDown(DrawingContext &dc);
        TextEdit* moveCursorDown();
        TextEdit* moveCursorBegin();
        TextEdit* moveCursorEnd();
        TextEdit* clear();
        TextEdit* setPlaceholderText(String text);
        String placeholderText();
        TextEdit* jumpWordLeft();
        TextEdit* jumpWordRight();
        bool isShiftPressed();
        void _updateVirtualWidth();
        // Returns true to indicate that there was a deletion performed, otherwise false.
        bool deleteOne(bool is_backspace = false, bool skip = false);
        // Returns true to indicate that there was a deletion performed, otherwise false.
        bool deleteSelection(bool skip = false);
        void selectAll();
        bool swapSelection();
        void insert(const char *text, bool skip = false);
        // Returns true to indicate that it was able to set the cursor to the specified position, otherwise false.
        bool setCursor(u64 line, u64 codepoint);
        // Returns true to indicate that the action was successful, otherwise false.
        bool undo();
        // Returns true to indicate that the action was successful, otherwise false.
        bool redo();
        i32 isFocusable() override;
        Widget* propagateMouseEvent(Window *window, State *state, MouseEvent event) override;
        bool handleScrollEvent(ScrollEvent event) override;
        String selection();
        Widget* handleFocusEvent(FocusEvent event, State *state, FocusPropagationData data) override;
        bool handleLayoutEvent(LayoutEvent event) override;

        std::vector<String> m_buffer;
        std::vector<u64> m_buffer_length;
        std::vector<String> m_placeholder_buffer;
        Size m_virtual_size = Size();
        i32 m_cursor_width = 2;
        i32 m_tab_width = 4;
        i32 m_line_spacing = 0;
        Selection m_selection;
        History m_history;
        Mode m_mode = Mode::SingleLine;
        u64 m_last_codepoint_index = 0;
        Option<Timer> m_mouse_scroll_callback;
        SDL_MouseMotionEvent m_mouse_scroll_event = { SDL_MOUSEMOTION, 0, 0, 0, 0, 0, 0, 0, 0 };
        bool m_text_changed = true;
    };
#endif
