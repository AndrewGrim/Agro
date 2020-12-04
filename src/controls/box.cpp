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

    // TODO probably can remove this, need to double check
    // as in the scissor test
    // actually maybe not because we would draw one over the viewport?
    dc->render();
    glEnable(GL_SCISSOR_TEST);
        Size window = ((Application*)this->app)->m_size;
        glScissor(rect.x, window.h - (rect.y + rect.h), rect.w, rect.h);
        layoutChildren(dc, rect);
        dc->render();
    glDisable(GL_SCISSOR_TEST);
}

void Box::layoutChildren(DrawingContext *dc, Rect rect) {
    // TODO double check if scissor test is needed, i think it is
    // also dont relayout children if the app layout hasnt changed?
    // ^ i think that would require setting the rect which we would need for bsearch anyway
    int non_expandable_widgets = 0;
    if (((Application*)this->app)->hasLayoutChanged()) {
        this->sizeHint(dc);
    }
    // TODO can we omptimize this?
    // we might not need to do this if we keep a variable keeping track of this
    // we could change it when a widgets is appended to a layout
    // and when the widget changes fillpolicy it could notify its parent?????
    // in fact we could store the non_expandable widgets separately for both align policies
    // obviously this would also need to change when destroying a widget (which i should implement :^)
    // the optimization would be worth it on a higher number of widgets
    // we know that the lib can do about 70k buttons in a scrolledbox smoothly in -O2
    // and this would get rid off of one loop through all the buttons
    // TODO we should probably do the above in sizeHint, and either
    // store the expandable widgets for both alignments or store the current
    // and reevaluate after changing the policy
    Align parent_layout = this->alignPolicy();
    for (Widget* child : this->children) {
        if (child->isVisible()) {
            Fill child_layout = child->fillPolicy();
            if (parent_layout == Align::Vertical) {
                if (child_layout == Fill::Horizontal || child_layout == Fill::None) {
                    non_expandable_widgets += 1;
                }
            } else if (parent_layout == Align::Horizontal) {
                if (child_layout == Fill::Vertical || child_layout == Fill::None) {
                    non_expandable_widgets += 1;
                }
            }
        }
    }

    // TODO add support for proportions, this could work like so
    // the widget would store its proportion internally with a default of 1
    // then we dividing the avaiable space between widgets we would get their internal
    // proportion and use that for the calculation
    // we would probably need to subtract extra proportions from child_count
    // and then multiply expandable_height times proportion
    // int child_count = this->m_visible_children - non_expandable_widgets;
    // if (!child_count) child_count = 1; // Protects from division by zero

    // TODO we wouldnt need to do most of this part unless the layout changed
    // (if we are going to implement bsearch!)
    // we would only need to go over every widget and redraw the visible ones
    int child_count = this->m_visible_children - non_expandable_widgets;
    if (!child_count) child_count = 1; // Protects from division by zero
    Point pos = Point(rect.x, rect.y);
    float generic_total_layout_length;
    float *generic_position_coord; // Needs to be a ptr because the value will change.
    float rect_length;
    Size size;
    float *generic_length; // Needs to be a ptr because the value will change.
    Size app_size = ((Application*)this->app)->m_size;
    float generic_app_length;
    switch (parent_layout) {
        case Align::Vertical:
            generic_total_layout_length = this->m_size.h;
            generic_position_coord = &pos.y;
            rect_length = rect.h;
            generic_length = &size.h;
            generic_app_length = app_size.h;
            break;
        case Align::Horizontal:
            generic_total_layout_length = this->m_size.w;
            generic_position_coord = &pos.x;
            rect_length = rect.w;
            generic_length = &size.w;
            generic_app_length = app_size.w;
            break;
    }
    float expandable_length = (rect_length - generic_total_layout_length) / child_count;
    if (expandable_length < 0) expandable_length = 0;
    for (Widget* child : this->children) {
        Size child_hint = child->sizeHint(dc);
        switch (parent_layout) {
            case Align::Vertical:
                switch (child->fillPolicy()) {
                    case Fill::Both: {
                        size = Size { 
                            rect.w > child_hint.w ? rect.w : child_hint.w, 
                            child_hint.h + expandable_length
                        };
                        break;
                    }
                    case Fill::Vertical: {
                        size = Size { 
                            child_hint.w, 
                            child_hint.h + expandable_length
                        };
                        break;
                    }
                    case Fill::Horizontal:
                        size = Size { 
                            rect.w > child_hint.w ? rect.w : child_hint.w, 
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
                                child_hint.w + expandable_length, 
                                rect.h > child_hint.h ? rect.h : child_hint.h
                            };
                            break;
                        }
                        case Fill::Vertical:
                            size = Size { 
                                child_hint.w, 
                                rect.h > child_hint.h ? rect.h : child_hint.h 
                            };
                            break;
                        case Fill::Horizontal: {
                            size = Size { 
                                child_hint.w + expandable_length, 
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
        if (generic_position_coord < 0) {
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
}

Size Box::sizeHint(DrawingContext *dc) {
    // `hasLayoutChanged()` is necessary here because the Box won't know
    // that it's sizeHint() needs to be recalculated because of it's children.
    unsigned int visible = 0;
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
                    visible++;
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
                    visible++;
                }
            }
        }
        this->m_visible_children = visible;
        this->m_size = size;
        this->m_size_changed = false;

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
        // this->layout
        // TODO calling layout here might be necessary if
        // we are going for the bsearch impementation
        // and the if layout changed in layouts layoutChildren()
    }
}

Align Box::alignPolicy() {
    return this->m_align_policy;
}