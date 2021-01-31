#include "lineedit.hpp"
#include "../app.hpp"

LineEdit::LineEdit(std::string text) : Widget() {
    Widget::m_bg = Color(1, 1, 1);
    this->setText(text);
    this->setPlaceholderText("");
    this->onMouseDown = [&](MouseEvent event) {
        this->m_mouse_event = true;
        if (!this->text().length()) {
            this->m_cursor_position = this->padding() + (this->borderWidth() / 2);
        } else {
            Rect local_rect = this->rect;
            if (!(m_virtual_size.w < rect.w)) {
                local_rect.x -= m_current_view * (m_virtual_size.w - rect.w);
            }
            DrawingContext *dc = ((Application*)this->app)->dc;
            float x = this->padding() + (this->borderWidth() / 2);
            size_t index = 0;
            for (char c : this->text()) {
                float w = dc->measureText(this->font() ? this->font() : dc->default_font, c).w;
                if (x + w > (local_rect.x * -1) + event.x) {
                    if (x + (w / 2) < (local_rect.x * -1) + event.x) {
                        x += w;
                        index++;
                    }
                    break;
                }
                x += w;
                index++;
            }
            if (!index) {
                m_current_view = m_min_view;
            } else if (index == this->text().size()) {
                m_current_view = m_max_view;
            } else {
                m_current_view = (x - m_padding - (m_border_width / 2)) / (m_virtual_size.w - m_padding - (m_border_width / 2));
            }
            this->m_cursor_position = x;
            this->m_cursor_index = index;
            this->selection.temp_x = x;
            this->selection.temp_index = index;
            
            this->selection.begin = index;
            this->selection.x_begin = x;
            this->selection.end = index;
            this->selection.x_end = x;
        }
    };
    this->onMouseMotion = [&](MouseEvent event) {
        if (isPressed()) {
            // TODO need to handle when the begin index is higher than end
            // we probably can just reverse them
            Rect local_rect = this->rect;
            if (!(m_virtual_size.w < rect.w)) {
                local_rect.x -= m_current_view * (m_virtual_size.w - rect.w);
            }
            DrawingContext *dc = ((Application*)this->app)->dc;
            float x = this->m_cursor_position;
            size_t index = this->m_cursor_index;
            for (;index < this->text().length();) {
                char c = this->text()[index];
                float w = dc->measureText(this->font() ? this->font() : dc->default_font, c).w;
                if (x + w > (local_rect.x * -1) + event.x) {
                    if (x + (w / 2) < (local_rect.x * -1) + event.x) {
                        x += w;
                        index++;
                    }
                    break;
                }
                x += w;
                index++;
            }
            if (!index) {
                m_current_view = m_min_view;
            } else if (index == this->text().size()) {
                m_current_view = m_max_view;
            } else {
                m_current_view = (x - m_padding - (m_border_width / 2)) / (m_virtual_size.w - m_padding - (m_border_width / 2));
            }

            // this->selection.begin = this->m_cursor_index;
            // this->selection.x_begin = this->m_cursor_position;
            this->selection.end = index;
            this->selection.x_end = x;

            this->m_cursor_position = x;
            this->m_cursor_index = index;
            update();
        }
    };
    this->onMouseUp = [&](MouseEvent event) {
        if (m_mouse_event) {
            this->m_mouse_event = false;
            this->selection.x_begin = this->selection.temp_x;
            this->selection.begin = this->selection.temp_index;
        }
    };
    this->onMouseEntered = [&](MouseEvent event) {
        ((Application*)this->app)->setMouseCursor(Cursor::IBeam);
    };
    this->onMouseLeft = [&](MouseEvent event) {
        ((Application*)this->app)->setMouseCursor(Cursor::Default);
        if (m_mouse_event) {
            this->m_mouse_event = false;
            this->selection.x_begin = this->selection.temp_x;
            this->selection.begin = this->selection.temp_index;
        }
    };
    this->bind(SDLK_LEFT, Mod::None, [&]{
        this->moveCursorLeft();
    });
    this->bind(SDLK_RIGHT, Mod::None, [&]{
        this->moveCursorRight();
    });
    this->bind(SDLK_HOME, Mod::None, [&]{
        this->moveCursorBegin();
    });
    this->bind(SDLK_END, Mod::None, [&]{
        this->moveCursorEnd();
    });
    this->bind(SDLK_BACKSPACE, Mod::None, [&]{
        if (m_cursor_index) {
            this->moveCursorLeft();
            this->deleteAt(m_cursor_index);
            this->updateView();
        }
    });
    this->bind(SDLK_DELETE, Mod::None, [&]{
        this->deleteAt(m_cursor_index);
        this->updateView();
    });
    this->bind(SDLK_LEFT, Mod::Ctrl, [&]{
        this->jumpWordLeft();
    });
    this->bind(SDLK_RIGHT, Mod::Ctrl, [&]{
        this->jumpWordRight();
    });
}

