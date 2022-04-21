#include "text_edit.hpp"
#include "../renderer/renderer.hpp"
#include "../application.hpp"

TextEdit::TextEdit(String text, String placeholder, Mode mode, Size min_size) : Scrollable(min_size) {
    m_mode = mode;
    setText(text);
    setPlaceholderText(placeholder);
    onMouseDown.addEventListener([&](Widget *widget, MouseEvent event) {
        m_selection.mouse_selection = true;
        if (m_buffer.size() && m_buffer[0].size()) {
            DrawingContext &dc = *Application::get()->currentWindow()->dc;
            u64 index = 0;
            i32 x = inner_rect.x;
            x -= (m_horizontal_scrollbar->isVisible() ? m_horizontal_scrollbar->m_slider->m_value : 0.0) * (m_virtual_size.w - inner_rect.w);
            i32 y = inner_rect.y;
            y -= (m_vertical_scrollbar->isVisible() ? m_vertical_scrollbar->m_slider->m_value : 0.0) * (m_virtual_size.h - inner_rect.h);
            i32 text_height = font() ? font()->maxHeight() : dc.default_font->maxHeight();
            u64 line = (event.y - y) / (text_height + m_line_spacing);
            if (line < m_buffer.size()) {
                utf8::Iterator iter = m_buffer[line].utf8Begin();
                while ((iter = iter.next())) {
                    i32 w = dc.measureText(font(), Slice<const char>(iter.data - iter.length, iter.length)).w;
                    if (x + w > event.x) {
                        if (x + (w / 2) < event.x) {
                            x += w;
                            index += iter.length;
                        }
                        break;
                    }
                    x += w;
                    index += iter.length;
                }
            }
            if (line >= m_buffer.size()) { line = m_buffer.size() - 1; } // This accounts for clicking outside text on y axis.
            m_selection.x_begin = x;
            m_selection.line_begin = line;
            m_selection.begin = index;
            m_selection.x_end = m_selection.x_begin;
            m_selection.line_end = m_selection.line_begin;
            m_selection.end = m_selection.begin;
        }
    });
    onMouseMotion.addEventListener([&](Widget *widget, MouseEvent event) {
        if (isPressed()) {
            // TODO figure out how we want to handle selecting newlines
            // because atm i believe they affect things to a certain extent
            // and in terms of buffer do we want to store them at the beginning of next line
            // or at the end of previous one?
            // and in singleline mode we need to skip over them as well
            DrawingContext &dc = *Application::get()->currentWindow()->dc;
            i32 x = inner_rect.x;
            x -= (m_horizontal_scrollbar->isVisible() ? m_horizontal_scrollbar->m_slider->m_value : 0.0) * (m_virtual_size.w - inner_rect.w);
            i32 y = inner_rect.y;
            y -= (m_vertical_scrollbar->isVisible() ? m_vertical_scrollbar->m_slider->m_value : 0.0) * (m_virtual_size.h - inner_rect.h);
            u64 index = 0;
            i32 text_height = font() ? font()->maxHeight() : dc.default_font->maxHeight();
            i32 line = (event.y - y) / (text_height + m_line_spacing);

            if (line < 0) {
                line = 0;
            } else if (line < m_buffer.size()) {
                // TODO we could optimise it for single line by doing what we did previously
                // and check if lines are the same
                utf8::Iterator iter = utf8::Iterator(m_buffer[line].data());
                while ((iter = iter.next())) {
                    i32 w = dc.measureText(font(), Slice<const char>(iter.data - iter.length, iter.length)).w;
                    if (x + w > event.x) {
                        break;
                    }
                    x += w;
                    index += iter.length;
                }
            } else {
                line = m_buffer.size() - 1;
            }

            // TODO tackle this last but essentially
            // we need to update the viewport to show us the contents
            // after the mouse movement, so maybe move slightly to the right lets say
            // or event fully scroll to the end or something
            // if (!index) {
            //     m_current_view = m_min_view;
            // } else if (index == this->text().size()) {
            //     m_current_view = m_max_view;
            // } else {
            //     m_current_view = m_selection.x_end / m_virtual_size.w;
            // }
            m_selection.x_end = x;
            m_selection.line_end = line;
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
        m_selection.mouse_selection = false; // TODO maybe dont do that
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
    auto up = [&]{
        moveCursorUp();
    };
    bind(SDLK_UP, Mod::None, up);
    bind(SDLK_UP, Mod::Shift, up);
    auto down = [&]{
        moveCursorDown();
    };
    bind(SDLK_DOWN, Mod::None, down);
    bind(SDLK_DOWN, Mod::Shift, down);
    // auto home = [&]{
    //     moveCursorBegin();
    // };
    // bind(SDLK_HOME, Mod::None, home);
    // bind(SDLK_HOME, Mod::Shift, home);
    // auto end = [&]{
    //     moveCursorEnd();
    // };
    // bind(SDLK_END, Mod::None, end);
    // bind(SDLK_END, Mod::Shift, end);
    // bind(SDLK_BACKSPACE, Mod::None, [&]{
    //     if (m_selection.hasSelection()) {
    //         deleteSelection();
    //     } else if (m_selection.begin) {
    //         moveCursorLeft();
    //         deleteAt(m_selection.begin);
    //     }
    //     updateView();
    // });
    // bind(SDLK_DELETE, Mod::None, [&]{
    //     if (m_selection.hasSelection()) {
    //         deleteSelection();
    //     } else {
    //         deleteAt(m_selection.begin);
    //     }
    //     updateView();
    // });
    // auto jump_left = [&]{
    //     jumpWordLeft();
    // };
    // bind(SDLK_LEFT, Mod::Ctrl, jump_left);
    // bind(SDLK_LEFT, Mod::Ctrl|Mod::Shift, jump_left);
    // auto jump_right = [&]{
    //     jumpWordRight();
    // };
    // bind(SDLK_RIGHT, Mod::Ctrl, jump_right);
    // bind(SDLK_RIGHT, Mod::Ctrl|Mod::Shift, jump_right);
    // bind(SDLK_a, Mod::Ctrl, [&]{
    //     selectAll();
    // });
    // bind(SDLK_v, Mod::Ctrl, [&]{
    //     if (SDL_HasClipboardText()) {
    //         char *s = SDL_GetClipboardText();
    //         if (s) {
    //             insert(m_selection.end, s);
    //             SDL_free(s);
    //         }
    //     }
    // });
    // bind(SDLK_c, Mod::Ctrl, [&]{
    //     if (m_selection.hasSelection()) {
    //         swapSelection();
    //         String s = this->text().substring(m_selection.begin, m_selection.end);
    //         SDL_SetClipboardText(s.data());
    //     }
    // });
    // bind(SDLK_z, Mod::Ctrl, [&]{
    //     undo();
    // });
    // bind(SDLK_y, Mod::Ctrl, [&]{
    //     redo();
    // });
    // bind(SDLK_TAB, Mod::None, [&]() {
    //     const char *tab = "\t";
    //     insert(m_selection.end, tab);
    // });
}

TextEdit::~TextEdit() {

}

const char* TextEdit::name() {
    return "TextEdit";
}

void TextEdit::draw(DrawingContext &dc, Rect rect, i32 state) {
    dc.margin(rect, style);
    this->rect = rect;
    Rect previous_clip = dc.clip();
    dc.drawBorder(rect, style, state);
    dc.setClip(rect.clipTo(previous_clip));
    Rect focus_rect = rect;
    dc.fillRect(rect, dc.textBackground(style));
    dc.padding(rect, style);

    Point pos = automaticallyAddOrRemoveScrollBars(dc, rect, m_virtual_size);
    inner_rect = rect;

    i32 text_height = font() ? font()->maxHeight() : dc.default_font->maxHeight();
    Rect text_region = Rect(pos.x, pos.y, inner_rect.w, inner_rect.h);
    // Draw normal text;
    u64 line_index = -((pos.y - inner_rect.y) / (text_height + m_line_spacing));
    text_region.y += (text_height + m_line_spacing) * line_index;
    if (m_buffer.size() && m_buffer[0].size()) {
        Selection before_swap = m_selection;
        bool did_swap = swapSelection();
        Renderer::Selection selection;
        for (; line_index < m_buffer.size(); line_index++) {
            const String &line = m_buffer[line_index];
            // Determine the selection to pass in to the renderer and dimensions to use for selection background.
            i32 bg_start = 0;
            i32 bg_end = 0;
            if (line_index >= m_selection.line_begin && line_index <= m_selection.line_end) {
                if (line_index == m_selection.line_begin) {
                    if (m_selection.line_begin == m_selection.line_end) {
                        selection = Renderer::Selection(m_selection.begin, m_selection.end);
                        bg_start = m_selection.x_begin;
                        bg_end = m_selection.x_end;
                    } else {
                        selection = Renderer::Selection(m_selection.begin, line.size());
                        bg_start = m_selection.x_begin;
                        bg_end = inner_rect.x + m_buffer_length[line_index];
                    }
                } else if (line_index == m_selection.line_end) {
                    if (m_selection.line_begin == m_selection.line_end) {
                        selection = Renderer::Selection(m_selection.begin, m_selection.end);
                        bg_start = m_selection.x_begin;
                        bg_end = m_selection.x_end;
                    } else {
                        selection = Renderer::Selection(0, m_selection.end);
                        bg_start = inner_rect.x;
                        bg_end = m_selection.x_end;
                    }
                } else {
                    selection = Renderer::Selection(0, line.size());
                    bg_start = inner_rect.x;
                    bg_end = inner_rect.x + m_buffer_length[line_index];
                }
            } else { selection = Renderer::Selection(); }

            // Draw selection background
            if (m_selection.mouse_selection || (state & STATE_HARD_FOCUSED && selection.begin != selection.end)) {
                dc.fillRect(
                    Rect(
                        bg_start,
                        text_region.y - (m_line_spacing / 2),
                        (bg_end - bg_start) + m_cursor_width,
                        text_height + m_line_spacing
                    ),
                    dc.accentWidgetBackground(style)
                );
            }

            // Draw the text buffer.
            dc.fillTextAligned(
                font(),
                line.slice(),
                HorizontalAlignment::Left,
                VerticalAlignment::Top,
                text_region,
                0,
                dc.textForeground(style),
                m_tab_width,
                state & STATE_HARD_FOCUSED ? selection : Renderer::Selection(),
                dc.textSelected(style)
            );
            text_region.y += text_height + m_line_spacing;
            if (text_region.y > rect.y + rect.h) { break; }
        }
        if (did_swap) { m_selection = before_swap; }
    // Draw placeholder text.
    } else {
        for (; line_index < m_placeholder_buffer.size(); line_index++) {
            const String &line = m_placeholder_buffer[line_index];
            dc.fillTextAligned(
                font(),
                line.slice(),
                HorizontalAlignment::Left,
                VerticalAlignment::Top,
                text_region,
                0,
                dc.textDisabled(style),
                m_tab_width
            );
            text_region.y += text_height + m_line_spacing;
            if (text_region.y > rect.y + rect.h) { break; }
        }
    }


    // Draw the text insertion cursor.
    if (state & STATE_HARD_FOCUSED) {
        i32 y = inner_rect.y;
        y -= (m_vertical_scrollbar->isVisible() ? m_vertical_scrollbar->m_slider->m_value : 0.0) * (m_virtual_size.h - inner_rect.h);
        dc.fillRect(
            Rect(
                m_selection.x_end,
                y  - (m_line_spacing / 2) + ((text_height + m_line_spacing) * m_selection.line_end),
                m_cursor_width,
                text_height + m_line_spacing
            ),
            dc.textForeground(style) // TODO should be a separate color setting
        );
    }

    drawScrollBars(dc, rect, m_virtual_size);
    dc.setClip(focus_rect); // No need to keep the last clip since we are done using it anyway.
    dc.drawKeyboardFocus(focus_rect, style, state);
    dc.setClip(previous_clip);
}

Size TextEdit::sizeHint(DrawingContext &dc) {
    if (m_text_changed) {
        m_virtual_size = Size();
        u64 index = 0;
        for (const String &line : m_buffer) {
            Size size = dc.measureText(font(), line.slice(), m_tab_width);
            m_buffer_length[index] = size.w;
            size.h += m_line_spacing;
            m_virtual_size.h += size.h;
            if (size.w > m_virtual_size.w) { m_virtual_size.w = size.w; }
            index++;
        }
        if (m_buffer.size() == 1) { m_virtual_size.h -= m_line_spacing; }
    }
    if (m_size_changed) {
        Scrollable::sizeHint(dc);
        Size size;
        if (m_mode == Mode::MultiLine) { size = m_viewport; }
        else { size = Size(m_viewport.w, font() ? font()->maxHeight() : dc.default_font->maxHeight()); }
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

String TextEdit::text() {
    String s = "";
    for (const String &line : m_buffer) {
        s += line.data();
    }
    return s;
}

TextEdit* TextEdit::setText(String text) {
    m_buffer.clear();
    m_buffer_length.clear();
    u64 index = 0;
    u64 last_line_index = 0;
    if (m_mode == Mode::SingleLine) {
        m_buffer.push_back(String(text.data()));
        m_buffer_length.push_back(0);
    } else {
        for (char c : text) {
            if (c == '\n') {
                m_buffer.push_back(String(text.data() + last_line_index, index - last_line_index));
                m_buffer_length.push_back(0);
                last_line_index = index;
            }
            index++;
        }
        m_buffer.push_back(String(text.data() + last_line_index, index - last_line_index));
        m_buffer_length.push_back(0);
    }

    m_text_changed = true;
    update();
    onTextChanged.notify();

    return this;
}

void TextEdit::handleTextEvent(DrawingContext &dc, const char *text) {
    // insert(m_selection.end, text);
}

void TextEdit::_noSelection() {
    m_selection.x_begin = m_selection.x_end;
    m_selection.line_begin = m_selection.line_end;
    m_selection.begin = m_selection.end;
}

void TextEdit::_moveLeft(DrawingContext &dc) {
    // beginning of the line
    if (m_selection.end == 0) {
        // move to line above if one exists
        if (m_selection.line_end) {
            m_selection.line_end -= 1;
            m_selection.end = m_buffer[m_selection.line_end].size();
            m_selection.x_end = inner_rect.x + m_buffer_length[m_selection.line_end];
        } // otherwise do nothing, end of text
    // decrement by one
    } else {
        utf8::Iterator iter = utf8::Iterator(m_buffer[m_selection.line_end].data(), m_selection.end).prev();
        i32 w = dc.measureText(font(), Slice<const char>(iter.data, iter.length)).w;
        m_selection.end -= iter.length;
        m_selection.x_end -= w;
    }
}

TextEdit* TextEdit::moveCursorLeft() {
    DrawingContext &dc = *Application::get()->currentWindow()->dc;
    if (isShiftPressed()) {
        // extend selection by one
        _moveLeft(dc);
    } else {
        if (m_selection.hasSelection()) {
            // go to the edge of the selection
            _noSelection();
        } else {
            // move cursor by one
            _moveLeft(dc);
            _noSelection();
        }
    }

    // TODO at some point update the view to account for the movement
    // if (!m_selection.end) {
    //     m_current_view = m_min_view;
    // } else {
    //     m_current_view = m_selection.x_end / m_virtual_size.w;
    // }
    update();

    return this;
}

void TextEdit::_moveRight(DrawingContext &dc) {
    // end of the line
    if (m_selection.end == m_buffer[m_selection.line_end].size()) {
        // move to line below if one exists
        if (m_selection.line_end < m_buffer.size()) {
            m_selection.line_end += 1;
            m_selection.end = 0;
            m_selection.x_end = inner_rect.x;
        } // otherwise do nothing, end of text
    // increment by one
    } else {
        utf8::Iterator iter = utf8::Iterator(m_buffer[m_selection.line_end].data(), m_selection.end).next();
        i32 w = dc.measureText(font(), Slice<const char>(iter.data - iter.length, iter.length)).w;
        m_selection.end += iter.length;
        m_selection.x_end += w;
    }
}

TextEdit* TextEdit::moveCursorRight() {
    DrawingContext &dc = *Application::get()->currentWindow()->dc;
    if (isShiftPressed()) {
        // extend selection by one
        _moveRight(dc);
    } else {
        if (m_selection.hasSelection()) {
            // go to the edge of the selection
            _noSelection();
        } else {
            // move cursor by one
            _moveRight(dc);
            _noSelection();
        }
    }

    // TODO at some point update the view to account for the movement
    // if (!m_selection.end) {
    //     m_current_view = m_min_view;
    // } else {
    //     m_current_view = m_selection.x_end / m_virtual_size.w;
    // }
    update();

    return this;
}

TextEdit* TextEdit::_moveUp(DrawingContext &dc) {
    if (m_selection.line_end) {
        u64 next_line = m_selection.line_end - 1;

        if (!m_buffer_length[next_line]) {
            m_selection.end = 0;
            m_selection.x_end = inner_rect.x;
            m_selection.line_end = next_line;
            update();
            return this;
        }

        utf8::Iterator current_line_iter = utf8::Iterator(m_buffer[m_selection.line_end].data(), m_selection.end);
        u64 current_codepoint_index = 0;
        for (; (current_line_iter = current_line_iter.prev()); current_codepoint_index++);

        utf8::Iterator next_line_iter = m_buffer[next_line].utf8Begin();
        u64 next_codepoint_index = 0;
        i32 next_x_end = inner_rect.x;
        u64 next_end = 0;
        while ((next_line_iter = next_line_iter.next())) {
            next_codepoint_index++;
            next_x_end += dc.measureText(font(), Slice<const char>(next_line_iter.data - next_line_iter.length, next_line_iter.length)).w;
            next_end += next_line_iter.length;
            if (next_codepoint_index == current_codepoint_index) { break; }
        }

        m_selection.x_end = next_x_end;
        m_selection.line_end = next_line;
        m_selection.end = next_end;
    } else { // End of text, set cursor to beginning
        m_selection.x_end = inner_rect.x;
        m_selection.end = 0;
    }
}

TextEdit* TextEdit::moveCursorUp() {
    // TODO this is close to ready
    // but we need to change left and right cursor movement and mousedown handler
    // to set m_last_codepoint_index to whatever it happens to be for that line and index
    // then we use that here to keep the position when going between the lines
    DrawingContext &dc = *Application::get()->currentWindow()->dc;

    if (isShiftPressed()) {
        // extend selection by one
        _moveUp(dc);
    } else {
        _moveUp(dc);
        _noSelection();
    }

    update();
    return this;
}

TextEdit* TextEdit::_moveDown(DrawingContext &dc) {

}

TextEdit* TextEdit::moveCursorDown() {

}

// TextEdit* TextEdit::moveCursorBegin() {
//     m_selection.x_end = 0;
//     m_selection.end = 0;
//     if (!isShiftPressed()) {
//         m_selection.x_begin = m_selection.x_end;
//         m_selection.begin = m_selection.end;
//     }
//     m_current_view = m_min_view;
//     update();

//     return this;
// }

// TextEdit* TextEdit::moveCursorEnd() {
//     m_selection.x_end = m_virtual_size.w;
//     m_selection.end = text().size();
//     if (!isShiftPressed()) {
//         m_selection.x_begin = m_selection.x_end;
//         m_selection.begin = m_selection.end;
//     }
//     m_current_view = m_max_view;
//     update();

//     return this;
// }

// TextEdit* TextEdit::deleteAt(u64 index, bool skip) {
//     if (index < text().size()) {
//         if (!skip) {
//             m_history.append(HistoryItem(HistoryItem::Action::Delete, String(m_text.data() + index, 1), m_selection));
//         }
//         m_text.erase(index, 1);
//         m_text_changed = true;
//         update();
//         onTextChanged.notify();
//     }

//     return this;
// }

TextEdit* TextEdit::clear() {
    m_buffer.clear();
    m_buffer_length.clear();
    m_text_changed = true;
    m_selection.x_begin = inner_rect.x;
    m_selection.begin = 0;
    m_selection.x_end = m_selection.x_begin;
    m_selection.end = m_selection.begin;
    m_history = History();
    update();
    onTextChanged.notify();

    return this;
}

TextEdit* TextEdit::setPlaceholderText(String text) {
    m_placeholder_buffer.clear();
    u64 index = 0;
    u64 last_line_index = 0;
    if (m_mode == Mode::SingleLine) {
        m_placeholder_buffer.push_back(String(text.data()));
    } else {
        for (char c : text) {
            if (c == '\n') {
                m_placeholder_buffer.push_back(String(text.data() + last_line_index, index - last_line_index));
                last_line_index = index;
            }
            index++;
        }
        m_placeholder_buffer.push_back(String(text.data() + last_line_index, index - last_line_index));
    }

    return this;
}

String TextEdit::placeholderText() {
    String s = "";
    for (const String &line : m_placeholder_buffer) {
        s += line.data();
    }
    return s;
}

// TextEdit* TextEdit::updateView() {
//     if (!m_selection.end) {
//         m_current_view = m_min_view;
//     } else if (m_selection.end == text().size()) {
//         m_current_view = m_max_view;
//     } else {
//         m_current_view = m_selection.x_end / m_virtual_size.w;
//     }
//     update();
//     return this;
// }

// TextEdit* TextEdit::jumpWordLeft() {
//     while (m_selection.end) {
//         moveCursorLeft();
//         if (text().data()[m_selection.end] == ' ') {
//             break;
//         }
//     }
//     update();
//     return this;
// }

// TextEdit* TextEdit::jumpWordRight() {
//     while (m_selection.end < text().size()) {
//         moveCursorRight();
//         if (text().data()[m_selection.end] == ' ') {
//             break;
//         }
//     }
//     update();
//     return this;
// }

bool TextEdit::isShiftPressed() {
    SDL_Keymod mod = SDL_GetModState();
    if (mod & Mod::Shift) {
        return true;
    }
    return false;
}

// void TextEdit::deleteSelection(bool skip) {
//     // Swap selection when the begin index is higher than the end index.
//     swapSelection();
//     if (!skip) {
//         m_history.append(HistoryItem(HistoryItem::Action::Delete, m_text.substring(m_selection.begin, m_selection.end - m_selection.begin), m_selection));
//     }
//     // Remove selected text.
//     m_text.erase(m_selection.begin, m_selection.end - m_selection.begin);
//     setText(text());

//     // Reset selection after deletion.
//     m_selection.x_end = m_selection.x_begin;
//     m_selection.end = m_selection.begin;
//     onTextChanged.notify();
// }

// void TextEdit::selectAll() {
//     m_selection.x_begin = 0;
//     m_selection.begin = 0;
//     m_selection.x_end = m_virtual_size.w;
//     m_selection.end = text().size();
//     m_current_view = m_max_view;
//     update();
// }

bool TextEdit::swapSelection() {
    if (m_selection.line_begin > m_selection.line_end) {
        i32 temp_x = m_selection.x_end;
        u64 temp_line = m_selection.line_end;
        u64 temp = m_selection.end;
        m_selection.x_end = m_selection.x_begin;
        m_selection.line_end = m_selection.line_begin;
        m_selection.end = m_selection.begin;
        m_selection.x_begin = temp_x;
        m_selection.line_begin = temp_line;
        m_selection.begin = temp;
        return true;
    }
    return false;
}

// void TextEdit::insert(u64 index, const char *text, bool skip) {
//     DrawingContext &dc = *Application::get()->currentWindow()->dc;

//     if (m_selection.hasSelection()) {
//         deleteSelection(skip);
//     }

//     if (!skip) {
//         m_history.append(HistoryItem(HistoryItem::Action::Insert, String(text), m_selection));
//     }
//     m_text.insert(m_selection.begin, text);
//     m_text_changed = true;

//     m_selection.x_end += dc.measureText(font(), text).w;
//     m_selection.end += strlen(text);
//     m_selection.x_begin = m_selection.x_end;
//     m_selection.begin = m_selection.end;

//     if (m_selection.end == this->text().size()) {
//         m_current_view = m_max_view;
//     } else {
//         m_current_view = m_selection.x_end / m_virtual_size.w;
//     }
//     update();
//     onTextChanged.notify();
// }

// void TextEdit::setCursor(u64 index) {
//     if (!index) {
//         m_selection.x_begin = 0;
//         m_selection.begin = 0;
//         m_selection.x_end = m_selection.x_begin;
//         m_selection.end = m_selection.begin;
//     } else {
//         if (!(m_virtual_size.w < inner_rect.w)) {
//             inner_rect.x -= m_current_view * (m_virtual_size.w - inner_rect.w);
//         }
//         DrawingContext &dc = *Application::get()->currentWindow()->dc;
//         u64 local_index = 0;
//         u8 length = 0;
//         for (u64 i = 0; i < text().size(); i += length) {
//             length = utf8SequenceLength(text().data() + i);
//             i32 w = dc.measureText(font(), Slice<const char>(text().data() + i, length)).w;
//             inner_rect.x += w;
//             local_index++;
//             if (index == local_index) {
//                 break;
//             }
//         }
//         m_selection.x_begin = inner_rect.x;
//         m_selection.begin = local_index;
//         m_selection.x_end = m_selection.x_begin;
//         m_selection.end = m_selection.begin;
//     }
//     update();
// }

// void TextEdit::undo() {
//     if (!m_history.undo_end) {
//         HistoryItem item = m_history.get(m_history.index);
//         if (item.action == HistoryItem::Action::Delete) {
//             insert(m_selection.begin, item.text.data(), true);
//             m_selection = item.selection;
//         } else {
//             m_selection = item.selection;
//             if (m_selection.hasSelection()) {
//                 deleteSelection(true);
//             } else if (item.text.size() > 1) {
//                 for (u64 i = 0; i < item.text.size(); i++) {
//                     deleteAt(m_selection.begin, true);
//                 }
//             } else {
//                 deleteAt(m_selection.begin, true);
//             }
//         }
//         if (!m_history.index) {
//             m_history.undo_end = true;
//         } else {
//             m_history.index--;
//         }
//         m_history.redo_end = false;
//     }
// }

// void TextEdit::redo() {
//     if (m_history.index < m_history.items.size() && !m_history.redo_end) {
//         HistoryItem item = m_history.get(m_history.undo_end ? 0 : ++m_history.index);
//         if (item.action == HistoryItem::Action::Delete) {
//             m_selection = item.selection;
//             if (m_selection.hasSelection()) {
//                 deleteSelection(true);
//             } else if (item.text.size() > 1) {
//                 for (u64 i = 0; i < item.text.size(); i++) {
//                     deleteAt(m_selection.begin, true);
//                 }
//             } else {
//                 deleteAt(m_selection.begin, true);
//             }
//         } else {
//             m_selection = item.selection;
//             insert(m_selection.begin, item.text.data(), true);
//         }
//         if (!m_history.index) {
//             m_history.undo_end = false;
//         }
//         if (m_history.index == m_history.items.size() - 1) {
//             m_history.redo_end = true;
//         }
//     }
// }

i32 TextEdit::isFocusable() {
    return (i32)FocusType::Focusable;
}
