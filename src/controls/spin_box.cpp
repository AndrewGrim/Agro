#include "spin_box.hpp"

#define TEXT_HEIGHT ((i32)((font() ? font()->maxHeight() : dc.default_font->maxHeight()) + m_line_spacing))

SpinBoxIconButton::SpinBoxIconButton(Image *image) : IconButton(image) {}

SpinBoxIconButton::~SpinBoxIconButton() {}

const char* SpinBoxIconButton::name() {
    return "SpinBoxIconButton";
}

void SpinBoxIconButton::draw(DrawingContext &dc, Rect rect, i32 state) {
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

SpinBox::SpinBox(f64 value, i32 min_length) : TextEdit(std::to_string(value).data(), "", TextEdit::Mode::SingleLine, Size(min_length, 100)) {
    append(m_up_arrow);
    append(m_down_arrow);
    m_up_arrow->onMouseClick.addEventListener([&](Widget *button, MouseEvent event) {
        try {
            this->setText(std::to_string(std::stod(text().data()) + 1).data());
        } catch (std::invalid_argument &e) {
            onParsingError.notify(this, SpinBox::Error::InvalidArgument);
        } catch (std::out_of_range &e) {
            onParsingError.notify(this, SpinBox::Error::OutOfRange);
        }

    });
    m_down_arrow->onMouseClick.addEventListener([&](Widget *button, MouseEvent event) {
        try {
            this->setText(std::to_string(std::stod(text().data()) - 1).data());
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

void SpinBox::draw(DrawingContext &dc, Rect rect, i32 state) {
    Size arrow_button_size = m_up_arrow->sizeHint(dc);

    rect.w -= arrow_button_size.w;
    TextEdit::draw(dc, rect, state);
    // We need to set rect here because TextEdit overwrites it otherwise.
    this->rect = Rect(rect.x, rect.y, rect.w + arrow_button_size.w, rect.h);

    rect.x += rect.w;
    rect.w = arrow_button_size.w;
    m_up_arrow->draw(dc, Rect(rect.x, rect.y, rect.w, rect.h / 2), m_up_arrow->state());
    m_down_arrow->draw(dc, Rect(rect.x, rect.y + (rect.h / 2), rect.w, rect.h / 2), m_down_arrow->state());
}

Size SpinBox::sizeHint(DrawingContext &dc) {
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
        m_text_changed = false;
    }
    if (m_size_changed) {
        Size size;
        Size arrow_button_size = m_up_arrow->sizeHint(dc);
        if (m_mode == Mode::MultiLine) { size = m_viewport; }
        else { size = Size(m_viewport.w, TEXT_HEIGHT); }
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

Result<SpinBox::Error, f64> SpinBox::value() {
    try {
       return Result<SpinBox::Error, f64>(std::stod(text().data()));
    } catch (std::invalid_argument &e) {
        return Result<SpinBox::Error, f64>(SpinBox::Error::InvalidArgument);
    } catch (std::out_of_range &e) {
        return Result<SpinBox::Error, f64>(SpinBox::Error::OutOfRange);
    }
}
