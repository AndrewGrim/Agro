#include "lineedit.hpp"
#include "../app.hpp"

LineEdit::LineEdit(std::string text) : Widget() {
    Widget::m_bg = Color(1, 1, 1);
    this->setText(text);
    this->setPlaceholderText("");
    this->m_selection.x_begin = padding() + (borderWidth() / 2);
    this->m_selection.x_end = padding() + (borderWidth() / 2);
    this->onMouseDown.addEventListener([&](Widget *widget, MouseEvent event) {
        this->m_selection.mouse_selection = true;
        if (!this->text().length()) {
            this->m_selection.x_begin = this->padding() + (this->borderWidth() / 2);
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
            this->m_selection.x_begin = x;
            this->m_selection.begin = index;
            this->m_selection.x_end = x;
            this->m_selection.end = index;
        }
    });
    this->onMouseMotion.addEventListener([&](Widget *widget, MouseEvent event) {
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

            float x = m_selection.x_begin;
            size_t index = m_selection.begin;

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
            this->m_selection.x_end = x;
            this->m_selection.end = index;
            update();
        }
    });
    this->onMouseUp.addEventListener([&](Widget *widget, MouseEvent event) {
        this->m_selection.mouse_selection = false;
    });
    this->onMouseEntered.addEventListener([&](Widget *widget, MouseEvent event) {
        ((Application*)this->app)->setMouseCursor(Cursor::IBeam);
    });
    this->onMouseLeft.addEventListener([&](Widget *widget, MouseEvent event) {
        ((Application*)this->app)->setMouseCursor(Cursor::Default);
        this->m_selection.mouse_selection = false;
    });
    auto left = [&]{
        this->moveCursorLeft();
    };
    this->bind(SDLK_LEFT, Mod::None, left);
    this->bind(SDLK_LEFT, Mod::Shift, left);
    auto right = [&]{
        this->moveCursorRight();
    };
    this->bind(SDLK_RIGHT, Mod::None, right);
    this->bind(SDLK_RIGHT, Mod::Shift, right);
    auto home = [&]{
        this->moveCursorBegin();
    };
    this->bind(SDLK_HOME, Mod::None, home);
    this->bind(SDLK_HOME, Mod::Shift, home);
    auto end = [&]{
        this->moveCursorEnd();
    };
    this->bind(SDLK_END, Mod::None, end);
    this->bind(SDLK_END, Mod::Shift, end);
    this->bind(SDLK_BACKSPACE, Mod::None, [&]{
        if (m_selection.hasSelection()) {
            this->deleteSelection();
        } else if (m_selection.begin) {
            this->moveCursorLeft();
            this->deleteAt(m_selection.begin);
        }
        this->updateView();
    });
    this->bind(SDLK_DELETE, Mod::None, [&]{
        if (m_selection.hasSelection()) {
            this->deleteSelection();
        } else {
            this->deleteAt(m_selection.begin);
        }
        this->updateView();
    });
    auto jump_left = [&]{
        this->jumpWordLeft();
    };
    this->bind(SDLK_LEFT, Mod::Ctrl, jump_left);
    this->bind(SDLK_LEFT, Mod::Ctrl|Mod::Shift, jump_left);
    auto jump_right = [&]{
        this->jumpWordRight();
    };
    this->bind(SDLK_RIGHT, Mod::Ctrl, jump_right);
    this->bind(SDLK_RIGHT, Mod::Ctrl|Mod::Shift, jump_right);
    this->bind(SDLK_a, Mod::Ctrl, [&]{
        this->selectAll();
    });
    this->bind(SDLK_v, Mod::Ctrl, [&]{
        if (SDL_HasClipboardText()) {
            char *s = SDL_GetClipboardText();
            if (s) {
                this->insert(m_selection.end, s);
                SDL_free(s);
            }
        }
    });
    this->bind(SDLK_c, Mod::Ctrl, [&]{
        if (m_selection.hasSelection()) {
            swapSelection();
            std::string s = this->text().substr(m_selection.begin, m_selection.end);
            SDL_SetClipboardText(s.c_str());
        }
    });
    this->bind(SDLK_z, Mod::Ctrl, [&]{
        undo();
    });
    this->bind(SDLK_y, Mod::Ctrl, [&]{
        redo();
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
        if (m_selection.mouse_selection) {
            float text_height = (float)(this->font() ? this->font()->max_height : dc->default_font->max_height);
            text_height += m_padding;
            dc->fillRect(
                Rect(
                    rect.x + m_selection.x_begin, 
                    rect.y + (rect.h / 2) - (text_height / 2), 
                    m_selection.x_end - m_selection.x_begin, 
                    text_height
                ),
                Color(0.2, 0.5, 1.0)
            );
        } else if (isFocused() && m_selection.hasSelection()) {
            float text_height = (float)(this->font() ? this->font()->max_height : dc->default_font->max_height);
            text_height += m_padding;
            dc->fillRect(
                Rect(
                    rect.x + m_selection.x_begin, 
                    rect.y + (rect.h / 2) - (text_height / 2), 
                    m_selection.x_end - m_selection.x_begin, 
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
                rect.x + this->m_selection.x_end, 
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
        this->layout();
    }
    
    return this;
}

void LineEdit::handleTextEvent(DrawingContext *dc, const char *text) {
    insert(m_selection.end, text);
}

float LineEdit::minLength() {
    return m_min_length;
}

LineEdit* LineEdit::setMinLength(float length) {
    if (m_min_length != length) {
        m_min_length = length;
        layout();
    }

    return this;
}

LineEdit* LineEdit::moveCursorLeft() {
    if (m_selection.end == 0) {
       if (m_selection.hasSelection()) {
            swapSelection();
            m_selection.x_end = m_selection.x_begin;
            m_selection.end = m_selection.begin;
        }
    } else if (m_selection.end && app) {
        if (m_selection.hasSelection() && !isShiftPressed()) {
            swapSelection();
            m_selection.x_end = m_selection.x_begin;
            m_selection.end = m_selection.begin;
            goto END;
        }
        DrawingContext *dc = ((Application*)this->app)->dc;
        m_selection.end--;
        float char_size = dc->measureText(font() ? font() : dc->default_font, text()[m_selection.end]).w;
        m_selection.x_end -= char_size;
        if (!isShiftPressed()) {
            m_selection.x_begin = m_selection.x_end;
            m_selection.begin = m_selection.end;
        }
    }
    END:;
    if (!m_selection.end) {
        m_current_view = m_min_view;
    } else {
        m_current_view = (m_selection.x_end - m_padding - (m_border_width / 2)) / (m_virtual_size.w - m_padding - (m_border_width / 2));
    }
    update();

    return this;
}

LineEdit* LineEdit::moveCursorRight() {
    if (m_selection.end == text().size()) {
       if (m_selection.hasSelection()) {
            m_selection.x_begin = m_selection.x_end;
            m_selection.begin = m_selection.end;
        }
    } else if (m_selection.end < text().size() && app) {
        if (m_selection.hasSelection() && !isShiftPressed()) {
            swapSelection();
            m_selection.x_begin = m_selection.x_end;
            m_selection.begin = m_selection.end;
            goto END;
        }
        DrawingContext *dc = ((Application*)this->app)->dc;
        float char_size = dc->measureText(font() ? font() : dc->default_font, text()[m_selection.end]).w;
        m_selection.x_end += char_size;
        m_selection.end++;
        if (!isShiftPressed()) {
            m_selection.x_begin = m_selection.x_end;
            m_selection.begin = m_selection.end;
        }
    }
    END:;
    if (m_selection.end == text().size()) {
        m_current_view = m_max_view;
    } else {
        m_current_view = (m_selection.x_end - m_padding - (m_border_width / 2)) / (m_virtual_size.w - m_padding - (m_border_width / 2));
    }
    update();

    return this;
}

LineEdit* LineEdit::moveCursorBegin() {
    m_selection.x_end = this->padding() + (this->borderWidth() / 2);
    m_selection.end = 0;
    if (!isShiftPressed()) {
        m_selection.x_begin = m_selection.x_end;
        m_selection.begin = m_selection.end;
    }
    m_current_view = m_min_view;
    update();
    
    return this;
}

LineEdit* LineEdit::moveCursorEnd() {
    m_selection.x_end = m_virtual_size.w - (this->padding() + (this->borderWidth() / 2));
    m_selection.end = text().size();
    if (!isShiftPressed()) {
        m_selection.x_begin = m_selection.x_end;
        m_selection.begin = m_selection.end;
    }
    m_current_view = m_max_view;
    update();
    
    return this;
}

LineEdit* LineEdit::deleteAt(size_t index, bool skip) {
    if (index >= 0 && index < text().size()) {
        if (!skip) {
            m_history.append({HistoryItem::Action::Delete, std::string(1, m_text[index]), m_selection});
        }
        m_text.erase(index, 1);
        m_text_changed = true;
        update();
        if (onTextChanged) {
            onTextChanged();
        }
    }

    return this;
}

LineEdit* LineEdit::clear() {
    m_text.clear();
    m_text_changed = true;
    m_selection.x_begin = padding() + (borderWidth() / 2);
    m_selection.begin = 0;
    m_selection.x_end = m_selection.x_begin;
    m_selection.end = m_selection.begin;
    m_current_view = m_min_view;
    m_history = History();
    update();
    if (onTextChanged) {
        onTextChanged();
    }

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
    // TODO investigate at some point, it used to use begin
    // but the result is the same with begin and end
    // and it shouldnt be????
    // so changed it to end so its consistent with the rest of the file
    if (!m_selection.end) {
        m_current_view = m_min_view;
    } else if (m_selection.end == text().size()) {
        m_current_view = m_max_view;
    } else {
        m_current_view = (m_selection.x_end - m_padding - (m_border_width / 2)) / (m_virtual_size.w - m_padding - (m_border_width / 2));
    }
    update();
    return this;
}

LineEdit* LineEdit::jumpWordLeft() {
    while (m_selection.end) {
        moveCursorLeft();
        if (text()[m_selection.end] == ' ') {
            break;
        }
    }
    update();
    return this;
}

LineEdit* LineEdit::jumpWordRight() {
    while (m_selection.end < text().size()) {
        moveCursorRight();
        if (text()[m_selection.end] == ' ') {
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

void LineEdit::deleteSelection(bool skip) {
    // Swap selection when the begin index is higher than the end index.
    swapSelection();
    if (!skip) {
        m_history.append({HistoryItem::Action::Delete, m_text.substr(m_selection.begin, m_selection.end - m_selection.begin), m_selection});
    }
    // Remove selected text.
    this->setText(this->text().erase(m_selection.begin, m_selection.end - m_selection.begin));

    // Reset selection after deletion.
    m_selection.x_end = m_selection.x_begin;
    m_selection.end = m_selection.begin;
    if (onTextChanged) {
        onTextChanged();
    }
}

void LineEdit::selectAll() {
    m_selection.x_begin = this->padding() + (this->borderWidth() / 2);
    m_selection.begin = 0;
    m_selection.x_end = m_virtual_size.w - (this->padding() + (this->borderWidth() / 2));
    m_selection.end = text().size();
    m_current_view = m_max_view;
    update();
}

void LineEdit::swapSelection() {
    if (m_selection.begin > m_selection.end) {
        float temp_x = m_selection.x_end;
        size_t temp = m_selection.end;
        m_selection.x_end = m_selection.x_begin;
        m_selection.end = m_selection.begin;
        m_selection.x_begin = temp_x;
        m_selection.begin = temp;
    }
}

void LineEdit::insert(size_t index, const char *text, bool skip) {
    DrawingContext *dc = ((Application*)this->app)->dc;

    if (m_selection.hasSelection()) {
        deleteSelection(skip);
    }

    m_selection.x_end += dc->measureText(font() ? font() : dc->default_font, text).w;
    m_selection.end += strlen(text);
    if (!skip) {
        // Pass a fake selection for the new text.
        // This is needed for properly deleting it when calling undo.
        m_history.append({HistoryItem::Action::Insert, text, m_selection});
    }
    m_text.insert(m_selection.begin, text);
    m_text_changed = true;

    m_selection.x_begin = m_selection.x_end;
    m_selection.begin = m_selection.end;

    if (m_selection.end == this->text().size()) {
        m_current_view = m_max_view;
    } else {
        m_current_view = (m_selection.x_end - m_padding - (m_border_width / 2)) / (m_virtual_size.w - m_padding - (m_border_width / 2));
    }
    update();
    if (onTextChanged) {
        onTextChanged();
    }
}

void LineEdit::setCursor(size_t index) {
    if (!index) {
        this->m_selection.x_begin = this->padding() + (this->borderWidth() / 2);
        this->m_selection.begin = 0;
        this->m_selection.x_end = this->m_selection.x_begin;
        this->m_selection.end = 0;
    } else {
        Rect local_rect = this->rect;
        if (!(m_virtual_size.w < rect.w)) {
            local_rect.x -= m_current_view * (m_virtual_size.w - rect.w);
        }
        DrawingContext *dc = ((Application*)this->app)->dc;
        float x = this->padding() + (this->borderWidth() / 2);
        size_t local_index = 0;
        for (char c : this->text()) {
            float w = dc->measureText(this->font() ? this->font() : dc->default_font, c).w;
            x += w;
            local_index++;
            if (index == local_index) {
                break;
            }
        }
        this->m_selection.x_begin = x;
        this->m_selection.begin = local_index;
        this->m_selection.x_end = x;
        this->m_selection.end = local_index;
    }
    update();
}

void LineEdit::undo() {
    if (!m_history.undo_end) {
        HistoryItem item = m_history.get(m_history.index);
        m_selection = item.selection;
        if (item.action == HistoryItem::Action::Delete) {
            insert(m_selection.begin, item.text.c_str(), true);
        } else {
            if (m_selection.hasSelection()) {
                deleteSelection(true);
            } else {
                deleteAt(m_selection.begin, true);
            }
        }
        if (!m_history.index) {
            m_history.undo_end = true;
        } else {
            m_history.index--;
        }
        m_history.redo_end = false;
    }
}

void LineEdit::redo() {
    if (m_history.index < m_history.items.size() && !m_history.redo_end) {
        HistoryItem item = m_history.get(m_history.index);
        m_selection = item.selection;
        if (item.action == HistoryItem::Action::Delete) {
            if (m_selection.hasSelection()) {
                deleteSelection(true);
            } else {
                deleteAt(m_selection.begin, true);
            }
        } else {
            insert(m_selection.begin, item.text.c_str(), true);
        }
        if (!m_history.index) {
            m_history.undo_end = false;
        }
        if (m_history.index < m_history.items.size() - 1) {
            m_history.index++;
        } else {
            m_history.redo_end = true;
        }
    }
}