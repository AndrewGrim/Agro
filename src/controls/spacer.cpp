#include "spacer.hpp"

Spacer::Spacer(Size min_size) : Widget() {
    m_size = min_size;
}

Spacer::~Spacer() {

}

const char* Spacer::name() {
    return "Spacer";
}

void Spacer::draw(DrawingContext &dc, Rect rect, i32 state) {
    this->rect = rect;
}

Size Spacer::sizeHint(DrawingContext &dc) {
    return m_size;
}

Size Spacer::minSize() {
    return m_size;
}

void Spacer::setMinSize(Size min_size) {
    m_size = min_size;
    layout(LAYOUT_STYLE);
}
