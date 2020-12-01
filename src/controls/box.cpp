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
    // TODO simplify the code branches here, and double check if scissor test is needed, i think it is
    // also dont relayout children if the app layout hasnt changed?
    // ^ i think that would require setting the rect which we would need for bsearch anyway
    int non_expandable_widgets = 0;
    if (this->m_size_changed) {
        this->sizeHint(dc);
    }
    Size total_children_size = this->m_size;
    // TODO can we omptimize this?
    // we might not need to do this if we keep a variable keeping track of this
    // we could change it when a widgets is appended to a layout
    // and when the widget changes fillpolicy it could notify its parent?????
    // in fact we could store the non_expandable widgets separately for both align policies
    // obviously this would also need to change when destroying a widget (which i should implement :^)
    // the optimization would be worth it on a higher number of widgets
    // we know that the lib can do about 70k buttons in a scrolledbox smoothly in -O2
    // and this would get rid off of one loop through all the buttons
    Align parent_layout = this->alignPolicy();
    for (Widget* child : this->children) {
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

    // TODO add support for proportions, this could work like so
    // the widget would store its proportion internally with a default of 1
    // then we dividing the avaiable space between widgets we would get their internal
    // proportion and use that for the calculation
    // we would probably need to subtract extra proportions from child_count
    // and then multiply expandable_height times proportion
    int child_count = this->children.size() - non_expandable_widgets;
    if (!child_count) child_count = 1; // Protects from division by zero
    Point pos = Point(rect.x, rect.y);
    // TODO we wouldnt need to do most of this part unless the layout changed
    // we would only need to go over every widget and redraw the visible ones
    // TODO check if widget is visible
    // should that affect anything other than drawing??
    switch (parent_layout) {
        case Align::Vertical: {
            float available_height = rect.h;
            float expandable_height = (available_height - total_children_size.h) / child_count;
            if (expandable_height < 0) expandable_height = 0;
            for (Widget* child : this->children) {
                Size size;
                Size child_hint = child->sizeHint(dc);
                switch (child->fillPolicy()) {
                    case Fill::Both: {
                        size = Size { 
                            rect.w > child_hint.w ? rect.w : child_hint.w, 
                            child_hint.h + expandable_height
                        };
                        break;
                    }
                    case Fill::Vertical: {
                        size = Size { 
                            child_hint.w, 
                            child_hint.h + expandable_height
                        };
                        break;
                    }
                    case Fill::Horizontal:
                        size = Size { rect.w > child_hint.w ? rect.w : child_hint.w, child_hint.h };
                        break;
                    case Fill::None:
                    default:
                        size = child_hint;
                }
                Rect widget_rect = Rect(pos.x, pos.y, size.w, size.h);
                if (pos.y < 0) {
                    if (pos.y + size.h < 0) {
                        child->rect = widget_rect;
                    }
                    else goto DRAW_VERTICAL;
                } else {
                    DRAW_VERTICAL:
                        child->draw(dc, widget_rect);
                        if (pos.y > ((Application*)this->app)->m_size.h) break;
                }
                pos.y += size.h;
            }
            break;
        }
        case Align::Horizontal: {
            float available_width = rect.w;
            float expandable_width = (available_width - total_children_size.w) / child_count;
            if (expandable_width < 0) expandable_width = 0;
            for (Widget* child : this->children) {
                Size size;
                Size child_hint = child->sizeHint(dc);
                switch (child->fillPolicy()) {
                    case Fill::Both: {
                        size = Size { 
                            child_hint.w + expandable_width, 
                            rect.h > child_hint.h ? rect.h : child_hint.h
                        };
                        break;
                    }
                    case Fill::Vertical:
                        size = Size { child_hint.w, rect.h > child_hint.h ? rect.h : child_hint.h };
                        break;
                    case Fill::Horizontal: {
                        size = Size { 
                            child_hint.w + expandable_width, 
                            child_hint.h
                        };
                        break;
                    }
                    case Fill::None:
                    default:
                        size = child_hint;
                }
                Rect widget_rect = Rect(pos.x, pos.y, size.w, size.h);
                if (pos.x < 0) {
                    if (pos.x + size.w < 0) {
                        child->rect = widget_rect;
                    }
                    else goto DRAW_HORIZONTAL;
                } else {
                    DRAW_HORIZONTAL:
                        child->draw(dc, widget_rect);
                        if (pos.x > ((Application*)this->app)->m_size.w) break;
                }
                pos.x += size.w;
            }
            break;
        }   
    }
}

Size Box::sizeHint(DrawingContext *dc) {
    // `hasLayoutChanged()` is necessary here because the Box won't know
    // that it's sizeHint() needs to be recalculated because of it's children.
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
                    this->m_visible_children++;
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
                    this->m_visible_children++;
                }
            }
        }
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