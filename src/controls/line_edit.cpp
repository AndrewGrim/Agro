#include "line_edit.hpp"
#include "../renderer/renderer.hpp"
#include "../application.hpp"

LineEdit::LineEdit(std::string text, std::string placeholder, int min_length) : Widget() {
    setText(text);
    setPlaceholderText(placeholder);
    setMinLength(min_length);
    m_selection.x_begin = 0;
    m_selection.x_end = m_selection.x_begin;
    onMouseDown.addEventListener([&](Widget *widget, MouseEvent event) {
        m_selection.mouse_selection = true;
        if (!this->text().length()) {
            m_selection.x_begin = 0;
        } else {
            DrawingContext &dc = *Application::get()->dc;
            int x = 0;
            size_t index = 0;
            int offset_event = event.x - inner_rect.x;
            for (char c : this->text()) {
                int w = dc.measureText(font(), c).w;
                if (x + w > offset_event) {
                    if (x + (w / 2) < offset_event) {
                        x += w;
                        index++;
                    }
                    break;
                }
                x += w;
                index++;
            }
            m_selection.x_begin = x;
            m_selection.begin = index;
            m_selection.x_end = m_selection.x_begin;
            m_selection.end = m_selection.begin;
        }
    });
    onMouseMotion.addEventListener([&](Widget *widget, MouseEvent event) {
        if (isPressed()) {
            // No x mouse movement.
            if (event.xrel == 0) {
                return;
            }

            DrawingContext &dc = *Application::get()->dc;
            int x = m_selection.x_begin;
            size_t index = m_selection.begin;
            int offset_event = event.x - inner_rect.x;

            // Selection is to the right of the origin point.
            if (offset_event >= x) {
                while (index < this->text().length()) {
                    char c = this->text()[index];
                    int w = dc.measureText(font(), c).w;
                    if (x + w > offset_event) {
                        break;
                    }
                    x += w;
                    index++;
                }
            // Selection is to the left of the origin point.
            } else {
                while (index) {
                    char c = this->text()[--index];
                    int w = dc.measureText(font(), c).w;
                    x -= w;
                    if (x < offset_event) {
                        break;
                    }
                }
            }

            if (!index) {
                m_current_view = m_min_view;
            } else if (index == this->text().size()) {
                m_current_view = m_max_view;
            } else {
                m_current_view = m_selection.x_end / m_virtual_size.w;
            }
            m_selection.x_end = x;
            m_selection.end = index;
            update();
        }
    });
    onMouseUp.addEventListener([&](Widget *widget, MouseEvent event) {
        m_selection.mouse_selection = false;
    });
    onMouseEntered.addEventListener([&](Widget *widget, MouseEvent event) {
        Application::get()->setMouseCursor(Cursor::IBeam);
    });
    onMouseLeft.addEventListener([&](Widget *widget, MouseEvent event) {
        Application::get()->setMouseCursor(Cursor::Default);
        m_selection.mouse_selection = false;
    });
    auto left = [&]{
        moveCursorLeft();
    };
    bind(SDLK_LEFT, Mod::None, left);
    bind(SDLK_LEFT, Mod::Shift, left);
    auto right = [&]{
        moveCursorRight();
    };
    bind(SDLK_RIGHT, Mod::None, right);
    bind(SDLK_RIGHT, Mod::Shift, right);
    auto home = [&]{
        moveCursorBegin();
    };
    bind(SDLK_HOME, Mod::None, home);
    bind(SDLK_HOME, Mod::Shift, home);
    auto end = [&]{
        moveCursorEnd();
    };
    bind(SDLK_END, Mod::None, end);
    bind(SDLK_END, Mod::Shift, end);
    bind(SDLK_BACKSPACE, Mod::None, [&]{
        if (m_selection.hasSelection()) {
            deleteSelection();
        } else if (m_selection.begin) {
            moveCursorLeft();
            deleteAt(m_selection.begin);
        }
        updateView();
    });
    bind(SDLK_DELETE, Mod::None, [&]{
        if (m_selection.hasSelection()) {
            deleteSelection();
        } else {
            deleteAt(m_selection.begin);
        }
        updateView();
    });
    auto jump_left = [&]{
        jumpWordLeft();
    };
    bind(SDLK_LEFT, Mod::Ctrl, jump_left);
    bind(SDLK_LEFT, Mod::Ctrl|Mod::Shift, jump_left);
    auto jump_right = [&]{
        jumpWordRight();
    };
    bind(SDLK_RIGHT, Mod::Ctrl, jump_right);
    bind(SDLK_RIGHT, Mod::Ctrl|Mod::Shift, jump_right);
    bind(SDLK_a, Mod::Ctrl, [&]{
        selectAll();
    });
    bind(SDLK_v, Mod::Ctrl, [&]{
        if (SDL_HasClipboardText()) {
            char *s = SDL_GetClipboardText();
            if (s) {
                insert(m_selection.end, s);
                SDL_free(s);
            }
        }
    });
    bind(SDLK_c, Mod::Ctrl, [&]{
        if (m_selection.hasSelection()) {
            swapSelection();
            std::string s = this->text().substr(m_selection.begin, m_selection.end);
            SDL_SetClipboardText(s.c_str());
        }
    });
    bind(SDLK_z, Mod::Ctrl, [&]{
        undo();
    });
    bind(SDLK_y, Mod::Ctrl, [&]{
        redo();
    });
}

