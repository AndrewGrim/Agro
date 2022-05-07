#include "text_edit.hpp"
#include "../renderer/renderer.hpp"
#include "../application.hpp"

#define NORMALIZE(min, max, value) (value < min ? min : value > max ? max : value)
#define TEXT_HEIGHT ((i32)((font() ? font()->maxHeight() : dc.default_font->maxHeight()) + m_line_spacing))
#define X_SCROLL_OFFSET ((m_horizontal_scrollbar->isVisible() ? m_horizontal_scrollbar->m_slider->m_value : 0.0) * (m_virtual_size.w - inner_rect.w))
#define Y_SCROLL_OFFSET ((m_vertical_scrollbar->isVisible() ? m_vertical_scrollbar->m_slider->m_value : 0.0) * (m_virtual_size.h - inner_rect.h))
#define DC (*(Application::get()->currentWindow()->dc))

TextEdit::TextEdit(String text, String placeholder, Mode mode, Size min_size) : Scrollable(min_size) {
    m_mode = mode;
    setText(text);
    setPlaceholderText(placeholder);
    onMouseDown.addEventListener([&](Widget *widget, MouseEvent event) {
        m_selection.mouse_selection = true;
        if (!(m_buffer.size() == 1 && !m_buffer[0].size())) {
            DrawingContext &dc = *Application::get()->currentWindow()->dc;
            u64 index = 0;
            i32 x = inner_rect.x;
            x -= X_SCROLL_OFFSET;
            i32 y = inner_rect.y;
            y -= Y_SCROLL_OFFSET;
            u64 line = (event.y - y) / TEXT_HEIGHT;
            m_last_codepoint_index = 0;
            if (line < m_buffer.size()) {
                utf8::Iterator iter = m_buffer[line].utf8Begin();
                while ((iter = iter.next())) {
                    i32 w = dc.measureText(font(), Slice<const char>(iter.data - iter.length, iter.length), m_tab_width).w;
                    if (x + w > event.x) {
                        if (x + (w / 2) < event.x) {
                            x += w;
                            index += iter.length;
                            m_last_codepoint_index++;
                        }
                        break;
                    }
                    x += w;
                    index += iter.length;
                    m_last_codepoint_index++;
                }
            }
            if (line >= m_buffer.size()) { line = m_buffer.size() - 1; } // This accounts for clicking outside text on y axis.

            if (isShiftPressed()) {
                if (line < m_selection.line_begin && line < m_selection.line_end) {
                    if (
                        (m_selection.line_begin < m_selection.line_end) ||
                        ((m_selection.line_begin == m_selection.line_end) && (m_selection.begin < m_selection.end))
                    ) {
                        _endSelection();
                    }
                } else if (line > m_selection.line_begin && line > m_selection.line_end) {
                    if (
                        (m_selection.line_begin > m_selection.line_end) ||
                        ((m_selection.line_begin == m_selection.line_end) && (m_selection.begin > m_selection.end))
                    ) {
                        _endSelection();
                    }
                }
                m_selection.x_end = x + X_SCROLL_OFFSET;
                m_selection.line_end = line;
                m_selection.end = index;
            } else {
                m_selection.x_end = x + X_SCROLL_OFFSET;
                m_selection.line_end = line;
                m_selection.end = index;
                _endSelection();
            }
        }
    });
    onMouseMotion.addEventListener([&](Widget *widget, MouseEvent event) {
        if (isPressed()) {
            DrawingContext &dc = *Application::get()->currentWindow()->dc;
            i32 x = inner_rect.x;
            x -= X_SCROLL_OFFSET;
            i32 y = inner_rect.y;
            y -= Y_SCROLL_OFFSET;
            u64 index = 0;
            i32 line = NORMALIZE(0, (i32)m_buffer.size() - 1, (event.y - y) / TEXT_HEIGHT);

            // TODO we could optimise it for single line by doing what we did previously
            // and check if lines are the same
            m_last_codepoint_index = 0;
            utf8::Iterator iter = utf8::Iterator(m_buffer[line].data());
            while ((iter = iter.next())) {
                i32 w = dc.measureText(font(), Slice<const char>(iter.data - iter.length, iter.length), m_tab_width).w;
                if (x + w > event.x) {
                    break;
                }
                x += w;
                index += iter.length;
                m_last_codepoint_index++;
            }

            m_selection.x_end = x + X_SCROLL_OFFSET;
            m_selection.line_end = line;
            m_selection.end = index;
            {
                auto _mouseScrollCallback = [&](u32 interval) -> u32 {
                    SDL_Event event = {};
                    event.type = SDL_MOUSEMOTION;
                    event.motion = m_mouse_scroll_event;
                    SDL_PushEvent(&event);
                    return 16;
                };

                m_mouse_scroll_event.timestamp = SDL_GetTicks();
                m_mouse_scroll_event.windowID = SDL_GetWindowID(Application::get()->currentWindow()->m_win);
                m_mouse_scroll_event.state = SDL_GetMouseState(&m_mouse_scroll_event.x, &m_mouse_scroll_event.y);
                bool can_remove_callback = true;

                if (event.x < inner_rect.x && m_horizontal_scrollbar->m_slider->m_value > 0.0) {
                    m_mouse_scroll_event.xrel = event.x - inner_rect.x;
                    if (!m_mouse_scroll_callback) {
                        m_mouse_scroll_callback = Application::get()->addTimer(0, _mouseScrollCallback);
                    }
                    if (event.xrel < 0) {
                        m_horizontal_scrollbar->m_slider->m_value += event.xrel / (f64)(m_virtual_size.w - inner_rect.w);
                        m_horizontal_scrollbar->m_slider->m_value = NORMALIZE(0.0, 1.0, m_horizontal_scrollbar->m_slider->m_value);
                    }
                    can_remove_callback = false;
                } else if (event.x > inner_rect.x + inner_rect.w && m_horizontal_scrollbar->m_slider->m_value < 1.0) {
                    m_mouse_scroll_event.xrel = event.x - (inner_rect.x + inner_rect.w);
                    if (!m_mouse_scroll_callback) {
                        m_mouse_scroll_callback = Application::get()->addTimer(0, _mouseScrollCallback);
                    }
                    if (event.xrel > 0) {
                        m_horizontal_scrollbar->m_slider->m_value += event.xrel / (f64)(m_virtual_size.w - inner_rect.w);
                        m_horizontal_scrollbar->m_slider->m_value = NORMALIZE(0.0, 1.0, m_horizontal_scrollbar->m_slider->m_value);
                    }
                    can_remove_callback = false;
                }

                if (m_mode == Mode::MultiLine) {
                    if (event.y < inner_rect.y && m_vertical_scrollbar->m_slider->m_value > 0.0) {
                        m_mouse_scroll_event.yrel = event.y - inner_rect.y;
                        if (!m_mouse_scroll_callback) {
                            m_mouse_scroll_callback = Application::get()->addTimer(0, _mouseScrollCallback);
                        }
                        if (event.yrel < 0) {
                            m_vertical_scrollbar->m_slider->m_value += event.yrel / (f64)(m_virtual_size.h - inner_rect.h);
                            m_vertical_scrollbar->m_slider->m_value = NORMALIZE(0.0, 1.0, m_vertical_scrollbar->m_slider->m_value);
                        }
                        can_remove_callback = false;
                    } else if (event.y > inner_rect.y + inner_rect.h && m_vertical_scrollbar->m_slider->m_value < 1.0) {
                        m_mouse_scroll_event.yrel = event.y - (inner_rect.y + inner_rect.h);
                        if (!m_mouse_scroll_callback) {
                            m_mouse_scroll_callback = Application::get()->addTimer(0, _mouseScrollCallback);
                        }
                        if (event.yrel > 0) {
                            m_vertical_scrollbar->m_slider->m_value += event.yrel / (f64)(m_virtual_size.h - inner_rect.h);
                            m_vertical_scrollbar->m_slider->m_value = NORMALIZE(0.0, 1.0, m_vertical_scrollbar->m_slider->m_value);
                        }
                        can_remove_callback = false;
                    }
                }

                if (can_remove_callback && m_mouse_scroll_callback) {
                    Application::get()->removeTimer(m_mouse_scroll_callback.value);
                    m_mouse_scroll_callback = Option<Timer>();
                }
            }
            update();
        }
    });
    onMouseUp.addEventListener([&](Widget *widget, MouseEvent event) {
        m_selection.mouse_selection = false;
        if (m_mouse_scroll_callback) {
            Application::get()->removeTimer(m_mouse_scroll_callback.value);
            m_mouse_scroll_callback = Option<Timer>();
        }
    });
    onMouseEntered.addEventListener([&](Widget *widget, MouseEvent event) {
        Application::get()->setMouseCursor(Cursor::IBeam);
    });
    onMouseLeft.addEventListener([&](Widget *widget, MouseEvent event) {
        Application::get()->setMouseCursor(Cursor::Default);
        m_selection.mouse_selection = false; // TODO maybe dont do that
        if (m_mouse_scroll_callback) {
            Application::get()->removeTimer(m_mouse_scroll_callback.value);
            m_mouse_scroll_callback = Option<Timer>();
        }
    });
    onFocusLost.addEventListener([&](Widget *widget, FocusEvent event) {
        if (m_mouse_scroll_callback) {
            Application::get()->removeTimer(m_mouse_scroll_callback.value);
            m_mouse_scroll_callback = Option<Timer>();
        }
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
        deleteSelection(true);
    });
    bind(SDLK_DELETE, Mod::None, [&]{
        deleteSelection();
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
                insert(s);
                SDL_free(s);
            }
        }
    });
    bind(SDLK_c, Mod::Ctrl, [&]{
        SDL_SetClipboardText(selection().data());
    });
    // bind(SDLK_z, Mod::Ctrl, [&]{
    //     undo();
    // });
    // bind(SDLK_y, Mod::Ctrl, [&]{
    //     redo();
    // });
    bind(SDLK_TAB, Mod::None, [&]() {
        insert("\t");
    });
}

