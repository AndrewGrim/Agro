#include "progress_bar.hpp"

ProgressBar::ProgressBar(i32 custom_width) : m_custom_width{custom_width} {}

ProgressBar::~ProgressBar() {}

const char* ProgressBar::name() {
    return "ProgressBar";
}

void ProgressBar::draw(DrawingContext &dc, Rect rect, i32 state) {
    dc.margin(rect, style());
    this->rect = rect;
    dc.drawBorder(rect, style(), state);
    i32 fill_end = rect.w * m_value;
    dc.fillRect(Rect(rect.x, rect.y, fill_end, rect.h), dc.accentWidgetBackground(style()));
    dc.padding(rect, style());
    fill_end = rect.w * m_value; // We have to recalculate the value again since we don't want to take the padding into account.

    String percent_text = toString((i32)(m_value * 100)) + "%";
    i32 text_width = dc.measureText(font(), percent_text).w;
    i32 text_start = (rect.w / 2) - (text_width / 2);
    u64 begin = 0;
    u64 end = 0;
    if (fill_end > text_start) {
        for (u8 c : percent_text) {
            i32 len = dc.measureText(font(), String::repeat(c, 1)).w;
            text_start += len;
            if (fill_end > text_start - len / 2) {
                end++;
            }
        }
    }

    dc.fillTextAligned(
        font(),
        percent_text,
        HorizontalAlignment::Center,
        VerticalAlignment::Center,
        rect,
        0,
        dc.textForeground(style()),
        0,
        Renderer::Selection(begin, end),
        dc.textSelected(style())
    );
}

Size ProgressBar::sizeHint(DrawingContext &dc) {
    if (m_size_changed) {
        Size s = dc.measureText(font(), "100%");
        dc.sizeHintMargin(s, style());
        dc.sizeHintBorder(s, style());
        dc.sizeHintPadding(s, style());
        if (m_custom_width > 0 && s.w < m_custom_width) { s.w = m_custom_width; }
        m_size = s;
        return s;
    }
    return m_size;
}
