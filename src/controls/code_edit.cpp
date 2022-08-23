#include "code_edit.hpp"

#define NORMALIZE(min, max, value) (value < min ? min : value > max ? max : value)
#define TEXT_HEIGHT ((i32)((font()->maxHeight()) + m_line_spacing))
#define Y_SCROLL_OFFSET ((m_vertical_scrollbar->isVisible() ? m_vertical_scrollbar->m_slider->m_value : 0.0) * (m_virtual_size.h - inner_rect.h))
#define IS_NOT_BASE_X_SCROLL ((m_horizontal_scrollbar->isVisible() ? m_horizontal_scrollbar->m_slider->m_value : 0.0) > 0.0)

Lexer::Lexer() {}

// TODO just gotta fix escapes
// they continue on until the next token
void Lexer::lex(Slice<const char> source) {
    pos = Offset();
    this->source = source;
    tokens = Slice<Token>(new Token[source.length], 0);

    while (pos.index < source.length) {
        u8 next_c = source.data[pos.index];
        Token::Type token = (Token::Type)next_c;
        if (next_c == '/') {
            if (peek(Token::Type::ForwardSlash)) {
                tokens.data[tokens.length++] = Token(Token::Type::SingleLineComment, pos.index);
                pos.advance(2);
                while (pos.index < source.length) {
                    if ((Token::Type)source.data[pos.index] == Token::Type::LF) {
                        pos.next();
                        break;
                    }
                    pos.next();
                }
            } else if (peek(Token::Type::Asterisk)) {
                tokens.data[tokens.length++] = Token(Token::Type::MultiLineComment, pos.index);
                pos.advance(2);
                while (pos.index < source.length) {
                    if ((Token::Type)source.data[pos.index] == Token::Type::Asterisk && peek(Token::Type::ForwardSlash)) {
                        pos.advance(2);
                        break;
                    }
                    pos.next();
                }
            } else {
                tokens.data[tokens.length++] = Token(Token::Type::ForwardSlash, pos.index);
                pos.next();
            }
        } else if (isalpha(next_c) || next_c == '_') {
            u32 identifier_start = pos.index;
            pos.next();
            while (pos.index < source.length) {
                if (!isalnum(source.data[pos.index]) && source.data[pos.index] != '_') {
                    Slice<const char> identifier = Slice<const char>(source.data + identifier_start, pos.index - identifier_start);
                    auto entry = keywords.find(identifier);
                    if (entry) {
                        tokens.data[tokens.length++] = Token(entry.value, identifier_start);
                    } else {
                        if (source.data[pos.index] == '(') {
                            tokens.data[tokens.length++] = Token(Token::Type::Function, identifier_start);
                        } else {
                            tokens.data[tokens.length++] = Token(Token::Type::Identifier, identifier_start);
                        }
                    }
                    break;
                }
                pos.next();
            }
        } else if (isdigit(next_c)) {
            tokens.data[tokens.length++] = Token(Token::Type::Number, pos.index);
            pos.next();
            while (pos.index < source.length) {
                if (!isalnum(source.data[pos.index])) {
                    break;
                }
                pos.next();
            }
        } else if (next_c == '"') {
            tokens.data[tokens.length++] = Token(Token::Type::String, pos.index);
            pos.next();
            while (pos.index < source.length) {
                if ((Token::Type)source.data[pos.index] == Token::Type::BackwardSlash) {
                    tokens.data[tokens.length++] = Token(Token::Type::Escaped, pos.index);
                    pos.advance(2);
                } else if ((Token::Type)source.data[pos.index] == Token::Type::DoubleQuotes) {
                    pos.next();
                    break;
                } else if ((Token::Type)source.data[pos.index] == Token::Type::LF) {
                    pos.next();
                    break;
                } else {
                    pos.next();
                }
            }
        } else if (next_c == '\'') {
            tokens.data[tokens.length++] = Token(Token::Type::Character, pos.index);
            pos.next();
            while (pos.index < source.length) {
                if ((Token::Type)source.data[pos.index] == Token::Type::BackwardSlash) {
                    tokens.data[tokens.length++] = Token(Token::Type::Escaped, pos.index);
                    pos.advance(2);
                } else if ((Token::Type)source.data[pos.index] == Token::Type::SingleQuote) {
                    pos.next();
                    break;
                } else if ((Token::Type)source.data[pos.index] == Token::Type::LF) {
                    pos.next();
                    break;
                } else {
                    pos.next();
                }
            }
        } else if (next_c == '#') {
            tokens.data[tokens.length++] = Token(Token::Type::PreProcessorStatement, pos.index);
            pos.next();
            while (pos.index < source.length) {
                if ((Token::Type)source.data[pos.index] == Token::Type::BackwardSlash) {
                    tokens.data[tokens.length++] = Token(Token::Type::Escaped, pos.index);
                    pos.advance(2);
                } else if ((Token::Type)source.data[pos.index] == Token::Type::LF) {
                    pos.next();
                    break;
                } else {
                    pos.next();
                }
            }
        } else {
            tokens.data[tokens.length++] = Token(token, pos.index);
            pos.next();
        }
    }
}