TextEdit::~TextEdit() {
    if (m_mouse_scroll_callback) {
        Application::get()->removeTimer(m_mouse_scroll_callback.value);
        m_mouse_scroll_callback = Option<Timer>();
    }
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

    Rect text_region = Rect(pos.x, pos.y, inner_rect.w, inner_rect.h);
    // Draw normal text;
    u64 line_index = -((pos.y - inner_rect.y) / TEXT_HEIGHT);
    text_region.y += TEXT_HEIGHT * line_index;
    i32 x_scroll_offset = (pos.x - inner_rect.x);
    if (!(m_buffer.size() == 1 && !m_buffer[0].size())) {
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
            dc.fillRect(
                Rect(
                    x_scroll_offset + bg_start,
                    text_region.y,
                    (bg_end - bg_start) + m_cursor_width,
                    TEXT_HEIGHT
                ),
                dc.accentWidgetBackground(style)
            );

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
                selection,
                dc.textSelected(style)
            );
            text_region.y += TEXT_HEIGHT;
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
            text_region.y += TEXT_HEIGHT;
            if (text_region.y > rect.y + rect.h) { break; }
        }
    }

    // Draw the text insertion cursor.
    if (state & STATE_HARD_FOCUSED) {
        i32 y = inner_rect.y;
        y -= Y_SCROLL_OFFSET;
        dc.fillRect(
            Rect(
                x_scroll_offset + m_selection.x_end,
                y + (TEXT_HEIGHT * m_selection.line_end),
                m_cursor_width,
                TEXT_HEIGHT
            ),
            dc.textForeground(style) // TODO should be a separate color setting
        );
    }

    if (m_mode == Mode::MultiLine) { drawScrollBars(dc, rect, m_virtual_size); }
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
        m_virtual_size.w += m_cursor_width;
        if (m_buffer.size() == 1) { m_virtual_size.h -= m_line_spacing; }
        m_text_changed = false;
    }
    if (m_size_changed) {
        Scrollable::sizeHint(dc);
        Size size;
        if (m_mode == Mode::MultiLine) { size = m_viewport; }
        else { size = Size(m_viewport.w, TEXT_HEIGHT); }
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
    u64 length = 0;
    for (u64 i = m_selection.line_begin; i <= m_selection.line_end; i++) {
        const String &line = m_buffer[i];
        if (i == m_selection.line_begin) {
            length += line.size() - m_selection.begin + 1; // +1 to account for newline
        } else if (i == m_selection.line_end) {
            length += m_selection.end;
        } else {
            length += line.size() + 1; // +1 to account for newline
        }
    }
    String s = String(length);
    u64 offset = 0;
    for (u64 i = m_selection.line_begin; i <= m_selection.line_end; i++) {
        const String &line = m_buffer[i];
        if (i == m_selection.line_begin) {
            memcpy(s.data() + offset, line.data() + m_selection.begin, line.size() - m_selection.begin);
            offset += line.size() - m_selection.begin + 1; // +1 to account for newline
            s.data()[offset - 1] = '\n';
        } else if (i == m_selection.line_end) {
            memcpy(s.data() + offset, line.data(), m_selection.end);
            offset += m_selection.end;
        } else {
            memcpy(s.data() + offset, line.data(), line.size());
            offset += line.size() + 1; // +1 to account for newline
            s.data()[offset - 1] = '\n';
        }
    }
    s.data()[s.size()] = '\0';
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
            // TODO ideally we would account for \r as well
            // and ideally we would strip both out of singleline buffer
            if (c == '\n') {
                m_buffer.push_back(String(text.data() + last_line_index, index - last_line_index));
                m_buffer_length.push_back(0);
                last_line_index = index + 1;
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
    insert(text);
}

void TextEdit::_beginSelection() {
    m_selection.end = m_selection.begin;
    m_selection.x_end = m_selection.x_begin;
    m_selection.line_end = m_selection.line_begin;
}

void TextEdit::_endSelection() {
    m_selection.x_begin = m_selection.x_end;
    m_selection.line_begin = m_selection.line_end;
    m_selection.begin = m_selection.end;
}

void TextEdit::_updateView(DrawingContext &dc) {
    u64 viewport_start_x = X_SCROLL_OFFSET + inner_rect.x;
    u64 viewport_start_y = Y_SCROLL_OFFSET;
    u64 next_x_pos = m_selection.x_end;
    u64 next_y_pos = m_selection.line_end * TEXT_HEIGHT;

    if (next_x_pos < viewport_start_x) {
        m_horizontal_scrollbar->m_slider->m_value = (next_x_pos - inner_rect.x) / (f64)(m_virtual_size.w - inner_rect.w);
        m_horizontal_scrollbar->m_slider->m_value = NORMALIZE(0.0, 1.0, m_horizontal_scrollbar->m_slider->m_value);
    } else if (next_x_pos > viewport_start_x + inner_rect.w) {
        m_horizontal_scrollbar->m_slider->m_value = (next_x_pos - inner_rect.w) / (f64)(m_virtual_size.w - inner_rect.w);
        m_horizontal_scrollbar->m_slider->m_value = NORMALIZE(0.0, 1.0, m_horizontal_scrollbar->m_slider->m_value);
    }

    if (m_mode == Mode::MultiLine) {
        if (next_y_pos < viewport_start_y) {
            m_vertical_scrollbar->m_slider->m_value = next_y_pos / (f64)(m_virtual_size.h - inner_rect.h);
            m_vertical_scrollbar->m_slider->m_value = NORMALIZE(0.0, 1.0, m_vertical_scrollbar->m_slider->m_value);
        } else if (next_y_pos + TEXT_HEIGHT > viewport_start_y + inner_rect.h) {
            m_vertical_scrollbar->m_slider->m_value = (next_y_pos + TEXT_HEIGHT - inner_rect.h) / (f64)(m_virtual_size.h - inner_rect.h);
            m_vertical_scrollbar->m_slider->m_value = NORMALIZE(0.0, 1.0, m_vertical_scrollbar->m_slider->m_value);
        }
    }
}

void TextEdit::_moveLeft(DrawingContext &dc) {
    // beginning of the line
    if (m_selection.end == 0) {
        // move to line above if one exists
        if (m_selection.line_end) {
            m_selection.line_end -= 1;
            m_selection.end = m_buffer[m_selection.line_end].size();
            m_selection.x_end = inner_rect.x + m_buffer_length[m_selection.line_end];
            utf8::Iterator iter = m_buffer[m_selection.line_end].utf8Begin();
            m_last_codepoint_index = 0;
            for (; ((iter = iter.next())); m_last_codepoint_index++);
        } // otherwise do nothing, end of text
    // decrement by one
    } else {
        utf8::Iterator iter = utf8::Iterator(m_buffer[m_selection.line_end].data(), m_selection.end).prev();
        i32 w = dc.measureText(font(), Slice<const char>(iter.data, iter.length), m_tab_width).w;
        m_selection.end -= iter.length;
        m_selection.x_end -= w;
        iter = m_buffer[m_selection.line_end].utf8Begin();
        m_last_codepoint_index = 0;
        for (u64 codepoint_byte_offset = 0; codepoint_byte_offset != m_selection.end ; iter = iter.next(), codepoint_byte_offset += iter.length, m_last_codepoint_index++);
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
            swapSelection();
            _beginSelection();
        } else {
            // move cursor by one
            _moveLeft(dc);
            _endSelection();
        }
    }

    _updateView(DC);
    update();

    return this;
}

