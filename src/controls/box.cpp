#include "box.hpp"
#include "../common/point.hpp"
#include "../application.hpp"

Box::Box(Align align_policy) : m_align_policy{align_policy} {}

Box::~Box() {}

void Box::draw(DrawingContext &dc, Rect rect, i32 state) {
    this->rect = rect;
    dc.fillRect(rect, dc.windowBackground(style()));
    layoutChildren(dc, rect);
}

void Box::layoutChildren(DrawingContext &dc, Rect rect) {
    Align parent_layout = alignPolicy();
    i32 generic_non_expandable_widgets;
    Point pos = Point(rect.x, rect.y);
    i32 generic_total_layout_length;
    i32 *generic_position_coord; // Needs to be a ptr because the value will change.
    i32 rect_length;
    i32 rect_opposite_length;
    Size size; // Widget size after optional expansion.
    i32 *generic_length; // Needs to be a ptr because the value will change.
    Rect parent_rect = parent ? parent->rect : Rect(0, 0, Application::get()->currentWindow()->size.w, Application::get()->currentWindow()->size.h);
    i32 generic_parent_coord;
    i32 generic_parent_length;
    if (parent_layout == Align::Vertical) {
        generic_non_expandable_widgets = m_vertical_non_expandable;
        generic_total_layout_length = m_widgets_only.h;
        generic_position_coord = &pos.y;
        rect_length = rect.h;
        rect_opposite_length = rect.w;
        generic_length = &size.h;
        generic_parent_coord = parent_rect.y;
        generic_parent_length = parent_rect.h;
    } else {
        generic_non_expandable_widgets = m_horizontal_non_expandable;
        generic_total_layout_length = m_widgets_only.w;
        generic_position_coord = &pos.x;
        rect_length = rect.w;
        rect_opposite_length = rect.h;
        generic_length = &size.w;
        generic_parent_coord = parent_rect.x;
        generic_parent_length = parent_rect.w;
    }
    i32 child_count = m_visible_children - generic_non_expandable_widgets;
    if (!child_count) child_count = 1; // Protects from division by zero
    i32 expandable_length = (rect_length - generic_total_layout_length) / child_count;
    i32 remainder = (rect_length - generic_total_layout_length) % child_count;
    if (expandable_length < 0) {
        expandable_length = 0;
        remainder = 0;
    }
    for (Widget *child : children) {
        Size child_hint = child->sizeHint(dc);
        i32 child_expandable_length = expandable_length;
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
                            child_hint.h + (child_expandable_length * (i32)child->proportion())
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
                            child_hint.h + (child_expandable_length * (i32)child->proportion())
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
                                child_hint.w + (child_expandable_length * (i32)child->proportion()),
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
                                child_hint.w + (child_expandable_length * (i32)child->proportion()),
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
        if ((*generic_position_coord + *generic_length) < generic_parent_coord) {
            child->rect = widget_rect;
        } else {
            if (child->isVisible()) {
                child->draw(dc, widget_rect, child->state());
            }
            if ((*generic_position_coord + *generic_length) > generic_parent_coord + generic_parent_length) {
                break;
            }
        }
        if (child->isVisible()) {
            *generic_position_coord += *generic_length;
        }
    }
}

Size Box::sizeHint(DrawingContext &dc) {
    u32 visible = 0;
    u32 vertical_non_expandable = 0;
    u32 horizontal_non_expandable = 0;
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

i32 Box::isFocusable() {
    return (i32)FocusType::Passthrough;
}

const char* Box::name() {
    return "Box";
}

void Box::setAlignPolicy(Align align_policy) {
    if (m_align_policy != align_policy) {
        m_align_policy = align_policy;
        layout(LAYOUT_CHILD);
    }
}

Align Box::alignPolicy() {
    return m_align_policy;
}

Widget* Box::handleFocusEvent(FocusEvent event, State *state, FocusPropagationData data) {
    assert(event != FocusEvent::Activate && "handleFocusEvent should only be called with Forward and Reverse focus events!");
    if (event == FocusEvent::Forward) {
        i32 child_index_unwrapped = data.origin_index ? data.origin_index.unwrap() + 1 : 0;
        for (; child_index_unwrapped < (i32)children.size(); child_index_unwrapped++) {
            Widget *child = children[child_index_unwrapped];
            if (child->isFocusable() && child->isVisible()) {
                return child->handleFocusEvent(event, state, FocusPropagationData());
            }
        }
        if (parent) {
            return parent->handleFocusEvent(event, state, FocusPropagationData(this, parent_index));
        }
        return nullptr;
    } else {
        i32 child_index_unwrapped = data.origin_index ? data.origin_index.unwrap() - 1 : (i32)children.size() - 1;
        for (; child_index_unwrapped > -1; child_index_unwrapped--) {
            Widget *child = children[child_index_unwrapped];
            if (child->isFocusable() && child->isVisible()) {
                return child->handleFocusEvent(event, state, FocusPropagationData());
            }
        }
        if (parent) {
            return parent->handleFocusEvent(event, state, FocusPropagationData(this, parent_index));
        }
        return nullptr;
    }
}