bool Lexer::peek(Token::Type type) {
    if (pos.index + 1 < source.length && (Token::Type)(source.data[pos.index + 1]) == type) { return true; }
    return false;
}

MinimapButton::MinimapButton(Size min_size) : Widget() {
    setMinSize(min_size);
}

MinimapButton::~MinimapButton() {}

const char* MinimapButton::name() {
    return "MinimapButton";
}

void MinimapButton::draw(DrawingContext &dc, Rect rect, i32 state) {
    this->rect = rect;
    Color color;
    if (state & STATE_PRESSED) {
        color = Color("#888888aa");
    } else if (state & STATE_HOVERED) {
        color = Color("#88888888");
    } else {
        color = Color("#88888855");
    }
    dc.drawBorder(rect, style(), state);
    dc.fillRect(rect, color);
}

Size MinimapButton::sizeHint(DrawingContext &dc) {
    return m_size;
}

MinimapButton* MinimapButton::setMinSize(Size size) {
    m_size = size;
    Application::get()->currentWindow()->dc->sizeHintBorder(m_size, style());
    layout(LAYOUT_STYLE);
    return this;
}

Size MinimapButton::minSize() {
    return m_size;
}

i32 MinimapButton::isFocusable() {
    return (i32)FocusType::Focusable;
}

Minimap::Minimap() : Box(Align::Vertical) {
    m_slider_button = new MinimapButton();
    append(m_slider_button, Fill::Both);
    m_slider_button->onMouseDown.addEventListener([&](Widget *widget, MouseEvent event) {
        // The origin point is the position from where
        // the value calculations will begin.
        // It determines the start and end points for the value axis.
        m_origin = m_slider_button->rect.y + m_slider_button->rect.h - event.y;
    });
    m_slider_button->onMouseMotion.addEventListener([&](Widget *widget, MouseEvent event) {
        MinimapButton *self = m_slider_button;
        if (self->isPressed()) {
            Rect rect = this->rect;
            i32 size = m_slider_button_size;

            i32 event_pos = event.y;
            i32 position = rect.y;
            i32 length = rect.h;
            i32 start = size - m_origin;
            f64 value = (event_pos - (position + start)) / (f64)(length - start - m_origin);
            m_value = NORMALIZE(m_min, m_max, value);
            // Sync the minimap movement with the vertical scrollbar.
            ((Scrollable*)parent)->m_vertical_scrollbar->m_slider->m_value = m_value;

            onValueChanged.notify();
            update();
        }
    });
    onMouseDown.addEventListener([&](Widget *widget, MouseEvent event) {
        Rect rect = this->rect;
        i32 size = m_slider_button_size;
        m_value = (event.y - (rect.y + size / 2.0)) / (f64)(rect.h - size);
        m_value = NORMALIZE(m_min, m_max, m_value);
        // Sync the minimap jump with the vertical scrollbar.
        ((Scrollable*)parent)->m_vertical_scrollbar->m_slider->m_value = m_value;
    });
}

