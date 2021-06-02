#include "spacer.hpp"

Spacer::Spacer(Size min_size) : Widget() {
    m_size = min_size;
    Widget::m_bg = COLOR_NONE;
}

Spacer::~Spacer() {

}

const char* Spacer::name() {
    return "Spacer";
}

void Spacer::draw(DrawingContext &dc, Rect rect, int state) {
    this->rect = rect;
    dc.fillRect(rect, m_bg);
}

Size Spacer::sizeHint(DrawingContext &dc) {
    return m_size;
}

Size Spacer::minSize() {
    return m_size;
}

void Spacer::setMinSize(Size min_size) {
    m_size = min_size;
    layout();
}