LineEdit::~LineEdit() {

}

const char* LineEdit::name() {
    return "LineEdit";
}

void LineEdit::draw(DrawingContext *dc, Rect rect) {
    this->rect = rect;

    Rect old_clip = dc->clip();
    dc->fillRect(
        rect, 
        m_fg
    );
    // resize rectangle to account for border
    rect.shrink(m_border_width);
    dc->setClip(rect);

    dc->fillRect(rect, this->background());
    if (!(m_virtual_size.w < rect.w)) {
        rect.x -= m_current_view * (m_virtual_size.w - rect.w);
    }
    // Draw placeholder text.
    if (!text().size()) {
        dc->fillTextAligned(
            this->font() ? this->font() : dc->default_font, 
            this->placeholderText(), 
            HorizontalAlignment::Left, 
            VerticalAlignment::Center, 
            rect,
            this->m_padding,
            Color(0.7, 0.7, 0.7)
        );
    // Draw normal text;
    } else {
        if (m_mouse_event) {
            float text_height = (float)(this->font() ? this->font()->max_height : dc->default_font->max_height);
            text_height += m_padding;
            dc->fillRect(
                Rect(
                    rect.x + selection.temp_x, 
                    rect.y + (rect.h / 2) - (text_height / 2), 
                    selection.x_end - selection.temp_x, 
                    text_height
                ),
                Color(0.2, 0.5, 1.0)
            );
        } else if (isFocused() && selection.hasSelection()) {
            float text_height = (float)(this->font() ? this->font()->max_height : dc->default_font->max_height);
            text_height += m_padding;
            dc->fillRect(
                Rect(
                    rect.x + selection.x_begin, 
                    rect.y + (rect.h / 2) - (text_height / 2), 
                    selection.x_end - selection.x_begin, 
                    text_height
                ),
                Color(0.2, 0.5, 1.0)
            );
        }
        dc->fillTextAligned(
            this->font() ? this->font() : dc->default_font, 
            this->text(), 
            HorizontalAlignment::Left, 
            VerticalAlignment::Center, 
            rect,
            this->m_padding,
            m_fg
        );
    }

    // Draw the text insertion cursor.
    if (this->isFocused()) {
        float text_height = (float)(this->font() ? this->font()->max_height : dc->default_font->max_height);
        text_height += m_padding;
        dc->fillRect(
            Rect(
                rect.x + this->m_cursor_position, 
                rect.y + (rect.h / 2) - (text_height / 2), 
                1, 
                text_height
            ), 
            m_fg
        );
    }
    dc->setClip(old_clip);
}

Size LineEdit::sizeHint(DrawingContext *dc) {
    if (m_text_changed) {
        m_virtual_size = dc->measureText(font() ? font() : dc->default_font, text());
        m_virtual_size.w += this->m_padding * 2 + this->m_border_width;
        m_virtual_size.h += this->m_padding * 2 + this->m_border_width;
    }
    if (this->m_size_changed) {
        Size size = Size(m_min_length, font() ? font()->max_height : dc->default_font->max_height);
        // Note: full padding is applied on both sides but
        // border width is divided in half for each side
        size.w += this->m_padding * 2 + this->m_border_width;
        size.h += this->m_padding * 2 + this->m_border_width;

        this->m_size = size;
        this->m_size_changed = false;

        return size;
    } else {
        return this->m_size;
    }
}

std::string LineEdit::text() {
    return this->m_text;
}

LineEdit* LineEdit::setText(std::string text) {
    this->m_text = text;
    this->m_text_changed = true;
    this->update();

    return this;
}

unsigned int LineEdit::borderWidth() {
    return this->m_border_width;
}

LineEdit* LineEdit::setBorderWidth(unsigned int border_width) {
    if (m_border_width != border_width) {
        m_border_width = border_width;
        this->m_size_changed = true;
        this->update();
        this->layout();
    }

    return this;
}

