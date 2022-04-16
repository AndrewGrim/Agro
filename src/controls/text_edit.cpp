#include "text_edit.hpp"
#include "../renderer/renderer.hpp"
#include "../application.hpp"

TextEdit::TextEdit(String text, String placeholder, Mode mode, Size min_size) : Scrollable(min_size) {
    m_mode = mode;
    setText(text);
    setPlaceholderText(placeholder);
    m_selection.x_begin = 0;
    m_selection.x_end = m_selection.x_begin;
    // onMouseDown.addEventListener([&](Widget *widget, MouseEvent event) {
    //     m_selection.mouse_selection = true;
    //     if (!this->text().size()) {
    //         m_selection.x_begin = 0;
    //     } else {
    //         DrawingContext &dc = *Application::get()->currentWindow()->dc;
    //         i32 x = 0;
    //         u64 index = 0;
    //         i32 offset_event = event.x - inner_rect.x;
    //         u8 length = 0;
    //         for (u64 i = 0; i < this->text().size(); i += length) {
    //             length = utf8SequenceLength(this->text().data() + i);
    //             i32 w = dc.measureText(font(), Slice<const char>(this->text().data() + i, length)).w;
    //             if (x + w > offset_event) {
    //                 if (x + (w / 2) < offset_event) {
    //                     x += w;
    //                     index++;
    //                 }
    //                 break;
    //             }
    //             x += w;
    //             index++;
    //         }
    //         m_selection.x_begin = x;
    //         m_selection.begin = index;
    //         m_selection.x_end = m_selection.x_begin;
    //         m_selection.end = m_selection.begin;
    //     }
    // });
    // onMouseMotion.addEventListener([&](Widget *widget, MouseEvent event) {
    //     if (isPressed()) {
    //         // No x mouse movement.
    //         if (event.xrel == 0) {
    //             return;
    //         }

    //         DrawingContext &dc = *Application::get()->currentWindow()->dc;
    //         i32 x = m_selection.x_begin;
    //         u64 index = m_selection.begin;
    //         i32 offset_event = event.x - inner_rect.x;

    //         // Selection is to the right of the origin point.
    //         if (offset_event >= x) {
    //             while (index < this->text().size()) {
    //                 u8 c = this->text().data()[index];
    //                 i32 w = dc.measureText(font(), c).w;
    //                 if (x + w > offset_event) {
    //                     break;
    //                 }
    //                 x += w;
    //                 index++;
    //             }
    //         // Selection is to the left of the origin point.
    //         } else {
    //             while (index) {
    //                 u8 c = this->text().data()[--index];
    //                 i32 w = dc.measureText(font(), c).w;
    //                 x -= w;
    //                 if (x < offset_event) {
    //                     break;
    //                 }
    //             }
    //         }

    //         if (!index) {
    //             m_current_view = m_min_view;
    //         } else if (index == this->text().size()) {
    //             m_current_view = m_max_view;
    //         } else {
    //             m_current_view = m_selection.x_end / m_virtual_size.w;
    //         }
    //         m_selection.x_end = x;
    //         m_selection.end = index;
    //         update();
    //     }
    // });
    // onMouseUp.addEventListener([&](Widget *widget, MouseEvent event) {
    //     m_selection.mouse_selection = false;
    // });
    // onMouseEntered.addEventListener([&](Widget *widget, MouseEvent event) {
    //     Application::get()->setMouseCursor(Cursor::IBeam);
    // });
    // onMouseLeft.addEventListener([&](Widget *widget, MouseEvent event) {
    //     Application::get()->setMouseCursor(Cursor::Default);
    //     m_selection.mouse_selection = false;
    // });
    // auto left = [&]{
    //     moveCursorLeft();
    // };
    // bind(SDLK_LEFT, Mod::None, left);
    // bind(SDLK_LEFT, Mod::Shift, left);
    // auto right = [&]{
    //     moveCursorRight();
    // };
    // bind(SDLK_RIGHT, Mod::None, right);
    // bind(SDLK_RIGHT, Mod::Shift, right);
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
    Rect focus_rect = rect;
    dc.fillRect(rect, dc.textBackground(style));
    dc.padding(rect, style);

    dc.setClip(rect.clipTo(previous_clip));
    Point pos;
    pos = automaticallyAddOrRemoveScrollBars(dc, rect, m_virtual_size);
    inner_rect = rect;

    if (m_buffer.size() && m_buffer[0].size()) {
        // TODO start drawing text based on scroll position and not from the beginning
        Rect region = inner_rect;
        for (const String &line : m_buffer) {
            dc.fillTextAligned(
                font(),
                line.slice(),
                HorizontalAlignment::Left,
                VerticalAlignment::Top,
                region,
                0,
                dc.textForeground(style),
                m_tab_width,
                isHardFocused() ? Renderer::Selection(m_selection.begin, m_selection.end) : Renderer::Selection(),
                dc.textSelected(style)
            );
            region.y += font() ? font()->maxHeight() : dc.default_font->maxHeight();
            region.y += m_line_spacing;
            if (region.y > rect.y + rect.h) { break; }
        }
    } else {
        // TODO start drawing text based on scroll position and not from the beginning
        Rect region = inner_rect;
        for (const String &line : m_placeholder_buffer) {
            dc.fillTextAligned(
                font(),
                line.slice(),
                HorizontalAlignment::Left,
                VerticalAlignment::Top,
                region,
                0,
                dc.textDisabled(style),
                m_tab_width
            );
            region.y += font() ? font()->maxHeight() : dc.default_font->maxHeight();
            region.y += m_line_spacing;
            if (region.y > rect.y + rect.h) { break; }
        }
    }

    drawScrollBars(dc, rect, m_virtual_size);

    // if (m_virtual_size.w > inner_rect.w) {
    //     inner_rect.x -= m_current_view * (m_virtual_size.w - inner_rect.w);
    // }
    // // Draw placeholder text.
    // if (!text().size()) {
    //     dc.fillTextAligned(
    //         font(),
    //         placeholderText().slice(),
    //         HorizontalAlignment::Left,
    //         VerticalAlignment::Center,
    //         inner_rect,
    //         0,
    //         dc.textDisabled(style)
    //     );
    // // Draw normal text;
    // } else {
    //     if (m_selection.mouse_selection || (isHardFocused() && m_selection.hasSelection())) {
    //         i32 text_height = (font() ? font()->maxHeight() : dc.default_font->maxHeight()) + TOP_PADDING(this) + BOTTOM_PADDING(this);
    //         i32 start = m_selection.x_begin < m_selection.x_end ? m_selection.x_begin : m_selection.x_end;
    //         i32 end = m_selection.x_begin < m_selection.x_end ? m_selection.x_end : m_selection.x_begin;
    //         dc.fillRect(
    //             Rect(
    //                 inner_rect.x + start,
    //                 inner_rect.y + (inner_rect.h / 2) - (text_height / 2),
    //                 (end - start) + m_cursor_width,
    //                 text_height
    //             ),
    //             dc.accentWidgetBackground(style)
    //         );
    //     }
    //     dc.fillTextAligned(
    //         font(),
    //         text().slice(),
    //         HorizontalAlignment::Left,
    //         VerticalAlignment::Center,
    //         inner_rect,
    //         0,
    //         dc.textForeground(style),
    //         m_tab_width,
    //         isHardFocused() ? Renderer::Selection(m_selection.begin, m_selection.end) : Renderer::Selection(),
    //         dc.textSelected(style)
    //     );
    // }

    // // Draw the text insertion cursor.
    // if (isHardFocused()) {
    //     i32 text_height = (font() ? font()->maxHeight() : dc.default_font->maxHeight()) + TOP_PADDING(this) + BOTTOM_PADDING(this);
    //     dc.fillRect(
    //         Rect(
    //             inner_rect.x + m_selection.x_end,
    //             inner_rect.y + (inner_rect.h / 2) - (text_height / 2),
    //             m_cursor_width,
    //             text_height
    //         ),
    //         dc.textForeground(style) // TODO should be a separate color setting
    //     );
    // }
    // dc.setClip(focus_rect); // No need to keep the last clip since we are done using it anyway.
    dc.drawKeyboardFocus(focus_rect, style, state);
    dc.setClip(previous_clip);
}