LineEdit::~LineEdit() {

}

const char* LineEdit::name() {
    return "LineEdit";
}

void LineEdit::draw(DrawingContext &dc, Rect rect, int state) {
    dc.margin(rect, style);
    this->rect = rect;
    dc.drawBorder(rect, style);
    dc.fillRect(rect, dc.textBackground(style));
    dc.padding(rect, style);
    inner_rect = rect;

    Rect old_clip = dc.clip();
    dc.setClip(Rect(rect.x, rect.y, rect.w + 1, rect.h).clipTo(old_clip));

    if (m_virtual_size.w > inner_rect.w) {
        inner_rect.x -= m_current_view * (m_virtual_size.w - inner_rect.w);
    }
    // Draw placeholder text.
    if (!text().length()) {
        dc.fillTextAligned(
            font(),
            placeholderText(),
            HorizontalAlignment::Left,
            VerticalAlignment::Center,
            inner_rect,
            0,
            dc.textDisabled(style)
        );
    // Draw normal text;
    } else {
        if (m_selection.mouse_selection || (isFocused() && m_selection.hasSelection())) {
            int text_height = m_text_height + TOP_PADDING(this) + BOTTOM_PADDING(this);
            int start = m_selection.x_begin < m_selection.x_end ? m_selection.x_begin : m_selection.x_end;
            int end = m_selection.x_begin < m_selection.x_end ? m_selection.x_end : m_selection.x_begin;
            dc.fillRect(
                Rect(
                    inner_rect.x + start,
                    inner_rect.y + (inner_rect.h / 2) - (text_height / 2),
                    (end - start) + m_cursor_width,
                    text_height
                ),
                dc.accentWidgetBackground(style)
            );
        }
        dc.fillTextAligned(
            font(),
            text(),
            HorizontalAlignment::Left,
            VerticalAlignment::Center,
            inner_rect,
            0,
            dc.textForeground(style),
            m_tab_width,
            isFocused() ? Renderer::Selection(m_selection.begin, m_selection.end) : Renderer::Selection(),
            dc.textSelected(style)
        );
    }

    // Draw the text insertion cursor.
    if (isFocused()) {
        int text_height = m_text_height + TOP_PADDING(this) + BOTTOM_PADDING(this);
        dc.fillRect(
            Rect(
                inner_rect.x + m_selection.x_end,
                inner_rect.y + (inner_rect.h / 2) - (text_height / 2),
                m_cursor_width,
                text_height
            ),
            dc.textForeground(style) // TODO should be a separate color setting
        );
    }
    dc.setClip(old_clip);
}

Size LineEdit::sizeHint(DrawingContext &dc) {
    if (m_text_changed) {
        m_virtual_size = dc.measureText(font(), text(), m_tab_width);
        m_text_height = m_virtual_size.h;
    }
    if (m_size_changed) {
        Size size = Size(m_min_length, font() ? font()->max_height : dc.default_font->max_height);
        dc.sizeHintMargin(size, style);
        dc.sizeHintBorder(size, style);
        dc.sizeHintPadding(size, style);

        m_size = size;
        m_size_changed = false;

        return size;
    } else {
        return m_size;
    }
}

std::string LineEdit::text() {
    return m_text;
}

LineEdit* LineEdit::setText(std::string text) {
    m_text = text;
    m_text_changed = true;
    update();
    onTextChanged.notify();

    return this;
}

void LineEdit::handleTextEvent(DrawingContext &dc, const char *text) {
    insert(m_selection.end, text);
}

int LineEdit::minLength() {
    return m_min_length;
}

LineEdit* LineEdit::setMinLength(int length) {
    if (m_min_length != length) {
        m_min_length = length;
        layout();
    }

    return this;
}

