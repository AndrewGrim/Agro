#include "progress_bar.hpp"

ProgressBar::ProgressBar(int custom_width) : m_custom_width{custom_width} {}

ProgressBar::~ProgressBar() {}

const char* ProgressBar::name() {
    return "ProgressBar";
}

void ProgressBar::draw(DrawingContext &dc, Rect rect, int state) {
    dc.margin(rect, style);
    this->rect = rect;
    dc.drawBorder(rect, style);
    int fill_end = rect.w * m_value;
    dc.fillRect(Rect(rect.x, rect.y, fill_end, rect.h), dc.accentWidgetBackground(style));
    dc.padding(rect, style);
    fill_end = rect.w * m_value; // We have to recalculate the value again since we don't want to take the padding into account.

    std::string percent_text = std::to_string((int)(m_value * 100)) + "%";
    int text_width = dc.measureText(font(), percent_text).w;
    int text_start = (rect.w / 2) - (text_width / 2);
    size_t begin = 0;
    size_t end = 0;
    if (fill_end > text_start) {
        for (char c : percent_text) {
            int len = dc.measureText(font(), c).w;
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
        dc.textForeground(style),
        0,
        Renderer::Selection(begin, end),
        dc.textSelected(style)
    );
}

Size ProgressBar::sizeHint(DrawingContext &dc) {
    if (m_size_changed) {
        Size s = dc.measureText(font(), "100%");
        dc.sizeHintMargin(s, style);
        dc.sizeHintBorder(s, style);
        dc.sizeHintPadding(s, style);
        if (m_custom_width > 0 && s.w < m_custom_width) { s.w = m_custom_width; }
        m_size = s;
        return s;
    }
    return m_size;
}