Minimap::~Minimap() {}

const char* Minimap::name() {
    return "Minimap";
}

void Minimap::draw(DrawingContext &dc, Rect rect, i32 state) {
    this->rect = rect;

    // TODO possibly draw selection here as well??
    auto coords = TextureCoordinates();
    assert(m_minimap_texture.get() && "Minimap texture should not be null!");
    dc.drawTexture(Point(rect.x, rect.y), Size(rect.w, rect.h), m_minimap_texture.get(), &coords, COLOR_WHITE);

    // Get the size of the slider button.
    i32 size;
    Size sizehint = m_slider_button->sizeHint(dc);
    if (!m_slider_button_size) {
        // Button size was not set. Default.
        if (m_align_policy == Align::Horizontal) {
            size = sizehint.w;
        } else {
            size = sizehint.h;
        }
    } else {
        // Button size was set externally, usually by a scrollable widget.
        size = m_slider_button_size;
    }

    // Determine and draw the location of the slider button.
    i32 start = size - m_origin;
    rect.y += (rect.h - start - m_origin) * m_value;
    m_slider_button->draw(dc, Rect(rect.x, rect.y, rect.w, size), m_slider_button->state());
}

Size Minimap::sizeHint(DrawingContext &dc) {
    return Size(1, 1);
}

bool Minimap::handleScrollEvent(ScrollEvent event) {
    m_value = NORMALIZE(m_min, m_max, m_value + m_step * event.y);
    // Sync the minimap scroll with the vertical scrollbar.
    ((Scrollable*)parent)->m_vertical_scrollbar->m_slider->m_value = m_value;
    onValueChanged.notify();
    update();
    return true;
}


CodeEdit::CodeEdit(String text, Size min_size) : TextEdit(text, "", TextEdit::Mode::MultiLine, min_size) {
    m_minimap = new Minimap();
    append(m_minimap);
    m_vertical_scrollbar->m_slider->onValueChanged.addEventListener([&]() {
        m_minimap->m_value = m_vertical_scrollbar->m_slider->m_value;
    });
    onTextChanged.addEventListener([&]() {
        delete[] m_lexer.tokens.data;
        m_lexer.lex(this->text());
        __renderMinimap(Size(m_minimap->m_minimap_width, inner_rect.h));
    });
    onTextChanged.notify();
}

CodeEdit::~CodeEdit() {
    delete[] m_lexer.tokens.data;
}

const char* CodeEdit::name() {
    return "CodeEdit";
}