LineEdit* LineEdit::moveCursorLeft() {
    if (m_selection.end == 0) {
       if (m_selection.hasSelection() && !isShiftPressed()) {
            swapSelection();
            m_selection.x_end = m_selection.x_begin;
            m_selection.end = m_selection.begin;
        }
    } else if (m_selection.end) {
        if (m_selection.hasSelection() && !isShiftPressed()) {
            swapSelection();
            m_selection.x_end = m_selection.x_begin;
            m_selection.end = m_selection.begin;
            goto END;
        }
        DrawingContext &dc = *Application::get()->dc;
        m_selection.end--;
        int char_size = dc.measureText(font(), text()[m_selection.end]).w;
        m_selection.x_end -= char_size;
        if (!isShiftPressed()) {
            m_selection.x_begin = m_selection.x_end;
            m_selection.begin = m_selection.end;
        }
    }
    END:;
    if (!m_selection.end) {
        m_current_view = m_min_view;
    } else {
        m_current_view = m_selection.x_end / m_virtual_size.w;
    }
    update();

    return this;
}

LineEdit* LineEdit::moveCursorRight() {
    if (m_selection.end == text().size()) {
       if (m_selection.hasSelection() && !isShiftPressed()) {
            m_selection.x_begin = m_selection.x_end;
            m_selection.begin = m_selection.end;
        }
    } else if (m_selection.end < text().size()) {
        if (m_selection.hasSelection() && !isShiftPressed()) {
            swapSelection();
            m_selection.x_begin = m_selection.x_end;
            m_selection.begin = m_selection.end;
            goto END;
        }
        DrawingContext &dc = *Application::get()->dc;
        int char_size = dc.measureText(font(), text()[m_selection.end]).w;
        m_selection.x_end += char_size;
        m_selection.end++;
        if (!isShiftPressed()) {
            m_selection.x_begin = m_selection.x_end;
            m_selection.begin = m_selection.end;
        }
    }
    END:;
    if (m_selection.end == text().size()) {
        m_current_view = m_max_view;
    } else {
        m_current_view = m_selection.x_end / m_virtual_size.w;
    }
    update();

    return this;
}

LineEdit* LineEdit::moveCursorBegin() {
    m_selection.x_end = 0;
    m_selection.end = 0;
    if (!isShiftPressed()) {
        m_selection.x_begin = m_selection.x_end;
        m_selection.begin = m_selection.end;
    }
    m_current_view = m_min_view;
    update();

    return this;
}

LineEdit* LineEdit::moveCursorEnd() {
    m_selection.x_end = m_virtual_size.w;
    m_selection.end = text().size();
    if (!isShiftPressed()) {
        m_selection.x_begin = m_selection.x_end;
        m_selection.begin = m_selection.end;
    }
    m_current_view = m_max_view;
    update();

    return this;
}

LineEdit* LineEdit::deleteAt(size_t index, bool skip) {
    if (index < text().length()) {
        if (!skip) {
            m_history.append({HistoryItem::Action::Delete, std::string(1, m_text[index]), m_selection});
        }
        m_text.erase(index, 1);
        m_text_changed = true;
        update();
        onTextChanged.notify();
    }

    return this;
}

LineEdit* LineEdit::clear() {
    m_text.clear();
    m_text_changed = true;
    m_selection.x_begin = inner_rect.x;
    m_selection.begin = 0;
    m_selection.x_end = m_selection.x_begin;
    m_selection.end = m_selection.begin;
    m_current_view = m_min_view;
    m_history = History();
    update();
    onTextChanged.notify();

    return this;
}

LineEdit* LineEdit::setPlaceholderText(std::string text) {
    m_placeholder_text = text;
    update();

    return this;
}

std::string LineEdit::placeholderText() {
    return m_placeholder_text;
}

LineEdit* LineEdit::updateView() {
    if (!m_selection.end) {
        m_current_view = m_min_view;
    } else if (m_selection.end == text().length()) {
        m_current_view = m_max_view;
    } else {
        m_current_view = m_selection.x_end / m_virtual_size.w;
    }
    update();
    return this;
}

LineEdit* LineEdit::jumpWordLeft() {
    while (m_selection.end) {
        moveCursorLeft();
        if (text()[m_selection.end] == ' ') {
            break;
        }
    }
    update();
    return this;
}

LineEdit* LineEdit::jumpWordRight() {
    while (m_selection.end < text().size()) {
        moveCursorRight();
        if (text()[m_selection.end] == ' ') {
            break;
        }
    }
    update();
    return this;
}

