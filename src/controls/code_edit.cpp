#include "code_edit.hpp"

#define TEXT_HEIGHT ((i32)((font()->maxHeight()) + m_line_spacing))
#define Y_SCROLL_OFFSET ((m_vertical_scrollbar->isVisible() ? m_vertical_scrollbar->m_slider->m_value : 0.0) * (m_virtual_size.h - inner_rect.h))

Lexer::Lexer() {}

void Lexer::lex(Slice<const char> source) {
    state = State::Default;
    pos = Position();
    this->source = source;
    tokens = Slice<Token>(new Token[source.length], 0);

    while (pos.index < source.length) {
        u8 next_c = source.data[pos.index];
        Token::Type token = (Token::Type)next_c;
        if (state == State::SingleLineComment) {
            if (token == Token::Type::Newline) {
                state = State::Default;
                // tokens.data[tokens.length++] = Token(Token::Type::SingleLineCommentEnd, pos.index - 1); // -1 to avoid including newline.
                // tokens.data[tokens.length++] = Token(Token::Type::Newline, pos.index);
                pos.newline();
            } else {
                pos.next();
            }
        } else if (state == State::MultiLineComment) {
            // TODO im not sure if C supports nested multiline comments
            if (token == Token::Type::Asterisk && peek(Token::Type::ForwardSlash)) {
                state = State::Default;
                // tokens.data[tokens.length++] = Token(Token::Type::MultiLineCommentEnd, pos.index);
                pos.advance(2);
            } else if (token == Token::Type::Newline) {
                tokens.data[tokens.length++] = Token(Token::Type::Newline, pos.index);
                pos.newline();
            } else {
                pos.next();
            }
        } else if (state == State::StringLiteral) {
            switch (token) {
                case Token::Type::BackwardSlash: {
                    if (pos.index + 1 < source.length) {
                        tokens.data[tokens.length++] = Token(Token::Type::Escaped, pos.index);
                        pos.advance(2);
                    }
                    break;
                }
                case Token::Type::DoubleQuotes: {
                    state = State::Default;
                    // tokens.data[tokens.length++] = Token(Token::Type::StringEnd, pos.index);
                    pos.next();
                    break;
                }
                case Token::Type::Newline: {
                    state = State::Default;
                    // tokens.data[tokens.length++] = Token(Token::Type::Newline, pos.index);
                    pos.newline();
                    break;
                }
                default:
                    pos.next();
            }
        } else if (state == State::CharacterLiteral) {
            switch (token) {
                case Token::Type::BackwardSlash: {
                    if (pos.index + 1 < source.length) {
                        tokens.data[tokens.length++] = Token(Token::Type::Escaped, pos.index);
                        pos.advance(2);
                    }
                    break;
                }
                case Token::Type::SingleQuote: {
                    state = State::Default;
                    tokens.data[tokens.length++] = Token(Token::Type::CharacterEnd, pos.index);
                    pos.next();
                    break;
                }
                case Token::Type::Newline: {
                    state = State::Default;
                    // tokens.data[tokens.length++] = Token(Token::Type::Newline, pos.index);
                    pos.newline();
                    break;
                }
                default:
                    pos.next();
            }
        } else if (state == State::Number) {
            // TODO number literals can have:
            // dots, F suffix for floats
            // hex and possibly octal and binary prefixes?
            // C23 ' as separator for easier reading
            if (!isdigit(next_c)) {
                state = State::Default;
                // tokens.data[tokens.length++] = Token(Token::Type::NumberEnd, pos.index - 1); // -1 to exclude end token from number.
            } else {
                pos.next();
            }
        } else if (state == State::Identifier) {
            if (!isalnum(next_c) && next_c != '_') {
                state = State::Default;
                String identifier = String(source.data + state_begin, pos.index - state_begin);
                auto entry = keywords.find(identifier);
                if (entry) {
                    tokens.data[tokens.length++] = Token(entry.value, state_begin);
                } else {
                    tokens.data[tokens.length++] = Token(Token::Type::Identifier, state_begin);
                }
            } else {
                pos.next();
            }
        } else if (state == State::PreProcessor) {
            switch (token) {
                case Token::Type::BackwardSlash: {
                    if (pos.index + 1 < source.length) {
                        tokens.data[tokens.length++] = Token(Token::Type::Escaped, pos.index);
                        pos.advance(2);
                    }
                    break;
                }
                case Token::Type::Newline: {
                    state = State::Default;
                    // tokens.data[tokens.length++] = Token(Token::Type::PreProcessorStatementEnd, pos.index - 1); // -1 to avoid including newline.
                    // tokens.data[tokens.length++] = Token(Token::Type::Newline, pos.index);
                    pos.newline();
                    break;
                }
                default: pos.next();
            }
        } else {
            switch (token) {
                case Token::Type::ForwardSlash: {
                    if (peek(Token::Type::ForwardSlash)) {
                        state = State::SingleLineComment;
                        tokens.data[tokens.length++] = Token(Token::Type::SingleLineCommentBegin, pos.index);
                        pos.advance(2);
                        break;
                    } else if (peek(Token::Type::Asterisk)) {
                        state = State::MultiLineComment;
                        tokens.data[tokens.length++] = Token(Token::Type::MultiLineCommentBegin, pos.index);
                        pos.advance(2);
                        break;
                    }
                    tokens.data[tokens.length++] = Token(token, pos.index);
                    pos.next();
                    break;
                }
                case Token::Type::DoubleQuotes: {
                    state = State::StringLiteral;
                    tokens.data[tokens.length++] = Token(Token::Type::StringBegin, pos.index);
                    pos.next();
                    break;
                }
                case Token::Type::SingleQuote: {
                    state = State::CharacterLiteral;
                    tokens.data[tokens.length++] = Token(Token::Type::CharacterBegin, pos.index);
                    pos.next();
                    break;
                }
                case Token::Type::Newline:
                    // tokens.data[tokens.length++] = Token(Token::Type::Newline, pos.index);
                    pos.newline();
                    break;
                case Token::Type::Pound: {
                    state = State::PreProcessor;
                    tokens.data[tokens.length++] = Token(Token::Type::PreProcessorStatementBegin, pos.index);
                    pos.next();
                    break;
                }
                default:
                    if (isalpha(next_c) || next_c == '_') {
                        state = State::Identifier;
                        state_begin = pos.index;
                    } else if (isdigit(next_c)) {
                        state = State::Number;
                        tokens.data[tokens.length++] = Token(Token::Type::NumberBegin, pos.index);
                    } else {
                        tokens.data[tokens.length++] = Token(token, pos.index);
                    }
                    pos.next();
            }
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

            // TODO last line is weird and consists of only tofus?
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
        if (c == ' ') {
            x += space_advance;
        } else if (c == '\t') {
            x += space_advance * m_tab_width;
        } else {
            Font::Character ch = font->get(utf8::decode(text.data + i, length));
            if (x > renderer.clip_rect.x + renderer.clip_rect.w) { return; }
            auto _color = color;
            if (!current_token) { _color = color; }
            else {
                if (
                    (u32)current_token->type > (u32)Token::Type::Identifier &&
                    (u32)current_token->type <= (u32)Token::Type::ThreadLocal
                ) {
                    _color = Color("#e44533");
                } else if (current_token->type == Token::Type::MultiLineCommentBegin) {
                    _color = Color("#928372");
                } else if (current_token->type == Token::Type::SingleLineCommentBegin) {
                    _color = Color("#928372");
                } else if (current_token->type == Token::Type::StringBegin) {
                    _color = Color("#b8bb26");
                } else if (current_token->type == Token::Type::CharacterBegin) {
                    _color = Color("#b8bb26");
                } else if (current_token->type == Token::Type::PreProcessorStatementBegin) {
                    _color = Color("#428372");
                } else if (current_token->type == Token::Type::NumberBegin) {
                    _color = Color("#cb8296");
                } else if (current_token->type == Token::Type::Identifier) {
                    _color = Color("#81ac71");
                } else if (current_token->type == Token::Type::Escaped) {
                    _color = Color("#cb8296");
                } else if ((u32)current_token->type < (u32)Token::Type::StringBegin) {
                    _color = Color("#928372");
                } else {
                    _color = color;
                }
            }
            if (selection.begin != selection.end && (i >= selection.begin && i < selection.end)) { _color = selection_color; }
            if (x + ch.advance >= renderer.clip_rect.x && x <= renderer.clip_rect.x + renderer.clip_rect.w) {
                f32 xpos = x + ch.bearing.x;
                f32 ypos = point.y + (base_bearing - ch.bearing.y);

                f32 w = ch.size.w;
                f32 h = ch.size.h;

                // TOP LEFT
                renderer.vertices[renderer.index++] = {
                    {xpos, ypos + h},
                    {ch.texture_x, (h / font->atlas_height)},
                    {_color.r / 255.0f, _color.g / 255.0f, _color.b / 255.0f, _color.a / 255.0f},
                    (f32)AGRO_OPENGL_RESERVED_FOR_TEXTURE_ARRAY,
                    ch.texture_array_index,
                    (f32)Renderer::Sampler::Text,
                    {1.0f, 1.0f, 1.0f, 1.0f},
                    {(f32)renderer.clip_rect.x, (f32)renderer.clip_rect.y, (f32)renderer.clip_rect.w, (f32)renderer.clip_rect.h}
                };
                // BOTTOM LEFT
                renderer.vertices[renderer.index++] = {
                    {xpos, ypos},
                    {ch.texture_x, 0.0f},
                    {_color.r / 255.0f, _color.g / 255.0f, _color.b / 255.0f, _color.a / 255.0f},
                    (f32)AGRO_OPENGL_RESERVED_FOR_TEXTURE_ARRAY,
                    ch.texture_array_index,
                    (f32)Renderer::Sampler::Text,
                    {1.0f, 1.0f, 1.0f, 1.0f},
                    {(f32)renderer.clip_rect.x, (f32)renderer.clip_rect.y, (f32)renderer.clip_rect.w, (f32)renderer.clip_rect.h}
                };
                // BOTTOM RIGHT
                renderer.vertices[renderer.index++] = {
                    {xpos + w, ypos},
                    {ch.texture_x + (w / font->atlas_width), 0.0f},
                    {_color.r / 255.0f, _color.g / 255.0f, _color.b / 255.0f, _color.a / 255.0f},
                    (f32)AGRO_OPENGL_RESERVED_FOR_TEXTURE_ARRAY,
                    ch.texture_array_index,
                    (f32)Renderer::Sampler::Text,
                    {1.0f, 1.0f, 1.0f, 1.0f},
                    {(f32)renderer.clip_rect.x, (f32)renderer.clip_rect.y, (f32)renderer.clip_rect.w, (f32)renderer.clip_rect.h}
                };
                // TOP RIGHT
                renderer.vertices[renderer.index++] = {
                    {xpos + w, ypos + h},
                    {ch.texture_x + (w / font->atlas_width), (h / font->atlas_height)},
                    {_color.r / 255.0f, _color.g / 255.0f, _color.b / 255.0f, _color.a / 255.0f},
                    (f32)AGRO_OPENGL_RESERVED_FOR_TEXTURE_ARRAY,
                    ch.texture_array_index,
                    (f32)Renderer::Sampler::Text,
                    {1.0f, 1.0f, 1.0f, 1.0f},
                    {(f32)renderer.clip_rect.x, (f32)renderer.clip_rect.y, (f32)renderer.clip_rect.w, (f32)renderer.clip_rect.h}
                };
                renderer.quad_count++;
            }
            x += ch.advance;
        }
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