void TextEdit::_moveRight(DrawingContext &dc) {
    // end of the line
    if (m_selection.end == m_buffer[m_selection.line_end].size()) {
        // move to line below if one exists
        if (m_selection.line_end + 1 < m_buffer.size()) {
            m_selection.line_end += 1;
            m_selection.end = 0;
            m_selection.x_end = inner_rect.x;
            m_last_codepoint_index = 0;
        } // otherwise do nothing, end of text
    // increment by one
    } else {
        utf8::Iterator iter = utf8::Iterator(m_buffer[m_selection.line_end].data(), m_selection.end).next();
        i32 w = dc.measureText(font(), Slice<const char>(iter.data - iter.length, iter.length), m_tab_width).w;
        m_selection.end += iter.length;
        m_selection.x_end += w;
        iter = m_buffer[m_selection.line_end].utf8Begin();
        m_last_codepoint_index = 0;
        for (u64 codepoint_byte_offset = 0; codepoint_byte_offset != m_selection.end ; iter = iter.next(), codepoint_byte_offset += iter.length, m_last_codepoint_index++);
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
            swapSelection();
            _endSelection();
        } else {
            // move cursor by one
            _moveRight(dc);
            _endSelection();
        }
    }

    _updateView(DC);
    update();

    return this;
}