bool LineEdit::isShiftPressed() {
    SDL_Keymod mod = SDL_GetModState();
    if (mod & Mod::Shift) {
        return true;
    }
    return false;
}

void LineEdit::deleteSelection(bool skip) {
    // Swap selection when the begin index is higher than the end index.
    swapSelection();
    if (!skip) {
        m_history.append({HistoryItem::Action::Delete, m_text.substr(m_selection.begin, m_selection.end - m_selection.begin), m_selection});
    }
    // Remove selected text.
    setText(text().erase(m_selection.begin, m_selection.end - m_selection.begin));

    // Reset selection after deletion.
    m_selection.x_end = m_selection.x_begin;
    m_selection.end = m_selection.begin;
    onTextChanged.notify();
}

void LineEdit::selectAll() {
    m_selection.x_begin = 0;
    m_selection.begin = 0;
    m_selection.x_end = m_virtual_size.w;
    m_selection.end = text().size();
    m_current_view = m_max_view;
    update();
}

void LineEdit::swapSelection() {
    if (m_selection.begin > m_selection.end) {
        int temp_x = m_selection.x_end;
        size_t temp = m_selection.end;
        m_selection.x_end = m_selection.x_begin;
        m_selection.end = m_selection.begin;
        m_selection.x_begin = temp_x;
        m_selection.begin = temp;
    }
}

void LineEdit::insert(size_t index, const char *text, bool skip) {
    DrawingContext &dc = *Application::get()->dc;

    if (m_selection.hasSelection()) {
        deleteSelection(skip);
    }

    if (!skip) {
        m_history.append({HistoryItem::Action::Insert, text, m_selection});
    }
    m_text.insert(m_selection.begin, text);
    m_text_changed = true;

    m_selection.x_end += dc.measureText(font(), text).w;
    m_selection.end += strlen(text);
    m_selection.x_begin = m_selection.x_end;
    m_selection.begin = m_selection.end;

    if (m_selection.end == this->text().size()) {
        m_current_view = m_max_view;
    } else {
        m_current_view = m_selection.x_end / m_virtual_size.w;
    }
    update();
    onTextChanged.notify();
}

void LineEdit::setCursor(size_t index) {
    if (!index) {
        m_selection.x_begin = 0;
        m_selection.begin = 0;
        m_selection.x_end = m_selection.x_begin;
        m_selection.end = m_selection.begin;
    } else {
        if (!(m_virtual_size.w < inner_rect.w)) {
            inner_rect.x -= m_current_view * (m_virtual_size.w - inner_rect.w);
        }
        DrawingContext &dc = *Application::get()->dc;
        size_t local_index = 0;
        for (char c : text()) {
            int w = dc.measureText(font(), c).w;
            inner_rect.x += w;
            local_index++;
            if (index == local_index) {
                break;
            }
        }
        m_selection.x_begin = inner_rect.x;
        m_selection.begin = local_index;
        m_selection.x_end = m_selection.x_begin;
        m_selection.end = m_selection.begin;
    }
    update();
}

void LineEdit::undo() {
    if (!m_history.undo_end) {
        HistoryItem item = m_history.get(m_history.index);
        if (item.action == HistoryItem::Action::Delete) {
            insert(m_selection.begin, item.text.c_str(), true);
            m_selection = item.selection;
        } else {
            m_selection = item.selection;
            if (m_selection.hasSelection()) {
                deleteSelection(true);
            } else if (item.text.length() > 1) {
                for (size_t i = 0; i < item.text.length(); i++) {
                    deleteAt(m_selection.begin, true);
                }
            } else {
                deleteAt(m_selection.begin, true);
            }
        }
        if (!m_history.index) {
            m_history.undo_end = true;
        } else {
            m_history.index--;
        }
        m_history.redo_end = false;
    }
}

void LineEdit::redo() {
    if (m_history.index < m_history.items.size() && !m_history.redo_end) {
        HistoryItem item = m_history.get(m_history.undo_end ? 0 : ++m_history.index);
        if (item.action == HistoryItem::Action::Delete) {
            m_selection = item.selection;
            if (m_selection.hasSelection()) {
                deleteSelection(true);
            } else if (item.text.length() > 1) {
                for (size_t i = 0; i < item.text.length(); i++) {
                    deleteAt(m_selection.begin, true);
                }
            } else {
                deleteAt(m_selection.begin, true);
            }
        } else {
            m_selection = item.selection;
            insert(m_selection.begin, item.text.c_str(), true);
        }
        if (!m_history.index) {
            m_history.undo_end = false;
        }
        if (m_history.index == m_history.items.size() - 1) {
            m_history.redo_end = true;
        }
    }
}