void CodeEdit::draw(DrawingContext &dc, Rect rect, i32 state) {
    dc.margin(rect, style());
    this->rect = rect;
    Rect previous_clip = dc.clip();
    dc.drawBorder(rect, style(), state);
    dc.setClip(rect.clipTo(previous_clip));
    Rect focus_rect = rect;
    dc.fillRect(rect, dc.textBackground(style()));
    dc.padding(rect, style());

    m_virtual_size.h = (m_buffer.size() * TEXT_HEIGHT);
    if (m_overscroll && m_virtual_size.h > rect.h) { m_virtual_size.h = ((m_buffer.size() - 2) * TEXT_HEIGHT) + rect.h; }

    i32 line_numbers_width = dc.measureText(font(), toString(m_buffer.size() + 1), m_tab_width).w; // +1 because line numbers are 1 based not 0
    u32 line_numbers_padding = 10;
    rect.w -= line_numbers_width + line_numbers_padding + m_minimap->m_minimap_width;

    Point pos = __automaticallyAddOrRemoveScrollBars(dc, rect, m_virtual_size);
    Rect post_padding = rect;
    inner_rect = rect;

    inner_rect.x += line_numbers_width + line_numbers_padding;
    pos.x += line_numbers_width + line_numbers_padding;

    Rect text_region = Rect(pos.x, pos.y, inner_rect.w, inner_rect.h);
    // Draw normal text;
    u64 line_index = NORMALIZE(0, (i32)m_buffer.size() - 1, -((pos.y - inner_rect.y) / TEXT_HEIGHT));
    text_region.y += TEXT_HEIGHT * line_index;
    i32 x_scroll_offset = (pos.x);
    if (!(m_buffer.size() == 1 && !m_buffer[0].size())) {
        Selection before_swap = m_selection;
        bool did_swap = swapSelection();
        Renderer::Selection selection;
        u64 byte_offset = 0;
        for (u64 line = 0; line < line_index; byte_offset += m_buffer[line].size() + 1, line++); // +1 for newline
        if (IS_NOT_BASE_X_SCROLL) {
            dc.fillRect(
                Rect(
                    rect.x + line_numbers_width + (line_numbers_padding / 2) - 1,
                    this->rect.y,
                    1,
                    this->rect.h
                ),
                Color("#cccccc")
            );
        }
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

            dc.fillTextAligned(
                font(),
                toString(line_index + 1),
                HorizontalAlignment::Center,
                VerticalAlignment::Center,
                Rect(rect.x, text_region.y, line_numbers_width, TEXT_HEIGHT),
                0,
                Color("#cccccc"),
                0,
                Renderer::Selection(),
                COLOR_NONE
            );

            // Modify clipping region to account for line numbers;
            dc.renderer->clip_rect.x += line_numbers_width + line_numbers_padding;
            dc.renderer->clip_rect.w -= line_numbers_width + line_numbers_padding;

            // Draw selection background
            i32 selection_extra = selection.begin != selection.end ? m_cursor_width : 0;
            dc.fillRect(
                Rect(
                    x_scroll_offset + bg_start,
                    text_region.y,
                    (bg_end - bg_start) + selection_extra,
                    TEXT_HEIGHT
                ),
                Color("#cccccc55")
            );

            // Draw the text buffer.
            __fillSingleLineColoredText(
                font(),
                line.slice(),
                Point(text_region.x, text_region.y),
                byte_offset,
                COLOR_BLACK,
                selection,
                dc.textSelected(style())
            );

            // Reset the clipping region back to what it was.
            dc.renderer->clip_rect.x -= line_numbers_width + line_numbers_padding;
            dc.renderer->clip_rect.w += line_numbers_width + line_numbers_padding;

            byte_offset += line.size() + 1; // +1 for newline
            text_region.y += TEXT_HEIGHT;
            if (text_region.y > rect.y + rect.h) { break; }
        }
        if (inner_rect.h != m_minimap->m_minimap_texture->height) {
            __renderMinimap(Size(m_minimap->m_minimap_width, inner_rect.h));
        }
        {
            i32 slider_size = m_virtual_size.h / inner_rect.h > (i32)m_buffer.size() ? m_buffer.size() : inner_rect.h;
            slider_size = (rect.h / TEXT_HEIGHT) / (f32)m_buffer.size() * (f32)(inner_rect.h > (i32)m_buffer.size() ? m_buffer.size() : inner_rect.h) + 1;
            if (slider_size < 5) {
                slider_size = 5;
            }
            m_minimap->m_slider_button_size = slider_size;
        }
        dc.fillRect(
            Rect(
                inner_rect.x + inner_rect.w + m_cursor_width, this->rect.y,
                m_minimap->m_minimap_width + (post_padding.x - this->rect.x), this->rect.h
            ),
            dc.textBackground(style())
        );
        m_minimap->draw(
            dc,
            Rect(
                inner_rect.x + inner_rect.w + m_cursor_width, inner_rect.y,
                m_minimap->m_minimap_width, inner_rect.h > (i32)m_buffer.size() ? m_buffer.size() : inner_rect.h
            ),
            m_minimap->state()
        );
        dc.fillRect(
            Rect(
                inner_rect.x + inner_rect.w + m_cursor_width,
                inner_rect.y + (m_selection.line_begin / (f32)m_buffer.size() * (f32)(inner_rect.h > (i32)m_buffer.size() ? m_buffer.size() : inner_rect.h)),
                m_minimap->m_minimap_width,
                (m_selection.line_end - m_selection.line_begin) / (f32)m_buffer.size() * (f32)(inner_rect.h > (i32)m_buffer.size() ? m_buffer.size() : inner_rect.h) + 1
            ),
            Color("#cccccc55")
        );
        if (m_horizontal_scrollbar->isVisible()) {
            dc.fillRect(
                Rect(
                    inner_rect.x + inner_rect.w + m_cursor_width - 1,
                    this->rect.y,
                    1,
                    this->rect.h
                ),
                Color("#cccccc")
            );
        }
        if (did_swap) { m_selection = before_swap; }
    // Draw placeholder text.
    } else {
        for (; line_index < m_placeholder_buffer.size(); line_index++) {
            const String &line = m_placeholder_buffer[line_index];
            dc.fillTextAligned(
                font(),
                toString(line_index + 1),
                HorizontalAlignment::Center,
                VerticalAlignment::Center,
                Rect(rect.x, text_region.y, line_numbers_width, TEXT_HEIGHT),
                0,
                Color("#cccccc"),
                0,
                Renderer::Selection(),
                COLOR_NONE
            );
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
        dc.renderer->clip_rect.x += line_numbers_width + line_numbers_padding;
        dc.renderer->clip_rect.w -= line_numbers_width + line_numbers_padding * 2 + m_minimap->m_minimap_width + (post_padding.x - this->rect.x);
        dc.fillRect(
            Rect(
                x_scroll_offset + m_selection.x_end,
                y + (TEXT_HEIGHT * m_selection.line_end),
                m_cursor_width,
                TEXT_HEIGHT
            ),
            dc.textForeground(style()) // TODO should be a separate color setting
        );
        dc.renderer->clip_rect.x -= line_numbers_width + line_numbers_padding;
        dc.renderer->clip_rect.w += line_numbers_width + line_numbers_padding * 2 + m_minimap->m_minimap_width + (post_padding.x - this->rect.x);
    }

    {
        // Here we do a little trick to draw the scrollbars as if we didnt have padding
        // while still having it for text coordinates.
        Size padding = Size();
        dc.sizeHintPadding(padding, style());
        Rect scrollbars = Rect(post_padding.x - paddingLeft(), post_padding.y - paddingTop(), post_padding.w + padding.w, post_padding.h + padding.h);
        __drawScrollBars(dc, scrollbars, m_virtual_size, line_numbers_width + line_numbers_padding + m_minimap->m_minimap_width + m_cursor_width);
    }
    dc.setClip(focus_rect); // No need to keep the last clip since we are done using it anyway.
    dc.drawKeyboardFocus(focus_rect, style(), state);
    dc.setClip(previous_clip);
}

