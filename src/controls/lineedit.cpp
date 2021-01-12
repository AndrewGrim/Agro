#include "lineedit.hpp"
#include "../app.hpp"

LineEdit::LineEdit(std::string text) : Widget() {
    Widget::m_bg = Color(1, 1, 1);
    this->setText(text);
    this->onMouseDown = [&](MouseEvent event) {
        if (!this->text().length()) {
            this->m_cursor_position = this->padding() + (this->borderWidth() / 2);
        } else {
            this->m_last_mouse_x = event.x;
            this->m_process_mouse_event = true;
        }
    };
    this->bind(SDLK_LEFT, Mod::None, [&]{
        if (this->app) {
            DrawingContext *dc = ((Application*)this->app)->dc;
            this->moveCursorLeft(dc);
        }
    });
    this->bind(SDLK_RIGHT, Mod::None, [&]{
        if (this->app) {
            auto dc = ((Application*)this->app)->dc;
            this->moveCursorRight(dc);
        }
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
    dc->setClip(rect);
    dc->fillRect(
        rect, 
        m_fg
    );
    // resize rectangle to account for border
    rect = Rect(
        rect.x + this->m_border_width, 
        rect.y + this->m_border_width, 
        rect.w - this->m_border_width * 2, 
        rect.h - this->m_border_width * 2
    );

    dc->fillRect(rect, this->background());
    if (!(m_virtual_size.w < rect.w)) {
        rect.x -= m_current_view * (m_virtual_size.w - rect.w);
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

    if (this->m_process_mouse_event) {
        float x = this->padding() + (this->borderWidth() / 2);
        unsigned int index = 0;
        for (char c : this->text()) {
            float w = dc->measureText(this->font() ? this->font() : dc->default_font, c).w;
            if (x + w > (rect.x * -1) + this->m_last_mouse_x) {
                break;
            }
            x += w;
            index++;
        }
        if (!index) {
            m_current_view = m_min_view;
        } else if (index == text().size()) {
            m_current_view = m_max_view;
        } else {
            m_current_view = (x - m_padding - (m_border_width / 2)) / (m_virtual_size.w - m_padding - (m_border_width / 2));
        }
        this->m_cursor_position = x;
        this->m_cursor_index = index;
        this->m_process_mouse_event = false;
    }
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

uint LineEdit::borderWidth() {
    return this->m_border_width;
}

LineEdit* LineEdit::setBorderWidth(uint border_width) {
    if (m_border_width != border_width) {
        m_border_width = border_width;
        this->m_size_changed = true;
        this->update();
        this->layout();
    }

    return this;
}

uint LineEdit::padding() {
    return this->m_padding;
}

LineEdit* LineEdit::setPadding(uint padding) {
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

LineEdit* LineEdit::moveCursorLeft(DrawingContext *dc) {
    if (m_cursor_index) {
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

LineEdit* LineEdit::moveCursorRight(DrawingContext *dc) {
    if (m_cursor_index < text().size()) {
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