unsigned int LineEdit::padding() {
    return this->m_padding;
}

LineEdit* LineEdit::setPadding(unsigned int padding) {
    if (m_padding != padding) {
        m_padding = padding;
        this->m_size_changed = true;
        this->update();
        this->layout();
    }
    
    return this;
}

void LineEdit::handleTextEvent(DrawingContext *dc, const char *text) {
    m_text.insert(m_cursor_index, text);
    // TODO add our own insert? just a wrapper? so that it calls text_changed automatically
    m_text_changed = true;

    m_cursor_index += strlen(text);
    m_cursor_position += dc->measureText(font() ? font() : dc->default_font, text).w;
    if (m_cursor_index == this->text().size()) {
        m_current_view = m_max_view;
    } else {
        m_current_view = (m_cursor_position - m_padding - (m_border_width / 2)) / (m_virtual_size.w - m_padding - (m_border_width / 2));
    }
    update();
}

float LineEdit::minLength() {
    return m_min_length;
}

LineEdit* LineEdit::setMinLength(float length) {
    if (m_min_length != length) {
        m_min_length = length;
        m_size_changed = true;
        update();
        layout();
    }

    return this;
}

LineEdit* LineEdit::moveCursorLeft() {
    if (m_cursor_index && app) {
        DrawingContext *dc = ((Application*)this->app)->dc;
        m_cursor_index--;
        float char_size = dc->measureText(font() ? font() : dc->default_font, text()[m_cursor_index]).w;
        m_cursor_position -= char_size;
        if (!m_cursor_index) {
            m_current_view = m_min_view;
        } else {
            m_current_view = (m_cursor_position - m_padding - (m_border_width / 2)) / (m_virtual_size.w - m_padding - (m_border_width / 2));
        }
        update();
    }

    return this;
}

LineEdit* LineEdit::moveCursorRight() {
    if (m_cursor_index < text().size() && app) {
        DrawingContext *dc = ((Application*)this->app)->dc;
        float char_size = dc->measureText(font() ? font() : dc->default_font, text()[m_cursor_index]).w;
        m_cursor_position += char_size;
        m_cursor_index++;
        if (m_cursor_index == text().size()) {
            m_current_view = m_max_view;
        } else {
            m_current_view = (m_cursor_position - m_padding - (m_border_width / 2)) / (m_virtual_size.w - m_padding - (m_border_width / 2));
        }
        update();
    }

    return this;
}

LineEdit* LineEdit::moveCursorBegin() {
    m_cursor_position = this->padding() + (this->borderWidth() / 2);
    m_cursor_index = 0;
    m_current_view = m_min_view;
    update();
    
    return this;
}

LineEdit* LineEdit::moveCursorEnd() {
    m_cursor_position = m_virtual_size.w - (this->padding() + (this->borderWidth() / 2));
    m_cursor_index = text().size();
    m_current_view = m_max_view;
    update();
    
    return this;
}

LineEdit* LineEdit::deleteAt(int index) {
    if (index > -1 && index < text().size()) {
        m_text.erase(index, 1);
        m_text_changed = true;
        update();
    }

    return this;
}

LineEdit* LineEdit::clear() {
    m_text.clear();
    m_text_changed = true;
    m_cursor_index = 0;
    m_cursor_position = padding() + (borderWidth() / 2);
    m_current_view = m_min_view;
    update();

    return this;
}

LineEdit* LineEdit::setPlaceholderText(std::string text) {
    this->m_placeholder_text = text;
    this->update();

    return this;
}

std::string LineEdit::placeholderText() {
    return m_placeholder_text;
}

LineEdit* LineEdit::updateView() {
    if (!m_cursor_index) {
        m_current_view = m_min_view;
    } else if (m_cursor_index == text().size()) {
        m_current_view = m_max_view;
    } else {
        m_current_view = (m_cursor_position - m_padding - (m_border_width / 2)) / (m_virtual_size.w - m_padding - (m_border_width / 2));
    }
    update();
    return this;
}

LineEdit* LineEdit::jumpWordLeft() {
    while (m_cursor_index) {
        moveCursorLeft();
        if (text()[m_cursor_index] == ' ') {
            break;
        }
    }
    update();
    return this;
}

LineEdit* LineEdit::jumpWordRight() {
    while (m_cursor_index < text().size()) {
        moveCursorRight();
        if (text()[m_cursor_index] == ' ') {
            break;
        }
    }
    update();
    return this;
}