void TextEdit::_moveUp(DrawingContext &dc) {
    if (m_selection.line_end) {
        u64 next_line = m_selection.line_end - 1;

        if (!m_buffer_length[next_line]) {
            m_selection.end = 0;
            m_selection.x_end = inner_rect.x;
            m_selection.line_end = next_line;
            return;
        }

        utf8::Iterator next_line_iter = m_buffer[next_line].utf8Begin();
        u64 next_codepoint_index = 0;
        i32 next_x_end = inner_rect.x;
        u64 next_end = 0;
        while ((next_line_iter = next_line_iter.next())) {
            if (next_codepoint_index == m_last_codepoint_index) { break; }
            next_codepoint_index++;
            next_x_end += dc.measureText(font(), Slice<const char>(next_line_iter.data - next_line_iter.length, next_line_iter.length), m_tab_width).w;
            next_end += next_line_iter.length;
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
    DrawingContext &dc = *Application::get()->currentWindow()->dc;

    if (isShiftPressed()) {
        // extend selection by one
        _moveUp(dc);
    } else {
        if (m_selection.hasSelection()) {
            // go to whichever selection edge is closer to the top
            if (
                (m_selection.line_begin < m_selection.line_end) ||
                ((m_selection.line_begin == m_selection.line_end) && (m_selection.begin < m_selection.end))
            ) {
                m_selection.line_end = m_selection.line_begin;
                m_selection.x_end = m_selection.x_begin;
                m_selection.end = m_selection.begin;
                utf8::Iterator iter = m_buffer[m_selection.line_end].utf8Begin();
                m_last_codepoint_index = 0;
                for (u64 codepoint_byte_offset = 0; codepoint_byte_offset != m_selection.end ; iter = iter.next(), codepoint_byte_offset += iter.length, m_last_codepoint_index++);
            }
            _moveUp(dc);
            _endSelection();
        } else {
            // move cursor by one
            _moveUp(dc);
            _endSelection();
        }
    }

    _updateView(dc);
    update();
    return this;
}

void TextEdit::_moveDown(DrawingContext &dc) {
    if (m_selection.line_end < m_buffer.size() - 1) {
        u64 next_line = m_selection.line_end + 1;

        if (!m_buffer_length[next_line]) {
            m_selection.end = 0;
            m_selection.x_end = inner_rect.x;
            m_selection.line_end = next_line;
            return;
        }

        utf8::Iterator next_line_iter = m_buffer[next_line].utf8Begin();
        u64 next_codepoint_index = 0;
        i32 next_x_end = inner_rect.x;
        u64 next_end = 0;
        while ((next_line_iter = next_line_iter.next())) {
            if (next_codepoint_index == m_last_codepoint_index) { break; }
            next_codepoint_index++;
            next_x_end += dc.measureText(font(), Slice<const char>(next_line_iter.data - next_line_iter.length, next_line_iter.length), m_tab_width).w;
            next_end += next_line_iter.length;
        }

        m_selection.x_end = next_x_end;
        m_selection.line_end = next_line;
        m_selection.end = next_end;
    } else { // End of text, set cursor to the end
        m_selection.x_end = inner_rect.x + m_buffer_length[m_selection.line_end];
        m_selection.end = m_buffer[m_selection.line_end].size();
    }
}

TextEdit* TextEdit::moveCursorDown() {
    DrawingContext &dc = *Application::get()->currentWindow()->dc;

    if (isShiftPressed()) {
        // extend selection by one
        _moveDown(dc);
    } else {
        if (m_selection.hasSelection()) {
            // go to whichever selection edge is closer to the bottom
            if (
                (m_selection.line_begin > m_selection.line_end) ||
                ((m_selection.line_begin == m_selection.line_end) && (m_selection.begin > m_selection.end))
            ) {
                m_selection.line_end = m_selection.line_begin;
                m_selection.x_end = m_selection.x_begin;
                m_selection.end = m_selection.begin;
                utf8::Iterator iter = m_buffer[m_selection.line_end].utf8Begin();
                m_last_codepoint_index = 0;
                for (u64 codepoint_byte_offset = 0; codepoint_byte_offset != m_selection.end ; iter = iter.next(), codepoint_byte_offset += iter.length, m_last_codepoint_index++);
            }
            _moveDown(dc);
            _endSelection();
        } else {
            // move cursor by one
            _moveDown(dc);
            _endSelection();
        }
    }

    _updateView(dc);
    update();
    return this;
}

TextEdit* TextEdit::moveCursorBegin() {
    m_selection.x_end = inner_rect.x;
    m_selection.end = 0;

    if (!isShiftPressed()) {
        m_selection.x_begin = m_selection.x_end;
        m_selection.line_begin = m_selection.line_end;
        m_selection.begin = m_selection.end;
    }

    m_last_codepoint_index = 0;

    _updateView(DC);
    update();

    return this;
}

TextEdit* TextEdit::moveCursorEnd() {
    m_selection.x_end = inner_rect.x + m_buffer_length[m_selection.line_end];
    m_selection.end = m_buffer[m_selection.line_end].size();

    if (!isShiftPressed()) {
        m_selection.x_begin = m_selection.x_end;
        m_selection.line_begin = m_selection.line_end;
        m_selection.begin = m_selection.end;
    }

    m_last_codepoint_index = 0;
    utf8::Iterator iter = m_buffer[m_selection.line_end].utf8Begin();
    for (; ((iter = iter.next())); m_last_codepoint_index++);

    _updateView(DC);
    update();

    return this;
}

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

TextEdit* TextEdit::jumpWordLeft() {
    if (!m_selection.end) {
        return moveCursorLeft();
    }
    do {
        moveCursorLeft();
        if (m_buffer[m_selection.line_end].data()[m_selection.end] == ' ') {
            break;
        }
    } while (m_selection.end);
    return this;
}

TextEdit* TextEdit::jumpWordRight() {
    if (m_selection.end == m_buffer[m_selection.line_end].size()) {
        return moveCursorRight();
    }
    do {
        moveCursorRight();
        if (m_buffer[m_selection.line_end].data()[m_selection.end] == ' ') {
            break;
        }
    } while (m_selection.end < m_buffer[m_selection.line_end].size());
    return this;
}

bool TextEdit::isShiftPressed() {
    SDL_Keymod mod = SDL_GetModState();
    if (mod & Mod::Shift) {
        return true;
    }
    return false;
}

void TextEdit::_updateVirtualWidth() {
    m_virtual_size.w = 0;
    for (u64 length : m_buffer_length) {
        if ((i32)length + m_cursor_width > m_virtual_size.w) { m_virtual_size.w = length + m_cursor_width; }
    }
}

void TextEdit::deleteSelection(bool is_backspace, bool skip) {
    DrawingContext &dc = DC;

    if (m_selection.hasSelection()) {
        if (m_selection.line_begin != m_selection.line_end) {
            // Multiline selection
            swapSelection(); // TODO We will need to make sure thats fine in terms of history
            u64 lines_to_delete = m_selection.line_end - m_selection.line_begin;

            String &first_line = m_buffer[m_selection.line_begin];
            u64 &first_line_length = m_buffer_length[m_selection.line_begin];
            i32 first_text_size = dc.measureText(font(), Slice<const char>(first_line.data() + m_selection.begin, first_line.size() - m_selection.begin), m_tab_width).w;
            first_line_length -= first_text_size;
            first_line.erase(m_selection.begin, first_line.size() - m_selection.begin);

            String &last_line = m_buffer[m_selection.line_end];
            i32 last_text_size = dc.measureText(font(), Slice<const char>(last_line.data() + m_selection.end, last_line.size() - m_selection.end), m_tab_width).w;
            if (last_text_size) {
                first_line += last_line.substring(m_selection.end, last_line.size()).data();
                first_line_length += last_text_size;
            }

            if (lines_to_delete == 1) {
                m_buffer.erase(m_buffer.begin() + m_selection.line_end);
                m_buffer_length.erase(m_buffer_length.begin() + m_selection.line_end);
            } else if (lines_to_delete > 1) {
                m_buffer.erase(m_buffer.begin() + m_selection.line_begin + 1, m_buffer.begin() + m_selection.line_end + 1);
                m_buffer_length.erase(m_buffer_length.begin() + m_selection.line_begin + 1, m_buffer_length.begin() + m_selection.line_end + 1);
            }
            m_virtual_size.h -= TEXT_HEIGHT * lines_to_delete;
            if ((i32)first_line_length + m_cursor_width > m_virtual_size.w) { m_virtual_size.w = first_line_length + m_cursor_width; }
            else { _updateVirtualWidth(); }
        } else {
            // Same line selection
            swapSelection(); // TODO We will need to make sure thats fine in terms of history
            String &line = m_buffer[m_selection.line_end];
            u64 &line_length = m_buffer_length[m_selection.line_end];
            Size text_size = dc.measureText(font(), Slice<const char>(line.data() + m_selection.begin, m_selection.end - m_selection.begin), m_tab_width);
            line_length -= text_size.w;
            if ((i32)line_length + text_size.w + m_cursor_width == m_virtual_size.w) { _updateVirtualWidth(); }
            line.erase(m_selection.begin, m_selection.end - m_selection.begin);
        }
        _beginSelection();
    // Delete one codepoint
    } else {
        String &line = m_buffer[m_selection.line_end];
        u64 &line_length = m_buffer_length[m_selection.line_end];
        if (is_backspace) {
            if (m_selection.end) {
                utf8::Iterator iter = utf8::Iterator(line.data(), m_selection.end).prev();
                assert(iter && "There should be a valid codepoint here since we already checked that we are not at the beginning of the line!");
                Size text_size = dc.measureText(font(), Slice<const char>(iter.data, iter.length), m_tab_width);
                line_length -= text_size.w;
                if ((i32)line_length + text_size.w + m_cursor_width == m_virtual_size.w) { _updateVirtualWidth(); }
                line.erase(m_selection.end - iter.length, iter.length);
                m_selection.end -= iter.length;
                m_selection.x_end -= text_size.w;
                m_last_codepoint_index--;
                _endSelection();
            // Delete newline between this and the previous line if one exists
            } else {
                if (m_selection.line_end) {
                    String &previous_line = m_buffer[m_selection.line_end - 1];
                    u64 &previous_line_length = m_buffer_length[m_selection.line_end - 1];
                    m_selection.end = previous_line.size();
                    m_selection.x_end = previous_line_length + inner_rect.x;
                    utf8::Iterator iter = previous_line.utf8Begin();
                    m_last_codepoint_index = 0;
                    for (; ((iter = iter.next())); m_last_codepoint_index++);
                    m_selection.line_end--;
                    _endSelection();
                    previous_line += line.data();
                    previous_line_length += line_length;
                    if ((i32)previous_line_length + m_cursor_width > m_virtual_size.w) { m_virtual_size.w = previous_line_length + m_cursor_width; }
                    m_buffer.erase(m_buffer.begin() + m_selection.line_end + 1);
                    m_buffer_length.erase(m_buffer_length.begin() + m_selection.line_end + 1);
                    m_virtual_size.h -= TEXT_HEIGHT;
                } else {
                    return;
                }
            }
        } else {
            if (m_selection.end < line.size()) {
                u64 codepoint_length = utf8::length(line.data() + m_selection.end);
                Size text_size = dc.measureText(font(), Slice<const char>(line.data() + m_selection.end, codepoint_length), m_tab_width);
                line_length -= text_size.w;
                if ((i32)line_length + text_size.w + m_cursor_width == m_virtual_size.w) { _updateVirtualWidth(); }
                line.erase(m_selection.end, codepoint_length);
            // Delete newline between this and the next line if one exists
            } else {
                if (m_selection.line_end + 1 < m_buffer.size()) {
                    line += m_buffer[m_selection.line_end + 1].data();
                    line_length += m_buffer_length[m_selection.line_end + 1];
                    if ((i32)line_length + m_cursor_width > m_virtual_size.w) { m_virtual_size.w = line_length + m_cursor_width; }
                    m_buffer.erase(m_buffer.begin() + m_selection.line_end + 1);
                    m_buffer_length.erase(m_buffer_length.begin() + m_selection.line_end + 1);
                    m_virtual_size.h -= TEXT_HEIGHT;
                } else {
                    return;
                }
            }
        }
    }

    // TODO eventually record the deletion in history

    _updateView(dc);
    update();
    onTextChanged.notify();
}

void TextEdit::selectAll() {
    m_selection.x_begin = inner_rect.x;
    m_selection.line_begin = 0;
    m_selection.begin = 0;
    m_selection.x_end = inner_rect.x + m_buffer_length[m_buffer.size() - 1];
    m_selection.line_end = m_buffer.size() - 1;
    m_selection.end = m_buffer[m_buffer.size() - 1].size();
    update();
}

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
    } else if (m_selection.line_begin == m_selection.line_end && m_selection.begin > m_selection.end) {
        i32 temp_x = m_selection.x_end;
        u64 temp = m_selection.end;
        m_selection.x_end = m_selection.x_begin;
        m_selection.end = m_selection.begin;
        m_selection.x_begin = temp_x;
        m_selection.begin = temp;
        return true;
    }
    return false;
}