Point CodeEdit::__automaticallyAddOrRemoveScrollBars(DrawingContext &dc, Rect &rect, const Size &virtual_size) {
    i32 content_x = rect.x;
    i32 content_y = rect.y;
    bool vert = false;
    if (rect.h < virtual_size.h) {
        vert = true;
        if (!m_vertical_scrollbar->isVisible()) {
            m_vertical_scrollbar->show();
        }
        rect.w -= m_vertical_scrollbar->sizeHint(dc).w;
    }
    if (rect.w < virtual_size.w) {
        if (!m_horizontal_scrollbar->isVisible()) {
            m_horizontal_scrollbar->show();
        }
        rect.h -= m_horizontal_scrollbar->sizeHint(dc).h;
        if (rect.h < virtual_size.h) {
            if (!m_vertical_scrollbar->isVisible()) {
                m_vertical_scrollbar->show();
            }
            if (!vert) {
                rect.w -= m_vertical_scrollbar->sizeHint(dc).w;
            }
        }
    } else {
        if (m_horizontal_scrollbar->isVisible()) {
            m_horizontal_scrollbar->hide();
        }
    }
    if (!(rect.h < virtual_size.h)) {
        if (m_vertical_scrollbar->isVisible()) {
            m_vertical_scrollbar->hide();
        }
    }
    if (m_vertical_scrollbar->isVisible()) {
        m_vertical_scrollbar->m_slider->m_step = Application::get()->scroll_amount / (f64)(virtual_size.h - rect.h);
    }
    if (m_horizontal_scrollbar->isVisible()) {
        content_x -= m_horizontal_scrollbar->m_slider->m_value * (virtual_size.w - rect.w);
        m_horizontal_scrollbar->m_slider->m_step = Application::get()->scroll_amount / (f64)(virtual_size.w - rect.w);
    }
    content_y -= m_minimap->m_value * (virtual_size.h - rect.h);
    // TODO we may not want to use rect.h here
    m_minimap->m_step = Application::get()->scroll_amount / (f64)(virtual_size.h - rect.h);

    return Point(content_x, content_y);
}