Size TextEdit::sizeHint(DrawingContext &dc) {
    if (m_text_changed) {
        m_virtual_size = Size();
        for (const String &line : m_buffer) {
            Size size = dc.measureText(font(), line.slice(), m_tab_width);
            size.h += m_line_spacing;
            m_virtual_size.h += size.h;
            if (size.w > m_virtual_size.w) { m_virtual_size.w = size.w; }
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
    u64 index = 0;
    u64 last_line_index = 0;
    if (m_mode == Mode::SingleLine) {
        m_buffer.push_back(String(text.data()));
    } else {
        for (char c : text) {
            if (c == '\n') {
                m_buffer.push_back(String(text.data() + last_line_index, index - last_line_index));
                last_line_index = index;
            }
            index++;
        }
        m_buffer.push_back(String(text.data() + last_line_index, index - last_line_index));
    }

    m_text_changed = true;
    update();
    onTextChanged.notify();

    return this;
}

void TextEdit::handleTextEvent(DrawingContext &dc, const char *text) {
    // insert(m_selection.end, text);
}

// TextEdit* TextEdit::moveCursorLeft() {
//     if (m_selection.end == 0) {
//        if (m_selection.hasSelection() && !isShiftPressed()) {
//             swapSelection();
//             m_selection.x_end = m_selection.x_begin;
//             m_selection.end = m_selection.begin;
//         }
//     } else if (m_selection.end) {
//         if (m_selection.hasSelection() && !isShiftPressed()) {
//             swapSelection();
//             m_selection.x_end = m_selection.x_begin;
//             m_selection.end = m_selection.begin;
//             goto END;
//         }
//         DrawingContext &dc = *Application::get()->currentWindow()->dc;
//         m_selection.end--;
//         i32 char_size = dc.measureText(font(), text().data()[m_selection.end]).w;
//         m_selection.x_end -= char_size;
//         if (!isShiftPressed()) {
//             m_selection.x_begin = m_selection.x_end;
//             m_selection.begin = m_selection.end;
//         }
//     }
//     END:;
//     if (!m_selection.end) {
//         m_current_view = m_min_view;
//     } else {
//         m_current_view = m_selection.x_end / m_virtual_size.w;
//     }
//     update();

//     return this;
// }

// TextEdit* TextEdit::moveCursorRight() {
//     if (m_selection.end == text().size()) {
//        if (m_selection.hasSelection() && !isShiftPressed()) {
//             m_selection.x_begin = m_selection.x_end;
//             m_selection.begin = m_selection.end;
//         }
//     } else if (m_selection.end < text().size()) {
//         if (m_selection.hasSelection() && !isShiftPressed()) {
//             swapSelection();
//             m_selection.x_begin = m_selection.x_end;
//             m_selection.begin = m_selection.end;
//             goto END;
//         }
//         DrawingContext &dc = *Application::get()->currentWindow()->dc;
//         i32 char_size = dc.measureText(font(), text().data()[m_selection.end]).w;
//         m_selection.x_end += char_size;
//         m_selection.end++;
//         if (!isShiftPressed()) {
//             m_selection.x_begin = m_selection.x_end;
//             m_selection.begin = m_selection.end;
//         }
//     }
//     END:;
//     if (m_selection.end == text().size()) {
//         m_current_view = m_max_view;
//     } else {
//         m_current_view = m_selection.x_end / m_virtual_size.w;
//     }
//     update();

//     return this;
// }

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

// void TextEdit::swapSelection() {
//     if (m_selection.begin > m_selection.end) {
//         i32 temp_x = m_selection.x_end;
//         u64 temp = m_selection.end;
//         m_selection.x_end = m_selection.x_begin;
//         m_selection.end = m_selection.begin;
//         m_selection.x_begin = temp_x;
//         m_selection.begin = temp;
//     }
// }

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