void TextEdit::insert(const char *text, bool skip) {
    DrawingContext &dc = DC;

    if (m_selection.hasSelection()) { deleteSelection(false, skip); }

    utf8::Iterator iter = utf8::Iterator(text);
    u64 length = 0;
    i32 text_size = 0;
    bool is_first_line = true;
    u64 last_line_index = 0;
    while ((iter = iter.next())) {
        // TODO we should get rid of newlines for singleline mode also \r
        if (iter.codepoint == '\n' && m_mode == Mode::MultiLine) {
            if (is_first_line) {
                u64 &line_length = m_buffer_length[m_selection.line_end];
                m_buffer[m_selection.line_end].insert(m_selection.end, text, length);
                line_length += text_size;
                if (line_length + m_cursor_width > m_virtual_size.w) { m_virtual_size.w = line_length + m_cursor_width; }
                is_first_line = false;
            } else {
                m_buffer.emplace(m_buffer.begin() + m_selection.line_end, text + last_line_index, length);
                m_buffer_length.insert(m_buffer_length.begin() + m_selection.line_end, text_size);
                if (text_size + m_cursor_width > m_virtual_size.w) { m_virtual_size.w = text_size + m_cursor_width; }
            }
            m_selection.line_end++;
            m_virtual_size.h += TEXT_HEIGHT;
            last_line_index += length + 1; // +1 to step over the newline.
            m_selection.x_end = inner_rect.x;
            m_selection.end = 0;
            text_size = 0;
            length = 0;
            m_last_codepoint_index = 0;
        } else {
            i32 codepoint_width = dc.measureText(font(), Slice<const char>(iter.data - iter.length, iter.length), m_tab_width).w;
            text_size += codepoint_width;
            length += iter.length;
            m_last_codepoint_index++;
        }
    }
    // If the text being inserted spans multiple lines then insert another line into the buffer for the last text range
    if (!is_first_line) {
        m_buffer.emplace(m_buffer.begin() + m_selection.line_end, text + last_line_index, length);
        m_buffer_length.insert(m_buffer_length.begin() + m_selection.line_end, text_size);
        if (text_size + m_cursor_width > m_virtual_size.w) { m_virtual_size.w = text_size + m_cursor_width; }
        m_selection.x_end += text_size;
        m_selection.end += length;
        m_virtual_size.h += TEXT_HEIGHT;
    // otherwise add the text to the current line from selection
    } else {
        u64 &line_length = m_buffer_length[m_selection.line_end];
        m_buffer[m_selection.line_end].insert(m_selection.end, text + last_line_index, length);
        line_length += text_size;
        m_selection.x_end += text_size;
        m_selection.end += length;
        if (line_length + m_cursor_width > m_virtual_size.w) { m_virtual_size.w = line_length + m_cursor_width; }
    }
    _endSelection();

    _updateView(dc);
    update();
    onTextChanged.notify();
}

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
//             i32 w = dc.measureText(font(), Slice<const char>(text().data() + i, length), m_tab_width).w;
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