void CodeEdit::__drawScrollBars(DrawingContext &dc, Rect &rect, const Size &virtual_size, i32 extra) {
    rect.w += extra;
    if (m_vertical_scrollbar->isVisible()) {
        Size size = m_vertical_scrollbar->sizeHint(dc);
        i32 slider_size = rect.h * ((rect.h - size.h / 2.0) / virtual_size.h);
        if (slider_size < 5) {
            slider_size = 5;
        }
        m_vertical_scrollbar->m_slider->m_slider_button_size = slider_size;
        m_vertical_scrollbar->draw(
            dc,
            Rect(
                rect.x + rect.w,
                rect.y,
                size.w,
                rect.h > size.h ? rect.h : size.h
            ),
            m_vertical_scrollbar->state()
        );
    }
    if (m_horizontal_scrollbar->isVisible()) {
        Size size = m_horizontal_scrollbar->sizeHint(dc);
        i32 slider_size = rect.w * ((rect.w - extra - size.w / 2.0) / virtual_size.w);
        if (slider_size < 5) {
            slider_size = 5;
        }
        m_horizontal_scrollbar->m_slider->m_slider_button_size = slider_size;
        m_horizontal_scrollbar->draw(
            dc,
            Rect(
                rect.x,
                rect.y + rect.h,
                rect.w > size.w ? rect.w : size.w,
                size.h
            ),
            m_horizontal_scrollbar->state()
        );
    }
    if (m_vertical_scrollbar->isVisible() && m_horizontal_scrollbar->isVisible()) {
        dc.fillRect(Rect(
            rect.x + rect.w,
            rect.y + rect.h,
            m_vertical_scrollbar->sizeHint(dc).w,
            m_horizontal_scrollbar->sizeHint(dc).h),
            dc.widgetBackground(m_vertical_scrollbar->style())
        );
    }
}

Token* binarySearch(u64 target, Slice<Token> tokens) {
    if (!tokens.length) { return nullptr; }
    u64 lower = 0;
    u64 upper = tokens.length - 1;
    u64 mid = 0;
    Token token;

    while (lower <= upper) {
        mid = (lower + upper) / 2;
        token = tokens.data[mid];
        if (target < token.index) {
            upper = mid - 1;
        } else if (target > token.index) {
            lower = mid + 1;
        } else {
            break;
        }
    }

    if (target < token.index) {
       return &tokens.data[mid - 1];
    }
    return &tokens.data[mid];
}

