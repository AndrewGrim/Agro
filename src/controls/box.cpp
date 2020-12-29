#include "box.hpp"
#include "../common/point.hpp"
#include "../app.hpp"

Box::Box(Align align_policy) {
    this->m_align_policy = align_policy;
    Widget::m_bg = Color(0.5, 0.5, 0.5);
}

Box::~Box() {

}

void Box::draw(DrawingContext *dc, Rect rect) {
    this->rect = rect;
    dc->fillRect(rect, this->background());

    // TODO double check if this is even needed anymore, need scrolledbox to check
    dc->render();
    glEnable(GL_SCISSOR_TEST);
        Size window = ((Application*)this->app)->m_size;
        glScissor(rect.x, window.h - (rect.y + rect.h), rect.w, rect.h);
        layoutChildren(dc, rect);
        dc->render();
    glDisable(GL_SCISSOR_TEST);
}

void Box::layoutChildren(DrawingContext *dc, Rect rect) {
    // if (((Application*)this->app)->hasLayoutChanged()) {
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
        Size app_size = ((Application*)this->app)->m_size;
        float generic_app_length;
        switch (parent_layout) {
            case Align::Vertical:
                generic_non_expandable_widgets = this->m_vertical_non_expandable;
                generic_total_layout_length = this->m_size.h;
                generic_max_layout_length = this->m_size.w;
                generic_position_coord = &pos.y;
                rect_length = rect.h;
                rect_opposite_length = rect.w;
                generic_length = &size.h;
                generic_app_length = app_size.h;
                break;
            case Align::Horizontal:
                generic_non_expandable_widgets = this->m_horizontal_non_expandable;
                generic_total_layout_length = this->m_size.w;
                generic_max_layout_length = this->m_size.h;
                generic_position_coord = &pos.x;
                rect_length = rect.w;
                rect_opposite_length = rect.h;
                generic_length = &size.w;
                generic_app_length = app_size.w;
                break;
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
            if (*generic_position_coord < 0) {
                if (*generic_position_coord + *generic_length < 0) {
                    child->rect = widget_rect;
                }
                else goto DRAW_WIDGET;
            } else {
                DRAW_WIDGET:
                    if (child->isVisible()) {
                        child->draw(dc, widget_rect);
                    }
                    if (*generic_position_coord > generic_app_length) {
                        break;
                    }
            }
            if (child->isVisible()) {
                *generic_position_coord += *generic_length;
            }
        }
    // } 
    // else {
    //     float *generic_child_coord;
    //     float generic_app_length;
    //     Rect child_rect;
    //     Size app_size = ((Application*)this->app)->m_size;
    //     if (this->alignPolicy() == Align::Vertical) {
    //         generic_child_coord = &child_rect.y;
    //         generic_app_length = app_size.h;
    //     } else {
    //         generic_child_coord = &child_rect.x;
    //         generic_app_length = app_size.w;
    //     }
    //     for (Widget* child : this->children) {
    //         child_rect = child->rect;
    //         if (child->isVisible()) {
    //             child->draw(dc, child->rect);
    //         }
    //         if (*generic_child_coord > generic_app_length) {
    //             break;
    //         }
    //     }
    // }
}

Size Box::sizeHint(DrawingContext *dc) {
    // `hasLayoutChanged()` is necessary here because the Box won't know
    // that it's sizeHint() needs to be recalculated because of it's children.
    // TODO ideally now that every widget should have a parent (except main_widget?)
    // it would be better to simply recalculate whats changed rather than
    // all of it, simply recursivly go up the widget tree and say the layout changed
    // until we hit the top :)
    uint visible = 0;
    uint vertical_non_expandable = 0;
    uint horizontal_non_expandable = 0;
    if (this->m_size_changed || ((Application*)this->app)->hasLayoutChanged()) {
        Size size = Size();
        if (this->m_align_policy == Align::Horizontal) {
            for (Widget* child : this->children) {
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
            for (Widget* child : this->children) {
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
        this->m_vertical_non_expandable = vertical_non_expandable;
        this->m_horizontal_non_expandable = horizontal_non_expandable;
        this->m_visible_children = visible;
        this->m_size = size;
        this->m_size_changed = false; // TODO m_size_changed might not be really necessary, will need to reevalute

        return size;
    } else {
        return this->m_size;
    }
}

Box* Box::setBackground(Color background) {
    if (Widget::m_bg != background) {
        Widget::m_bg = background;
        this->update();
    }

    return this;
}


Box* Box::setForeground(Color foreground) {
    if (Widget::m_fg != foreground) {
        Widget::m_fg = foreground;
        this->update();
    }

    return this;
}

bool Box::isLayout() {
    return true;
}

const char* Box::name() {
    return "Box";
}

void Box::setAlignPolicy(Align align_policy) {
    if (this->m_align_policy != align_policy) {
        this->m_align_policy = align_policy;
        this->update();
        this->layout();
    }
}

Align Box::alignPolicy() {
    return this->m_align_policy;
}