Widget* TextEdit::propagateMouseEvent(Window *window, State *state, MouseEvent event) {
    if (m_mode == Mode::MultiLine) {
        if (m_vertical_scrollbar->isVisible()) {
            if ((event.x >= m_vertical_scrollbar->rect.x && event.x <= m_vertical_scrollbar->rect.x + m_vertical_scrollbar->rect.w) &&
                (event.y >= m_vertical_scrollbar->rect.y && event.y <= m_vertical_scrollbar->rect.y + m_vertical_scrollbar->rect.h)) {
                return m_vertical_scrollbar->propagateMouseEvent(window, state, event);
            }
        }
        if (m_horizontal_scrollbar->isVisible()) {
            if ((event.x >= m_horizontal_scrollbar->rect.x && event.x <= m_horizontal_scrollbar->rect.x + m_horizontal_scrollbar->rect.w) &&
                (event.y >= m_horizontal_scrollbar->rect.y && event.y <= m_horizontal_scrollbar->rect.y + m_horizontal_scrollbar->rect.h)) {
                return m_horizontal_scrollbar->propagateMouseEvent(window, state, event);
            }
        }
        if (m_vertical_scrollbar->isVisible() && m_horizontal_scrollbar->isVisible()) {
            if ((event.x > m_horizontal_scrollbar->rect.x + m_horizontal_scrollbar->rect.w) &&
                (event.y > m_vertical_scrollbar->rect.y + m_vertical_scrollbar->rect.h)) {
                if (state->hovered) {
                    state->hovered->onMouseLeft.notify(this, event);
                }
                state->hovered = nullptr;
                update();
                return nullptr;
            }
        }
    }

    for (Widget *child : children) {
        if (child->isVisible()) {
            if ((event.x >= child->rect.x && event.x <= child->rect.x + child->rect.w) &&
                (event.y >= child->rect.y && event.y <= child->rect.y + child->rect.h)) {
                Widget *last = nullptr;
                if (child->isLayout()) {
                    last = child->propagateMouseEvent(window, state, event);
                } else {
                    child->handleMouseEvent(window, state, event);
                    last = child;
                }
                return last;
            }
        }
    }

    handleMouseEvent(window, state, event);
    return this;
}

