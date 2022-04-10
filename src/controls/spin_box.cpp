#include "spin_box.hpp"

SpinBoxIconButton::SpinBoxIconButton(Image *image) : IconButton(image) {}

SpinBoxIconButton::~SpinBoxIconButton() {}

const char* SpinBoxIconButton::name() {
    return "SpinBoxIconButton";
}

void SpinBoxIconButton::draw(DrawingContext &dc, Rect rect, int state) {
    Color color;
    if (state & STATE_PRESSED && state & STATE_HOVERED) {
        color = dc.accentPressedBackground(style);
    } else if (state & STATE_HOVERED) {
        color = dc.accentHoveredBackground(style);
    } else {
        color = dc.accentWidgetBackground(style);
    }

    dc.margin(rect, style);
    this->rect = rect;
    dc.drawBorder(rect, style, state);
    dc.fillRect(rect, color);
    dc.padding(rect, style);

    dc.drawTextureAligned(
        rect, m_image->size(),
        m_image->_texture(), m_image->coords(),
        HorizontalAlignment::Center, VerticalAlignment::Center,
        dc.textSelected(style)
    );
}

SpinBox::SpinBox(double value, int min_length) : LineEdit(std::to_string(value), "", min_length) {
    append(m_up_arrow);
    append(m_down_arrow);
    m_up_arrow->onMouseClick.addEventListener([&](Widget *button, MouseEvent event) {
        try {
            this->setText(std::to_string(std::stod(text()) + 1));
        } catch (std::invalid_argument &e) {
            onParsingError.notify(this, SpinBox::Error::InvalidArgument);
        } catch (std::out_of_range &e) {
            onParsingError.notify(this, SpinBox::Error::OutOfRange);
        }

    });
    m_down_arrow->onMouseClick.addEventListener([&](Widget *button, MouseEvent event) {
        try {
            this->setText(std::to_string(std::stod(text()) - 1));
        } catch (std::invalid_argument &e) {
            onParsingError.notify(this, SpinBox::Error::InvalidArgument);
        } catch (std::out_of_range &e) {
            onParsingError.notify(this, SpinBox::Error::OutOfRange);
        }
    });
    m_up_arrow->style.margin.type = STYLE_NONE;
    m_up_arrow->style.border.type = STYLE_TOP|STYLE_RIGHT;
    m_down_arrow->style.margin.type = STYLE_NONE;
    m_down_arrow->style.border.type = STYLE_BOTTOM|STYLE_RIGHT;
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
        Size size = Size(m_min_length, font() ? font()->maxHeight() : dc.default_font->maxHeight());
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

bool SpinBox::isLayout() {
    return true;
}

Result<SpinBox::Error, double> SpinBox::value() {
    try {
       return Result<SpinBox::Error, double>(std::stod(text()));
    } catch (std::invalid_argument &e) {
        return Result<SpinBox::Error, double>(SpinBox::Error::InvalidArgument);
    } catch (std::out_of_range &e) {
        return Result<SpinBox::Error, double>(SpinBox::Error::OutOfRange);
    }
}
