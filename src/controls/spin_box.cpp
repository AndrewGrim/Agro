#include "spin_box.hpp"

SpinBox::SpinBox(int value, int min_length) : LineEdit(std::to_string(value), "", min_length) {
    append(m_up_arrow);
    append(m_down_arrow);
    // TODO we will want to guard against overflowing the storage number type
    m_up_arrow->onMouseClick.addEventListener([&](Widget *button, MouseEvent event) {
        if (this->text().size()) {
            this->setText(std::to_string(std::stoi(this->text()) + 1));
        } else {
            this->setText(std::to_string(0 + 1));
        }
    });
    // TODO we will want to guard against overflowing the storage number type
    m_down_arrow->onMouseClick.addEventListener([&](Widget *button, MouseEvent event) {
        if (this->text().size()) {
            this->setText(std::to_string(std::stoi(this->text()) - 1));
        } else {
            this->setText(std::to_string(0 - 1));
        }
    });
    // m_up_arrow->style.widget_background = Application::get()->dc->accentWidgetBackground(style);
    // m_down_arrow->style.widget_background = Application::get()->dc->accentWidgetBackground(style);
    m_up_arrow->style.margin.type = STYLE_NONE;
    m_down_arrow->style.margin.type = STYLE_NONE;
    style.margin.type = STYLE_NONE;
}

SpinBox::~SpinBox() {}

const char* SpinBox::name() {
    return "SpinBox";
}

void SpinBox::draw(DrawingContext &dc, Rect rect, int state) {
    Size arrow_button_size = m_up_arrow->sizeHint(dc);

    rect.w -= arrow_button_size.w;
    LineEdit::draw(dc, rect, state);
    // We need to set rect here because LineEdit overwrites it otherwise.
    this->rect = Rect(rect.x, rect.y, rect.w + arrow_button_size.w, rect.h);

    rect.x += rect.w;
    rect.w = arrow_button_size.w;
    m_up_arrow->draw(dc, Rect(rect.x, rect.y, rect.w, rect.h / 2), m_up_arrow->state());
    m_down_arrow->draw(dc, Rect(rect.x, rect.y + (rect.h / 2), rect.w, rect.h / 2), m_down_arrow->state());
}

Size SpinBox::sizeHint(DrawingContext &dc) {
    if (m_text_changed) {
        m_virtual_size = dc.measureText(font(), text());
        m_text_height = m_virtual_size.h;
    }
    if (m_size_changed) {
        Size size = Size(m_min_length, font() ? font()->max_height : dc.default_font->max_height);
        Size arrow_button_size = m_up_arrow->sizeHint(dc);

        dc.sizeHintMargin(size, style);
        dc.sizeHintBorder(size, style);
        dc.sizeHintPadding(size, style);

        if (size.h <= arrow_button_size.h) {
            size.h = arrow_button_size.h * 2;
        }
        size.w += arrow_button_size.w;

        m_size = size;
        m_size_changed = false;

        return size;
    } else {
        return m_size;
    }
}

bool isNumber(char c) {
    if (c >= 48 && c <= 57) { return true; }
    return false;
}

void SpinBox::handleTextEvent(DrawingContext &dc, const char *text) {
    if (text && isNumber(text[0])) {
        insert(m_selection.end, text);
    }
}

bool SpinBox::isLayout() {
    return true;
}
