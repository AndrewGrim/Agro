#include "box.hpp"
#include "../common/point.hpp"
#include "../application.hpp"

Box::Box(Align align_policy) : m_align_policy{align_policy} {}

Box::~Box() {}

void Box::draw(DrawingContext &dc, Rect rect, int state) {
    this->rect = rect;
    dc.fillRect(rect, dc.windowBackground(style));
    layoutChildren(dc, rect);
}

void Box::layoutChildren(DrawingContext &dc, Rect rect) {
    sizeHint(dc);
    Align parent_layout = alignPolicy();
    int generic_non_expandable_widgets;
    Point pos = Point(rect.x, rect.y);
    int generic_total_layout_length;
    int *generic_position_coord; // Needs to be a ptr because the value will change.
    int rect_length;
    int rect_opposite_length;
    Size size; // Widget size after optional expansion.
    int *generic_length; // Needs to be a ptr because the value will change.
    if (parent_layout == Align::Vertical) {
        generic_non_expandable_widgets = m_vertical_non_expandable;
        generic_total_layout_length = m_widgets_only.h;
        generic_position_coord = &pos.y;
        rect_length = rect.h;
        rect_opposite_length = rect.w;
        generic_length = &size.h;
    } else {
        generic_non_expandable_widgets = m_horizontal_non_expandable;
        generic_total_layout_length = m_widgets_only.w;
        generic_position_coord = &pos.x;
        rect_length = rect.w;
        rect_opposite_length = rect.h;
        generic_length = &size.w;
    }
    int child_count = m_visible_children - generic_non_expandable_widgets;
    if (!child_count) child_count = 1; // Protects from division by zero
    int expandable_length = (rect_length - generic_total_layout_length) / child_count;
    int remainder = (rect_length - generic_total_layout_length) % child_count;
    if (expandable_length < 0) {
        expandable_length = 0;
        remainder = 0;
    }
    for (Widget* child : children) {
        Size child_hint = child->sizeHint(dc);
        int child_expandable_length = expandable_length;
        switch (parent_layout) {
            case Align::Vertical:
                switch (child->fillPolicy()) {
                    case Fill::Both: {
                        if (remainder) {
                            child_expandable_length++;
                            remainder--;
                        }
                        size = Size {
                            rect_opposite_length > child_hint.w ? rect_opposite_length : child_hint.w,
                            child_hint.h + (child_expandable_length * (int)child->proportion())
                        };
                        break;
                    }
                    case Fill::Vertical: {
                        if (remainder) {
                            child_expandable_length++;
                            remainder--;
                        }
                        size = Size {
                            child_hint.w,
                            child_hint.h + (child_expandable_length * (int)child->proportion())
                        };
                        break;
                    }
                    case Fill::Horizontal:
                        size = Size {
                            rect_opposite_length > child_hint.w ? rect_opposite_length : child_hint.w,
                            child_hint.h
                        };
                        break;
                    case Fill::None:
                    default:
                        size = child_hint;
                }
                break;
            case Align::Horizontal:
                switch (child->fillPolicy()) {
                    case Fill::Both: {
                            if (remainder) {
                                child_expandable_length++;
                                remainder--;
                            }
                            size = Size {
                                child_hint.w + (child_expandable_length * (int)child->proportion()),
                                rect_opposite_length > child_hint.h ? rect_opposite_length : child_hint.h
                            };
                            break;
                        }
                        case Fill::Vertical:
                            size = Size {
                                child_hint.w,
                                rect_opposite_length > child_hint.h ? rect_opposite_length : child_hint.h
                            };
                            break;
                        case Fill::Horizontal: {
                            if (remainder) {
                                child_expandable_length++;
                                remainder--;
                            }
                            size = Size {
                                child_hint.w + (child_expandable_length * (int)child->proportion()),
                                child_hint.h
                            };
                            break;
                        }
                        case Fill::None:
                        default:
                            size = child_hint;
                    break;
                }
        }
        Rect widget_rect = Rect(pos.x, pos.y, size.w, size.h);
        // TODO find a solution to optimize the drawing further
        // since this is suboptimal
        if ((*generic_position_coord + *generic_length) < 0) {
            child->rect = widget_rect;
        } else {
            if (child->isVisible()) {
                child->draw(dc, widget_rect, child->state());
            }
            // TODO this seems dumb, use window dimensions instead
            // right it would never finish drawing early because the widget in a box
            // always get their ask in terms of size so the rect dimensions would be
            // at least as big as all the widget
            if (m_align_policy == Align::Horizontal) {
                if ((*generic_position_coord + *generic_length) > rect.x + rect.w) {
                    break;
                }
            } else {
                if ((*generic_position_coord + *generic_length) > rect.y + rect.h) {
                    break;
                }
            }
        }
        if (child->isVisible()) {
            *generic_position_coord += *generic_length;
        }
    }
}

Size Box::sizeHint(DrawingContext &dc) {
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
        m_vertical_non_expandable = vertical_non_expandable;
        m_horizontal_non_expandable = horizontal_non_expandable;
        m_visible_children = visible;
        m_size = size;
        m_widgets_only = size;
        m_size_changed = false;

        return size;
    } else {
        return m_size;
    }
}

bool Box::isLayout() {
    return true;
}

const char* Box::name() {
    return "Box";
}

void Box::setAlignPolicy(Align align_policy) {
    if (m_align_policy != align_policy) {
        m_align_policy = align_policy;
        layout();
    }
}

Align Box::alignPolicy() {
    return m_align_policy;
}