bool TextEdit::handleScrollEvent(ScrollEvent event) {
    SDL_Keymod mod = SDL_GetModState();
    if (mod & Mod::Shift) {
        if (m_horizontal_scrollbar->isVisible()) {
            return m_horizontal_scrollbar->m_slider->handleScrollEvent(event);
        }
    } else {
        if (m_mode == Mode::MultiLine && m_vertical_scrollbar->isVisible()) {
            return m_vertical_scrollbar->m_slider->handleScrollEvent(event);
        }
    }
    return false;
}

String TextEdit::selection() {
    swapSelection();
    if (m_selection.line_begin == m_selection.line_end) {
        return m_buffer[m_selection.line_end].substring(m_selection.begin, m_selection.end);
    } else {
        u64 length = 0;
        for (u64 i = m_selection.line_begin; i <= m_selection.line_end; i++) {
            const String &line = m_buffer[i];
            if (i == m_selection.line_begin) {
                length += line.size() - m_selection.begin + 1; // +1 to account for newline
            } else if (i == m_selection.line_end) {
                length += m_selection.end;
            } else {
                length += line.size() + 1; // +1 to account for newline
            }
        }
        String s = String(length);
        u64 offset = 0;
        for (u64 i = m_selection.line_begin; i <= m_selection.line_end; i++) {
            const String &line = m_buffer[i];
            if (i == m_selection.line_begin) {
                memcpy(s.data() + offset, line.data() + m_selection.begin, line.size() - m_selection.begin);
                offset += line.size() - m_selection.begin + 1; // +1 to account for newline
                s.data()[offset - 1] = '\n';
            } else if (i == m_selection.line_end) {
                memcpy(s.data() + offset, line.data(), m_selection.end);
                offset += m_selection.end;
            } else {
                memcpy(s.data() + offset, line.data(), line.size());
                offset += line.size() + 1; // +1 to account for newline
                s.data()[offset - 1] = '\n';
            }
        }
        s.data()[s.size()] = '\0';
        return s;
    }
}