void CodeEdit::__fillSingleLineColoredText(
    std::shared_ptr<Font> font,
    Slice<const char> text,
    Point point,
    u64 byte_offset,
    Color color,
    Renderer::Selection selection,
    Color selection_color
) {
    Renderer &renderer = *Application::get()->currentWindow()->dc->renderer;
    Token *current_token = binarySearch(byte_offset, m_lexer.tokens);

    if (selection.begin > selection.end) {
        auto temp = selection.end;
        selection.end = selection.begin;
        selection.begin = temp;
    }

    if (renderer.last_font != font.get()) {
        renderer.last_font = font.get();
        renderer.render();
    }
    glActiveTexture(renderer.gl_texture_begin + AGRO_OPENGL_RESERVED_FOR_TEXTURE_ARRAY);
    glBindTexture(GL_TEXTURE_2D_ARRAY, font->atlas_ID);
    i32 x = point.x;
    i32 base_bearing = font->max_bearing;
    i32 space_advance = font->get((u32)' ').advance;
    for (u64 i = 0; i < text.length;) {
        u8 c = text.data[i];
        u8 length = utf8::length(text.data + i);
        assert(length && "Invalid utf8 sequence start byte");
        renderer.textCheck(font);
        Font::Character ch;
        i32 advance = -1;
        f32 xpos = -1.0f;
        f32 ypos = -1.0f;
        if (c == ' ') {
            ch = font->get('.');
            advance = space_advance;
            xpos = x + ch.bearing.x;
            ypos = point.y + (font->maxHeight() / 2);
        } else if (c == '\t') {
            ch = font->get('_');
            advance = space_advance * m_tab_width;
            xpos = x + ch.bearing.x;
            ypos = point.y + (font->maxHeight() / 2);
        } else {
            ch = font->get(utf8::decode(text.data + i, length));
            advance = ch.advance;
            xpos = x + ch.bearing.x;
            ypos = point.y + (base_bearing - ch.bearing.y);
        }
        if (x + advance >= renderer.clip_rect.x && x <= renderer.clip_rect.x + renderer.clip_rect.w) {
            auto _color = color;
            if (!current_token) { _color = color; }
            else {
                if (
                    (u32)current_token->type > (u32)Token::Type::Identifier &&
                    (u32)current_token->type <= (u32)Token::Type::ThreadLocal
                ) {
                    _color = Color(0xe4, 0x45, 0x33);
                } else if (current_token->type == Token::Type::MultiLineComment) {
                    _color = Color(0x92, 0x83, 0x72);
                } else if (current_token->type == Token::Type::SingleLineComment) {
                    _color = Color(0x92, 0x83, 0x72);
                } else if (current_token->type == Token::Type::String) {
                    _color = Color(0xb8, 0xbb, 0x26);
                } else if (current_token->type == Token::Type::Character) {
                    _color = Color(0xb8, 0xbb, 0x26);
                } else if (current_token->type == Token::Type::PreProcessorStatement) {
                    _color = Color(0x42, 0x83, 0x72);
                } else if (current_token->type == Token::Type::Number) {
                    _color = Color(0xcb, 0x82, 0x96);
                } else if (current_token->type == Token::Type::Identifier) {
                    _color = Color(0x81, 0xac, 0x71);
                } else if (current_token->type == Token::Type::Escaped) {
                    _color = Color(0xcb, 0x82, 0x96);
                } else if (current_token->type == Token::Type::Function) {
                    _color = Color(0xd0, 0x76, 0x17);
                } else if ((u32)current_token->type < (u32)Token::Type::String) {
                    _color = Color(0x92, 0x83, 0x72);
                } else {
                    assert(false && "Unimplemented Token::Type when syntax highlighting!");
                }
            }
            if (c == ' ') {
                _color.a = 80;
                renderer.textQuad(xpos, ypos, ch, *font.get(), _color);
            } else if (c == '\t') {
                _color.a = 80;
                // TODO we may want to determine better size
                renderer.fillRect(Rect(x + (m_tab_width / 2), ypos, advance - m_tab_width, ch.size.h), _color);
            } else {
                renderer.textQuad(xpos, ypos, ch, *font.get(), _color);
            }
        }
        x += advance;
        if (x > renderer.clip_rect.x + renderer.clip_rect.w) { return; }
        i += length;
        byte_offset += length;
        if (
            current_token &&
            current_token != m_lexer.tokens.data + m_lexer.tokens.length &&
            byte_offset == (current_token + 1)->index
        ) {
            current_token++;
        }
    }
}

