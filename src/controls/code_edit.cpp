#include "code_edit.hpp"

#define TEXT_HEIGHT ((i32)((font()->maxHeight()) + m_line_spacing))
#define Y_SCROLL_OFFSET ((m_vertical_scrollbar->isVisible() ? m_vertical_scrollbar->m_slider->m_value : 0.0) * (m_virtual_size.h - inner_rect.h))
#define IS_BASE_X_SCROLL ((m_vertical_scrollbar->isVisible() ? m_horizontal_scrollbar->m_slider->m_value : 0.0) > 0.0)

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


CodeEdit::CodeEdit(String text, Size min_size) : TextEdit(text, "", TextEdit::Mode::MultiLine, min_size) {
    onTextChanged.addEventListener([&]() {
        delete[] lexer.tokens.data;
        lexer.lex(this->text().slice());
    });
    onTextChanged.notify();
}

CodeEdit::~CodeEdit() {
    delete[] lexer.tokens.data;
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

    Point pos = automaticallyAddOrRemoveScrollBars(dc, rect, m_virtual_size);
    Rect post_padding = rect;
    inner_rect = rect;

    i32 line_numbers_width = dc.measureText(font(), toString(m_buffer.size() + 1), m_tab_width).w; // +1 because line numbers are 1 based not 0
    u32 line_numbers_padding = 10;
    inner_rect.x += line_numbers_width + line_numbers_padding;
    inner_rect.w -= line_numbers_width + line_numbers_padding;
    pos.x += line_numbers_width + line_numbers_padding;

    Rect text_region = Rect(pos.x, pos.y, inner_rect.w, inner_rect.h);
    // Draw normal text;
    u64 line_index = -((pos.y - inner_rect.y) / TEXT_HEIGHT);
    text_region.y += TEXT_HEIGHT * line_index;
    i32 x_scroll_offset = (pos.x);
    if (!(m_buffer.size() == 1 && !m_buffer[0].size())) {
        Selection before_swap = m_selection;
        bool did_swap = swapSelection();
        Renderer::Selection selection;
        u64 byte_offset = 0;
        // TODO font scaling issue here where sometimes the math makes it such
        // that line_index falls outside of m_buffer and text does not render
        // update: the below workaround does work but i would like it to be more "precise"
        if (line_index >= m_buffer.size()) { line_index = m_buffer.size() - 2; }
        for (u64 line = 0; line < line_index; byte_offset += m_buffer[line].size() + 1, line++); // +1 for newline
        if (IS_BASE_X_SCROLL) {
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
        dc.renderer->clip_rect.w -= line_numbers_width + line_numbers_padding;
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
        dc.renderer->clip_rect.w += line_numbers_width + line_numbers_padding;
    }

    {
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

void CodeEdit::__fillSingleLineColoredText(std::shared_ptr<Font> font, Slice<const char> text, Point point, u64 byte_offset, Color color, Renderer::Selection selection, Color selection_color) {
    Renderer &renderer = *Application::get()->currentWindow()->dc->renderer;
    Token *current_token = binarySearch(byte_offset, lexer.tokens);

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
                    _color = Color("#e44533");
                } else if (current_token->type == Token::Type::MultiLineComment) {
                    _color = Color("#928372");
                } else if (current_token->type == Token::Type::SingleLineComment) {
                    _color = Color("#928372");
                } else if (current_token->type == Token::Type::String) {
                    _color = Color("#b8bb26");
                } else if (current_token->type == Token::Type::Character) {
                    _color = Color("#b8bb26");
                } else if (current_token->type == Token::Type::PreProcessorStatement) {
                    _color = Color("#428372");
                } else if (current_token->type == Token::Type::Number) {
                    _color = Color("#cb8296");
                } else if (current_token->type == Token::Type::Identifier) {
                    _color = Color("#81ac71");
                } else if (current_token->type == Token::Type::Escaped) {
                    _color = Color("#cb8296");
                } else if (current_token->type == Token::Type::Function) {
                    _color = Color("#d07617");
                } else if ((u32)current_token->type < (u32)Token::Type::String) {
                    _color = Color("#928372");
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
            current_token != lexer.tokens.data + lexer.tokens.length &&
            byte_offset == (current_token + 1)->index
        ) {
            current_token++;
        }
    }
}
