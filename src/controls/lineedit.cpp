#include "lineedit.hpp"
#include "../app.hpp"

LineEdit::LineEdit(std::string text) : Widget() {
    Widget::m_bg = Color(1, 1, 1);
    this->setText(text);
    this->setPlaceholderText("");
    this->onMouseDown = [&](MouseEvent event) {
        this->selection.mouse_selection = true;
        if (!this->text().length()) {
            this->selection.x_begin = this->padding() + (this->borderWidth() / 2);
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
            this->selection.x_begin = x;
            this->selection.begin = index;
            this->selection.x_end = x;
            this->selection.end = index;
        }
    };
    this->onMouseMotion = [&](MouseEvent event) {
        if (isPressed()) {
            // No x mouse movement.
            if (event.xrel == 0) {
                return;
            }

            Rect local_rect = this->rect;
            if (!(m_virtual_size.w < rect.w)) {
                local_rect.x -= m_current_view * (m_virtual_size.w - rect.w);
            }
            DrawingContext *dc = ((Application*)this->app)->dc;

            float x = selection.x_begin;
            size_t index = selection.begin;

            // Selection is to the right of the origin point.
            if (event.x >= x) {
                while (index < this->text().length()) {
                    char c = this->text()[index];
                    float w = dc->measureText(this->font() ? this->font() : dc->default_font, c).w;
                    if (x + w > (local_rect.x * -1) + event.x) {
                        break;
                    }
                    x += w;
                    index++;
                }
            // Selection is to the left of the origin point.
            } else {
                while (index) {
                    char c = this->text()[--index];
                    float w = dc->measureText(this->font() ? this->font() : dc->default_font, c).w;
                    x -= w;
                    if (x < (local_rect.x * -1) + event.x) {
                        break;
                    }
                }
            }
            
            if (!index) {
                m_current_view = m_min_view;
            } else if (index == this->text().size()) {
                m_current_view = m_max_view;
            } else {
                m_current_view = (x - m_padding - (m_border_width / 2)) / (m_virtual_size.w - m_padding - (m_border_width / 2));
            }
            this->selection.x_end = x;
            this->selection.end = index;
            update();
        }
    };
    this->onMouseUp = [&](MouseEvent event) {
        this->selection.mouse_selection = false;
    };
    this->onMouseEntered = [&](MouseEvent event) {
        ((Application*)this->app)->setMouseCursor(Cursor::IBeam);
    };
    this->onMouseLeft = [&](MouseEvent event) {
        ((Application*)this->app)->setMouseCursor(Cursor::Default);
        this->selection.mouse_selection = false;
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
        if (selection.hasSelection()) {
            this->deleteSelection();
        } else if (selection.begin) {
            this->moveCursorLeft();
            this->deleteAt(selection.begin);
        }
        this->updateView();
    });
    this->bind(SDLK_DELETE, Mod::None, [&]{
        if (selection.hasSelection()) {
            this->deleteSelection();
        } else {
            this->deleteAt(selection.begin);
        }
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
        if (selection.mouse_selection) {
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
                rect.x + this->selection.x_end, 
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
    if (selection.hasSelection()) {
        deleteSelection();
    }
    m_text.insert(selection.end, text);
    // TODO add our own insert? just a wrapper? so that it calls text_changed automatically
    m_text_changed = true;

    selection.x_end += dc->measureText(font() ? font() : dc->default_font, text).w;
    selection.end += strlen(text);
    selection.x_begin = selection.x_end;
    selection.begin = selection.end;
    if (selection.end == this->text().size()) {
        m_current_view = m_max_view;
    } else {
        m_current_view = (selection.x_end - m_padding - (m_border_width / 2)) / (m_virtual_size.w - m_padding - (m_border_width / 2));
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
    if (selection.end && app) {
        DrawingContext *dc = ((Application*)this->app)->dc;
        selection.end--;
        float char_size = dc->measureText(font() ? font() : dc->default_font, text()[selection.end]).w;
        selection.x_end -= char_size;
        if (!selection.end) {
            m_current_view = m_min_view;
        } else {
            m_current_view = (selection.x_end - m_padding - (m_border_width / 2)) / (m_virtual_size.w - m_padding - (m_border_width / 2));
        }
        update();
    }

    return this;
}

LineEdit* LineEdit::moveCursorRight() {
    if (selection.end < text().size() && app) {
        DrawingContext *dc = ((Application*)this->app)->dc;
        float char_size = dc->measureText(font() ? font() : dc->default_font, text()[selection.end]).w;
        selection.x_end += char_size;
        selection.end++;
        if (selection.end == text().size()) {
            m_current_view = m_max_view;
        } else {
            m_current_view = (selection.x_end - m_padding - (m_border_width / 2)) / (m_virtual_size.w - m_padding - (m_border_width / 2));
        }
        update();
    }

    return this;
}

LineEdit* LineEdit::moveCursorBegin() {
    selection.x_end = this->padding() + (this->borderWidth() / 2);
    selection.end = 0;
    m_current_view = m_min_view;
    update();
    
    return this;
}

LineEdit* LineEdit::moveCursorEnd() {
    selection.x_end = m_virtual_size.w - (this->padding() + (this->borderWidth() / 2));
    selection.end = text().size();
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
    selection.x_begin = padding() + (borderWidth() / 2);
    selection.begin = 0;
    selection.x_end = selection.x_begin;
    selection.end = selection.begin;
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
    if (!selection.begin) {
        m_current_view = m_min_view;
    } else if (selection.begin == text().size()) {
        m_current_view = m_max_view;
    } else {
        m_current_view = (selection.x_begin - m_padding - (m_border_width / 2)) / (m_virtual_size.w - m_padding - (m_border_width / 2));
    }
    update();
    return this;
}

LineEdit* LineEdit::jumpWordLeft() {
    while (selection.end) {
        moveCursorLeft();
        if (text()[selection.end] == ' ') {
            break;
        }
    }
    update();
    return this;
}

LineEdit* LineEdit::jumpWordRight() {
    while (selection.end < text().size()) {
        moveCursorRight();
        if (text()[selection.end] == ' ') {
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

void LineEdit::deleteSelection() {
    // Swap selection when the begin index is higher than the end index.
    if (selection.begin > selection.end) {
        float temp_x = selection.x_end;
        size_t temp = selection.end;
        selection.x_end = selection.x_begin;
        selection.end = selection.begin;
        selection.x_begin = temp_x;
        selection.begin = temp;
    }
    // Remove selected text.
    this->setText(this->text().erase(selection.begin, selection.end - selection.begin));

    // Reset selection after deletion.
    selection.x_end = selection.x_begin;
    selection.end = selection.begin;
}