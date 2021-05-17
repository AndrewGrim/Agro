#include "box.hpp"
#include "../common/point.hpp"
#include "../application.hpp"

Box::Box(Align align_policy) {
    this->m_align_policy = align_policy;
    Widget::m_bg = Color(0.5f, 0.5f, 0.5f);
}

Box::~Box() {

}

void Box::draw(DrawingContext *dc, Rect rect) {
    this->rect = rect;
    dc->fillRect(rect, this->background());
    layoutChildren(dc, rect);
}

void Box::layoutChildren(DrawingContext *dc, Rect rect) {
    this->sizeHint(dc);
    Align parent_layout = this->alignPolicy();
    int generic_non_expandable_widgets;
    Point pos = Point(rect.x, rect.y);
    float generic_total_layout_length;
    float generic_max_layout_length;
    float *generic_position_coord; // Needs to be a ptr because the value will change.
    float rect_length;
    float rect_opposite_length;
    Size size;
    float *generic_length; // Needs to be a ptr because the value will change.
    Size app_size = Application::get()->size;
    float generic_app_length;
    if (parent_layout == Align::Vertical) {
        generic_non_expandable_widgets = this->m_vertical_non_expandable;
        generic_total_layout_length = this->m_size.h;
        generic_max_layout_length = this->m_size.w;
        generic_position_coord = &pos.y;
        rect_length = rect.h;
        rect_opposite_length = rect.w;
        generic_length = &size.h;
        generic_app_length = app_size.h;
    } else {
        generic_non_expandable_widgets = this->m_horizontal_non_expandable;
        generic_total_layout_length = this->m_size.w;
        generic_max_layout_length = this->m_size.h;
        generic_position_coord = &pos.x;
        rect_length = rect.w;
        rect_opposite_length = rect.h;
        generic_length = &size.w;
        generic_app_length = app_size.w;
    }
    int child_count = this->m_visible_children - generic_non_expandable_widgets;
    if (!child_count) child_count = 1; // Protects from division by zero
    float expandable_length = (rect_length - generic_total_layout_length) / child_count;
    float expandable_opposite_length = rect_opposite_length > generic_max_layout_length ? rect_opposite_length : generic_max_layout_length;
    if (expandable_length < 0) expandable_length = 0;
    for (Widget* child : this->children) {
        Size child_hint = child->sizeHint(dc);
        switch (parent_layout) {
            case Align::Vertical:
                switch (child->fillPolicy()) {
                    case Fill::Both: {
                        size = Size { 
                            expandable_opposite_length > child_hint.w ? expandable_opposite_length : child_hint.w, 
                            child_hint.h + (expandable_length * child->proportion())
                        };
                        break;
                    }
                    case Fill::Vertical: {
                        size = Size { 
                            child_hint.w, 
                            child_hint.h + (expandable_length * child->proportion())
                        };
                        break;
                    }
                    case Fill::Horizontal:
                        size = Size { 
                            expandable_opposite_length > child_hint.w ? expandable_opposite_length : child_hint.w, 
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
                            size = Size { 
                                child_hint.w + (expandable_length * child->proportion()), 
                                expandable_opposite_length > child_hint.h ? expandable_opposite_length : child_hint.h
                            };
                            break;
                        }
                        case Fill::Vertical:
                            size = Size { 
                                child_hint.w, 
                                expandable_opposite_length > child_hint.h ? expandable_opposite_length : child_hint.h 
                            };
                            break;
                        case Fill::Horizontal: {
                            size = Size { 
                                child_hint.w + (expandable_length * child->proportion()), 
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
                // When the widget is wider than the parent
                // clip to the parent's width accounting for scroll.
                if (widget_rect.x + widget_rect.w > rect.x + rect.w) {
                    widget_rect.w = (rect.x + rect.w) - widget_rect.x;
                }
                // When the widget is taller than the parent
                // clip to the parent's height accounting for scroll.
                if (widget_rect.y + widget_rect.h > rect.y + rect.h) {
                    widget_rect.h = (rect.y + rect.h) - widget_rect.y;
                }
                // When the widget is visible within the parent but
                // starts outside of the visible area
                // clip only whats visible.
                if (widget_rect.x + widget_rect.w >= rect.x && !(widget_rect.x > rect.x)) {
                    widget_rect.x = rect.x;
                    widget_rect.w = widget_rect.x + widget_rect.w - rect.x;
                }
                // When the widget is visible within the parent but
                // starts outside of the visible area
                // clip only whats visible.
                if (widget_rect.y + widget_rect.h >= rect.y && !(widget_rect.y > rect.y)) {
                    widget_rect.y = rect.y;
                    widget_rect.h = widget_rect.y + widget_rect.h - rect.y;
                }
                child->draw(dc, widget_rect);
            }
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

Size Box::sizeHint(DrawingContext *dc) {
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
        m_size_changed = false; // TODO m_size_changed might not be really necessary, will need to reevalute

        return size;
    } else {
        return m_size;
    }
}

Box* Box::setBackground(Color background) {
    Widget::m_bg = background;
    this->update();

    return this;
}


Box* Box::setForeground(Color foreground) {
    Widget::m_fg = foreground;
    this->update();

    return this;
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