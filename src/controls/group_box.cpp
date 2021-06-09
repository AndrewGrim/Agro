#include "group_box.hpp"

GroupBox::GroupBox(Align align_policy, std::string label) : Box(align_policy), label{label} {
    style.margin.top = 5;
    style.margin.bottom = 5;
    style.margin.left = 5;
    style.margin.right = 5;
}

GroupBox::~GroupBox() {}

const char* GroupBox::name() {
    return "GroupBox";
}

void GroupBox::draw(DrawingContext &dc, Rect rect, int state) {
    dc.margin(rect, style);
    this->rect = rect;
    Point point;
    Size text_size;
    float text_size_half = 0.0f;
    if (label.length()) {
        point = Point(rect.x + 10, rect.y); // 10 padding from the left
        text_size = dc.measureText(font(), label);
        text_size_half = text_size.h / 2;
        rect.y += text_size_half;
        rect.h -= text_size_half;
    }
    dc.drawBorder(rect, style);
    rect.y += text_size_half;
    rect.h -= text_size_half;
    dc.fillRect(rect, dc.windowBackground(style));
    dc.padding(rect, style);
    if (label.length()) {
        dc.fillRect(Rect(point.x, point.y + text_size_half, text_size.w + 2, text_size_half), dc.windowBackground(style)); // 2 padding on the right so the text doesnt touch the border
        dc.fillText(font(), label, point, dc.textForeground(style));
    }
    layoutChildren(dc, rect);
}

Size GroupBox::sizeHint(DrawingContext &dc) {
    unsigned int visible = 0;
    unsigned int vertical_non_expandable = 0;
    unsigned int horizontal_non_expandable = 0;
    if (m_size_changed) {
        Size size = Size();
        if (m_align_policy == Align::Horizontal) {
            for (Widget* child : children) {
                if (child->isVisible()) {
                    Size s = child->sizeHint(dc);
                    size.w += s.w;
                    if (s.h > size.h) {
                        size.h = s.h;
                    }
                    visible += child->proportion();
                    if (child->fillPolicy() == Fill::Vertical || child->fillPolicy() == Fill::None) {
                        horizontal_non_expandable++;
                    }
                }
            }
        } else {
            for (Widget* child : children) {
                if (child->isVisible()) {
                    Size s = child->sizeHint(dc);
                    size.h += s.h;
                    if (s.w > size.w) {
                        size.w = s.w;
                    }
                    visible += child->proportion();
                    if (child->fillPolicy() == Fill::Horizontal || child->fillPolicy() == Fill::None) {
                        vertical_non_expandable++;
                    }
                }
            }
        }
        dc.sizeHintMargin(size, style);
        dc.sizeHintBorder(size, style);
        dc.sizeHintPadding(size, style);

        Size text_size = dc.measureText(font(), label);
        text_size.w += 12; // 10 padding on the left, 2 padding on the right
        if (size.w < text_size.w) { size.w = text_size.w; }
        size.h += text_size.h;

        m_vertical_non_expandable = vertical_non_expandable;
        m_horizontal_non_expandable = horizontal_non_expandable;
        m_visible_children = visible;
        m_size = size;
        m_size_changed = false;

        return size;
    } else {
        return m_size;
    }
}
