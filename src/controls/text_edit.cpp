#include "text_edit.hpp"
#include "../renderer/renderer.hpp"
#include "../application.hpp"

#define NORMALIZE(min, max, value) (value < min ? min : value > max ? max : value)
#define TEXT_HEIGHT ((i32)((font()->maxHeight()) + m_line_spacing))
#define X_SCROLL_OFFSET ((m_horizontal_scrollbar->isVisible() ? m_horizontal_scrollbar->m_slider->m_value : 0.0) * (m_virtual_size.w - inner_rect.w))
#define Y_SCROLL_OFFSET ((m_vertical_scrollbar->isVisible() ? m_vertical_scrollbar->m_slider->m_value : 0.0) * (m_virtual_size.h - inner_rect.h))
#define DC (*(Application::get()->currentWindow()->dc))

TextEdit::TextEdit(String text, String placeholder, Mode mode, Size min_size) : Scrollable(min_size) {
    m_mode = mode;
    {
        // A little hack that allows the widget to correctly
        // reason about its view and dimensions before being laid out.
        // This prevents wrong viewport position for setText() or setCursor()
        // calls that were made before the application was ready.
        // This also allows us to call Scrollable::sizeHint() only here in order to initialise the scrollbars.
        DrawingContext &dc = DC;
        Scrollable::sizeHint(dc);
        Size s = sizeHint(dc);
        rect = Rect(0, 0, s.w, s.h);
        dc.sizeHintPadding(s, style());
        inner_rect = Rect(rect.w - s.w, rect.h - s.h, s.w, s.h);
    }
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
            u64 line = NORMALIZE(0, (i32)m_buffer.size() - 1, (event.y - y) / TEXT_HEIGHT);
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
                m_selection.x_end = x + X_SCROLL_OFFSET - inner_rect.x;
                m_selection.line_end = line;
                m_selection.end = index;
            } else {
                m_selection.x_end = x + X_SCROLL_OFFSET - inner_rect.x;
                m_selection.line_end = line;
                m_selection.end = index;
                _endSelection();
            }
            _updateView(dc);
        } else {
            m_selection.x_end = 0;
            m_selection.line_end = 0;
            m_selection.end = 0;
            _endSelection();
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
            u64 line = NORMALIZE(0, (i32)m_buffer.size() - 1, (event.y - y) / TEXT_HEIGHT);

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

            m_selection.x_end = x + X_SCROLL_OFFSET - inner_rect.x;
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
        if (!deleteSelection()) { deleteOne(true); }
    });
    bind(SDLK_DELETE, Mod::None, [&]{
        if (!deleteSelection()) { deleteOne(); }
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
    bind(SDLK_z, Mod::Ctrl, [&]{
        undo();
    });
    bind(SDLK_y, Mod::Ctrl, [&]{
        redo();
    });
    bind(SDLK_TAB, Mod::None, [&]() {
        insert("\t");
    });
    bind(SDLK_RETURN, Mod::None, [&]() {
        insert("\n");
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
    dc.margin(rect, style());
    this->rect = rect;
    Rect previous_clip = dc.clip();
    dc.drawBorder(rect, style(), state);
    dc.setClip(rect.clipTo(previous_clip));
    Rect focus_rect = rect;
    dc.fillRect(rect, dc.textBackground(style()));
    dc.padding(rect, style());

    Point pos = automaticallyAddOrRemoveScrollBars(dc, rect, m_virtual_size);
    Rect post_padding = rect;
    inner_rect = rect;

    Rect text_region = Rect(pos.x, pos.y, inner_rect.w, inner_rect.h);
    // Draw normal text;
    u64 line_index = -((pos.y - inner_rect.y) / TEXT_HEIGHT);
    text_region.y += TEXT_HEIGHT * line_index;
    i32 x_scroll_offset = (pos.x);
    if (!(m_buffer.size() == 1 && !m_buffer[0].size())) {
        Selection before_swap = m_selection;
        bool did_swap = swapSelection();
        Renderer::Selection selection;
        // TODO font scaling issue here where sometimes the math makes it such
        // that line_index falls outside of m_buffer and text does not render
        // update: the below workaround does work but i would like it to be more "precise"
        if (line_index >= m_buffer.size()) { line_index = m_buffer.size() - 2; }
        for (; line_index < m_buffer.size(); line_index++) {
            const String &line = m_buffer[line_index];
            // Determine the selection to pass in to the renderer and dimensions to use for selection background.
            i32 bg_start = 0;
            i32 bg_end = 0;
             // TODO figure out a good value for this
            i32 newline_indicator = 10;
            if (line_index >= m_selection.line_begin && line_index <= m_selection.line_end) {
                if (line_index == m_selection.line_begin) {
                    if (m_selection.line_begin == m_selection.line_end) {
                        selection = Renderer::Selection(m_selection.begin, m_selection.end);
                        bg_start = m_selection.x_begin;
                        bg_end = m_selection.x_end;
                    } else {
                        selection = Renderer::Selection(m_selection.begin, line.size());
                        bg_start = m_selection.x_begin;
                        bg_end = m_buffer_length[line_index] + newline_indicator;
                    }
                } else if (line_index == m_selection.line_end) {
                    if (m_selection.line_begin == m_selection.line_end) {
                        selection = Renderer::Selection(m_selection.begin, m_selection.end);
                        bg_start = m_selection.x_begin;
                        bg_end = m_selection.x_end;
                    } else {
                        selection = Renderer::Selection(0, m_selection.end);
                        bg_start = 0;
                        bg_end = m_selection.x_end;
                    }
                } else {
                    selection = Renderer::Selection(0, line.size());
                    bg_start = 0;
                    bg_end = m_buffer_length[line_index] + newline_indicator;
                }
            } else { selection = Renderer::Selection(); }

            // Draw selection background
            i32 selection_extra = selection.begin != selection.end ? m_cursor_width : 0;
            dc.fillRect(
                Rect(
                    x_scroll_offset + bg_start,
                    text_region.y,
                    (bg_end - bg_start) + selection_extra,
                    TEXT_HEIGHT
                ),
                dc.accentWidgetBackground(style())
            );

            // Draw the text buffer.
            dc.fillTextAligned(
                font(),
                line.slice(),
                HorizontalAlignment::Left,
                VerticalAlignment::Top,
                text_region,
                0,
                dc.textForeground(style()),
                m_tab_width,
                selection,
                dc.textSelected(style())
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
                dc.textDisabled(style()),
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
            dc.textForeground(style()) // TODO should be a separate color setting
        );
    }

    if (m_mode == Mode::MultiLine) {
        // Here we do a little trick to draw the scrollbars as if we didnt have padding
        // while still having it for text coordinates.
        Size padding = Size();
        dc.sizeHintPadding(padding, style());
        Rect scrollbars = Rect(post_padding.x - paddingLeft(), post_padding.y - paddingTop(), post_padding.w + padding.w, post_padding.h + padding.h);
        drawScrollBars(dc, scrollbars, m_virtual_size);
    }
    dc.setClip(focus_rect); // No need to keep the last clip since we are done using it anyway.
    dc.drawKeyboardFocus(focus_rect, style(), state);
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
        // A little workaround to update the selection x values after text changes.
        // TODO we should probably do this everytime we draw since the x values can
        // get out of date so quickly and any of them recorded in history become invalid
        // once text size has changed
        if (m_buffer.size()) {
            bool did_swap = swapSelection();
            if (m_selection.line_begin == m_selection.line_end) {
                const String &line = m_buffer[m_selection.line_begin];
                m_selection.x_begin = dc.measureText(font(), Slice<const char>(line.data(), m_selection.begin), m_tab_width).w;
                m_selection.x_end = m_selection.x_begin + dc.measureText(font(), Slice<const char>(line.data() + m_selection.begin, m_selection.end - m_selection.begin), m_tab_width).w;
                if (did_swap) {
                    i32 temp_x = m_selection.x_begin;
                    m_selection.x_begin = m_selection.x_end;
                    m_selection.x_end = temp_x;
                    i32 temp = m_selection.begin;
                    m_selection.begin = m_selection.end;
                    m_selection.end = temp;
                }
            } else {
                {
                    const String &line = m_buffer[m_selection.line_begin];
                    m_selection.x_begin = dc.measureText(font(), Slice<const char>(line.data(), m_selection.begin), m_tab_width).w;
                }
                {
                    const String &line = m_buffer[m_selection.line_end];
                    m_selection.x_end = dc.measureText(font(), Slice<const char>(line.data(), m_selection.end), m_tab_width).w;
                }
                if (did_swap) {
                    i32 temp_x = m_selection.x_begin;
                    m_selection.x_begin = m_selection.x_end;
                    m_selection.x_end = temp_x;
                    i32 temp = m_selection.begin;
                    m_selection.begin = m_selection.end;
                    m_selection.end = temp;
                    i32 temp_line = m_selection.line_begin;
                    m_selection.line_begin = m_selection.line_end;
                    m_selection.line_end = temp_line;
                }
            }

        }
        m_text_changed = false;
    }
    if (m_size_changed) {
        Size size;
        if (m_mode == Mode::MultiLine) { size = m_viewport; }
        else { size = Size(m_viewport.w, TEXT_HEIGHT); }
        dc.sizeHintMargin(size, style());
        dc.sizeHintBorder(size, style());
        dc.sizeHintPadding(size, style());

        m_size = size;
        m_size_changed = false;

        return size;
    } else {
        return m_size;
    }
}

String TextEdit::text() {
    u64 length = 0;
    for (u64 i = 0; i < m_buffer.size(); i++) {
        const String &line = m_buffer[i];
        if (i == m_buffer.size() - 1) {
            length += line.size();
        } else {
            length += line.size() + 1; // +1 to account for newline
        }
    }
    String s = String(length);
    u64 offset = 0;
    for (u64 i = 0; i < m_buffer.size(); i++) {
        const String &line = m_buffer[i];
        if (i == m_buffer.size() - 1) {
            memcpy(s.data() + offset, line.data(), line.size());
            offset += line.size();
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
    m_selection.line_end = 0;
    m_selection.x_end = 0;
    m_selection.end = 0;
    m_last_codepoint_index = 0;
    _endSelection();
    m_history = History();
    _updateView(DC);
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
    u64 viewport_start_x = X_SCROLL_OFFSET;
    u64 viewport_start_y = Y_SCROLL_OFFSET;
    u64 next_x_pos = m_selection.x_end;
    u64 next_y_pos = m_selection.line_end * TEXT_HEIGHT;

    if (next_x_pos < viewport_start_x) {
        m_horizontal_scrollbar->m_slider->m_value = (next_x_pos) / (f64)(m_virtual_size.w - inner_rect.w);
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
            m_selection.x_end = m_buffer_length[m_selection.line_end];
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
            m_selection.x_end = 0;
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
            m_selection.x_end = 0;
            m_selection.line_end = next_line;
            return;
        }

        utf8::Iterator next_line_iter = m_buffer[next_line].utf8Begin();
        u64 next_codepoint_index = 0;
        i32 next_x_end = 0;
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
        m_selection.x_end = 0;
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
            m_selection.x_end = 0;
            m_selection.line_end = next_line;
            return;
        }

        utf8::Iterator next_line_iter = m_buffer[next_line].utf8Begin();
        u64 next_codepoint_index = 0;
        i32 next_x_end = 0;
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
        m_selection.x_end = m_buffer_length[m_selection.line_end];
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
    m_selection.x_end = 0;
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
    m_selection.x_end = m_buffer_length[m_selection.line_end];
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

TextEdit* TextEdit::clear() {
    m_buffer.clear();
    m_buffer.push_back(String());
    m_buffer_length.clear();
    m_buffer_length.push_back(0);
    m_text_changed = true;
    m_selection.line_end = 0;
    m_selection.x_end = 0;
    m_selection.end = 0;
    m_last_codepoint_index = 0;
    _endSelection();
    m_history = History();
    _updateView(DC);
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

bool TextEdit::deleteOne(bool is_backspace, bool skip) {
    DrawingContext &dc = DC;

    if (m_selection.hasSelection()) { return false; }

    String &line = m_buffer[m_selection.line_end];
    u64 &line_length = m_buffer_length[m_selection.line_end];
    if (is_backspace) {
        if (m_selection.end) {
            utf8::Iterator iter = utf8::Iterator(line.data(), m_selection.end).prev();
            assert(iter && "There should be a valid codepoint here since we already checked that we are not at the beginning of the line!");
            String text = String(iter.data, iter.length);
            Size text_size = dc.measureText(font(), text.slice(), m_tab_width);
            line_length -= text_size.w;
            if ((i32)line_length + text_size.w + m_cursor_width == m_virtual_size.w) { _updateVirtualWidth(); }
            line.erase(m_selection.end - iter.length, iter.length);
            m_selection.end -= iter.length;
            m_selection.x_end -= text_size.w;
            m_last_codepoint_index--;
            _endSelection();
            if (!skip) {
                m_history.append(HistoryItem(HistoryItem::Action::Backspace, text, m_selection));
            }
        // Delete newline between this and the previous line if one exists
        } else {
            if (m_selection.line_end) {
                String &previous_line = m_buffer[m_selection.line_end - 1];
                u64 &previous_line_length = m_buffer_length[m_selection.line_end - 1];
                m_selection.end = previous_line.size();
                m_selection.x_end = previous_line_length;
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
                if (!skip) {
                    m_history.append(HistoryItem(HistoryItem::Action::Backspace, "\n", m_selection));
                }
            } else {
                return false;
            }
        }
    } else {
        if (m_selection.end < line.size()) {
            u64 codepoint_length = utf8::length(line.data() + m_selection.end);
            Slice<const char> text = Slice<const char>(line.data() + m_selection.end, codepoint_length);
            Size text_size = dc.measureText(font(), text, m_tab_width);
            line_length -= text_size.w;
            if ((i32)line_length + text_size.w + m_cursor_width == m_virtual_size.w) { _updateVirtualWidth(); }
            if (!skip) {
                m_history.append(HistoryItem(HistoryItem::Action::Delete, String(text.data, text.length), m_selection));
            }
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
                if (!skip) {
                    m_history.append(HistoryItem(HistoryItem::Action::Delete, "\n", m_selection));
                }
            } else {
                return false;
            }
        }
    }

    _updateView(dc);
    update();
    onTextChanged.notify();
    return true;
}

bool TextEdit::deleteSelection(bool skip) {
    DrawingContext &dc = DC;

    if (!m_selection.hasSelection()) { return false; }

    Selection backup = m_selection; // This is needed because call to selection() can swap.
    if (!skip) {
        m_history.append(HistoryItem(HistoryItem::Action::Delete, selection(), backup));
    }
    m_selection = backup;

    if (m_selection.line_begin != m_selection.line_end) {
        // Multiline selection
        swapSelection();
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
        swapSelection();
        String &line = m_buffer[m_selection.line_end];
        u64 &line_length = m_buffer_length[m_selection.line_end];
        Size text_size = dc.measureText(font(), Slice<const char>(line.data() + m_selection.begin, m_selection.end - m_selection.begin), m_tab_width);
        line_length -= text_size.w;
        if ((i32)line_length + text_size.w + m_cursor_width == m_virtual_size.w) { _updateVirtualWidth(); }
        line.erase(m_selection.begin, m_selection.end - m_selection.begin);
    }
    _beginSelection();

    _updateView(dc);
    update();
    onTextChanged.notify();
    return true;
}

void TextEdit::selectAll() {
    m_selection.x_begin = 0;
    m_selection.line_begin = 0;
    m_selection.begin = 0;
    m_selection.x_end = m_buffer_length[m_buffer.size() - 1];
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

    deleteSelection(skip);

    if (!skip) {
        m_history.append(HistoryItem(HistoryItem::Action::Insert, text, m_selection));
    }

    if (m_mode == Mode::MultiLine) {
        utf8::Iterator iter = utf8::Iterator(text);
        u64 length = 0;
        i32 text_size = 0;
        bool is_first_line = true;
        u64 last_line_index = 0;
        while ((iter = iter.next())) {
            if (iter.codepoint == '\n') {
                if (is_first_line) {
                    String &line = m_buffer[m_selection.line_end];
                    u64 &line_length = m_buffer_length[m_selection.line_end];
                    line.insert(m_selection.end, text, length);
                    line_length += text_size;
                    is_first_line = false;

                    Slice<const char> remainder_text = Slice<const char>(line.data() + m_selection.end + length, line.size() - (m_selection.end + length));
                    if (remainder_text.length) {
                        i32 remainder_width = dc.measureText(font(), remainder_text, m_tab_width).w;
                        m_buffer.emplace(m_buffer.begin() + m_selection.line_end + 1, remainder_text.data, remainder_text.length);
                        m_buffer_length.insert(m_buffer_length.begin() + m_selection.line_end + 1, remainder_width);
                        line.erase(m_selection.end + length, remainder_text.length);
                        line_length -= remainder_width;
                    } else {
                        m_buffer.insert(m_buffer.begin() + m_selection.line_end + 1, String());
                        m_buffer_length.insert(m_buffer_length.begin() + m_selection.line_end + 1, 0);
                    }
                } else {
                    m_buffer.emplace(m_buffer.begin() + m_selection.line_end, text + last_line_index, length);
                    m_buffer_length.insert(m_buffer_length.begin() + m_selection.line_end, text_size);
                }
                m_selection.line_end++;
                m_virtual_size.h += TEXT_HEIGHT;
                last_line_index += length + 1; // +1 to step over the newline.
                m_selection.x_end = 0;
                m_selection.end = 0;
                text_size = 0;
                length = 0;
                m_last_codepoint_index = 0;
            } else {
                text_size += dc.measureText(font(), Slice<const char>(iter.data - iter.length, iter.length), m_tab_width).w;
                length += iter.length;
                m_last_codepoint_index++;
            }
        }
        u64 &line_length = m_buffer_length[m_selection.line_end];
        line_length += text_size;
        m_buffer[m_selection.line_end].insert(m_selection.end, text + last_line_index, length);
        m_selection.x_end += text_size;
        m_selection.end += length;
    } else {
        // TODO in the future remove newlines and carriage return
        // and perhaps since we know the newlines are removed we could just run it on the same path as multiline
        u64 length = strlen(text);
        i32 text_size = dc.measureText(font(), Slice<const char>(text, length), m_tab_width).w;
        u64 &line_length = m_buffer_length[m_selection.line_end];
        m_buffer[m_selection.line_end].insert(m_selection.end, text, length);
        line_length += text_size;
        m_selection.x_end += text_size;
        m_selection.end += length;
    }
    _updateVirtualWidth();
    _endSelection();

    _updateView(dc);
    update();
    onTextChanged.notify();
}

bool TextEdit::setCursor(u64 line, u64 codepoint) {
    DrawingContext &dc = DC;
    if (line < m_buffer.size()) {
        const String &text = m_buffer[line];
        utf8::Iterator iter = text.utf8Begin();
        m_last_codepoint_index = 0;
        m_selection.line_end = line;
        m_selection.x_end = 0;
        m_selection.end = 0;
        while ((iter = iter.next())) {
            m_selection.x_end += dc.measureText(font(), Slice<const char>(iter.data - iter.length, iter.length), m_tab_width).w;
            m_selection.end += iter.length;
            m_last_codepoint_index++;
            if (m_last_codepoint_index == codepoint) {
                _endSelection();
                _updateView(dc);
                update();
                return true;
            }
        }
    }
    _endSelection();
    _updateView(dc);
    update();
    return false;
}

bool TextEdit::undo() {
    if (!m_history.undo_end) {
        HistoryItem item = m_history.get(m_history.index);
        switch (item.action) {
            case HistoryItem::Action::Delete:
                m_selection = item.selection;
                swapSelection();
                _beginSelection();
                insert(item.text.data(), true);
                m_selection = item.selection;
                break;
            case HistoryItem::Action::Backspace:
                m_selection = item.selection;
                insert(item.text.data(), true);
                m_selection = item.selection;
                moveCursorRight();
                break;
            case HistoryItem::Action::Insert: {
                m_selection = item.selection;
                utf8::Iterator iter = item.text.utf8Begin();
                while ((iter = iter.next())) {
                    deleteOne(false, true);
                }
                break;
            }
            default: assert(false && "Not a valid HistoryItem!");
        }
        if (!m_history.index) {
            m_history.undo_end = true;
        } else {
            m_history.index--;
        }
        m_history.redo_end = false;
        return true;
    }
    return false;
}

bool TextEdit::redo() {
    if (m_history.index < m_history.items.size() && !m_history.redo_end) {
        HistoryItem item = m_history.get(m_history.undo_end ? 0 : ++m_history.index);
        switch (item.action) {
            case HistoryItem::Action::Delete:
            case HistoryItem::Action::Backspace:
                m_selection = item.selection;
                if (!deleteSelection(true)) { deleteOne(false, true); }
                break;
            case HistoryItem::Action::Insert:
                m_selection = item.selection;
                insert(item.text.data(), true);
                break;
            default: assert(false && "Not a valid HistoryItem!");
        }
        if (!m_history.index) {
            m_history.undo_end = false;
        }
        if (m_history.index == m_history.items.size() - 1) {
            m_history.redo_end = true;
        }
        return true;
    }
    return false;
}

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

Widget* TextEdit::handleFocusEvent(FocusEvent event, State *state, FocusPropagationData data) {
    return Widget::handleFocusEvent(event, state, data);
}

bool TextEdit::handleLayoutEvent(LayoutEvent event) {
    if (event) {
        if (event & LAYOUT_FONT) {
            // If the font or font size have changed we need to recalculate the text extents.
            m_text_changed = true;
            // If no other bits are set and the TextEdit is NOT operating in SingleLine mode
            // then we can return early since the viewport doesnt change.
            // For SingleLine mode we need to keep going since the text height affects the returned size from sizeHint.
            if (!(event^LAYOUT_FONT) && m_mode == Mode::MultiLine) { return true; }
        }
        if (event & LAYOUT_SCALE) {
            m_text_changed = true;
            if (_style.font.get()) {
                setFont(std::shared_ptr<Font>(_style.font.get()->reload(Application::get()->currentWindow()->dc->default_style.font->pixel_size)));
            }
        }
        // Since we already know the layout needs to be redone
        // we return true to avoid having to traverse the entire widget graph to the top.
        if (m_size_changed) { return true; }
        m_size_changed = true;
    }
    return false;
}