void CodeEdit::__renderMinimap(Size size) {
    // In the event that the texture will be taller than the line count
    // set the texture size to which ever is smaller.
    size.h = size.h > (i32)m_buffer.size() ? m_buffer.size() : size.h;
    assert(sizeof(Color) == 4 && "Color size should be 4 bytes or 32bits!");
    u8 *texture = new u8[size.h * size.w * sizeof(Color)];
    memset(texture, 0x00, size.h * size.w * sizeof(Color));
    u64 texture_line = 0;
    f32 line_index = 0;
    u64 byte_offset = 0;
    for (; texture_line < (u64)size.h;) {
        Token *current_token = binarySearch(byte_offset, m_lexer.tokens);
        String &line = m_buffer[(u64)line_index];
        u64 line_byte_offset = byte_offset;
        for (u64 index = 0; index < line.size() and index < (u64)size.w;) {
            const u8 &byte = line[index];
            if (byte == ' ') {
                index++;
                line_byte_offset++;
            } else if (byte == '\t') {
                index += m_tab_width;
                line_byte_offset += m_tab_width;
            } else {
                Color color;
                if (!current_token) { color = Application::get()->currentWindow()->dc->textForeground(style()); }
                else {
                    if (
                        (u32)current_token->type > (u32)Token::Type::Identifier &&
                        (u32)current_token->type <= (u32)Token::Type::ThreadLocal
                    ) {
                        color = Color(0xe4, 0x45, 0x33);
                    } else if (current_token->type == Token::Type::MultiLineComment) {
                        color = Color(0x92, 0x83, 0x72);
                    } else if (current_token->type == Token::Type::SingleLineComment) {
                        color = Color(0x92, 0x83, 0x72);
                    } else if (current_token->type == Token::Type::String) {
                        color = Color(0xb8, 0xbb, 0x26);
                    } else if (current_token->type == Token::Type::Character) {
                        color = Color(0xb8, 0xbb, 0x26);
                    } else if (current_token->type == Token::Type::PreProcessorStatement) {
                        color = Color(0x42, 0x83, 0x72);
                    } else if (current_token->type == Token::Type::Number) {
                        color = Color(0xcb, 0x82, 0x96);
                    } else if (current_token->type == Token::Type::Identifier) {
                        color = Color(0x81, 0xac, 0x71);
                    } else if (current_token->type == Token::Type::Escaped) {
                        color = Color(0xcb, 0x82, 0x96);
                    } else if (current_token->type == Token::Type::Function) {
                        color = Color(0xd0, 0x76, 0x17);
                    } else if ((u32)current_token->type < (u32)Token::Type::String) {
                        color = Color(0x92, 0x83, 0x72);
                    } else {
                        assert(false && "Unimplemented Token::Type when syntax highlighting in minimap!");
                    }
                }
                if ((u64)texture_line % 2) {
                    color.a = 0xaa;
                }
                *(Color*)&texture[(texture_line * size.w * sizeof(Color)) + (index * sizeof(Color))] = color;
                index++;
                line_byte_offset++;
            }
            if (
                current_token &&
                current_token != m_lexer.tokens.data + m_lexer.tokens.length &&
                line_byte_offset == (current_token + 1)->index
            ) {
                current_token++;
            }
        }
        texture_line++;
        for (u64 __line = line_index; __line < (u64)line_index + m_buffer.size() / size.h; __line++) {
            byte_offset += m_buffer[__line].size() + 1; // +1 for newline
        }
        line_index += m_buffer.size() / (f32)size.h;
    }
    m_minimap->m_minimap_texture = std::make_shared<Texture>(texture, size.w, size.h, (i32)sizeof(Color));
    delete[] texture;
}

bool CodeEdit::handleScrollEvent(ScrollEvent event) {
    SDL_Keymod mod = SDL_GetModState();
    if (mod & Mod::Shift) {
        if (m_horizontal_scrollbar->isVisible()) {
            return m_horizontal_scrollbar->m_slider->handleScrollEvent(event);
        }
    } else {
        if (m_vertical_scrollbar->isVisible()) {
            m_minimap->handleScrollEvent(event);
            return m_vertical_scrollbar->m_slider->handleScrollEvent(event);
        }
    }
    return false;
}
