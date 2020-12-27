#include "../app.hpp"
#include "scrolledbox.hpp"

ScrolledBox::ScrolledBox(Align align_policy, Size min_size) : Box(align_policy) {
    this->m_viewport = min_size;
}

ScrolledBox::~ScrolledBox() {
    // ScrollBars are not added to children so if they
    // exists the need to be delete with the ScrolledBox.
    if (m_horizontal_scrollbar) {
        delete m_horizontal_scrollbar;
    }
    if (m_vertical_scrollbar) {
        delete m_vertical_scrollbar;
    }
}

const char* ScrolledBox::name() {
    return "ScrolledBox";
}

void ScrolledBox::draw(DrawingContext *dc, Rect rect) {
    this->rect = rect;
    dc->fillRect(rect, this->m_bg);

    dc->render();
    glEnable(GL_SCISSOR_TEST);
        Size window = ((Application*)this->app)->m_size;
        glScissor(rect.x, window.h - (rect.y + rect.h), rect.w, rect.h);
        layoutChildren(dc, rect);
        dc->render();
    glDisable(GL_SCISSOR_TEST);
}

void ScrolledBox::layoutChildren(DrawingContext *dc, Rect rect) {
    Align parent_layout = this->alignPolicy();
    int generic_non_expandable_widgets;
    Point pos;
    float generic_total_layout_length;
    float generic_max_layout_length;
    float *generic_position_coord; // Needs to be a ptr because the value will change.
    float *generic_rect_coord;
    float rect_length;
    Size size; // Individual widget size.
    float *generic_length; // Needs to be a ptr because the value will change.
    Size app_size = ((Application*)this->app)->m_size;
    float generic_app_length;
    switch (parent_layout) {
        case Align::Vertical:
            generic_non_expandable_widgets = this->m_vertical_non_expandable;
            generic_total_layout_length = this->m_size.h;
            generic_max_layout_length = this->m_size.w;
            generic_position_coord = &pos.y;
            generic_rect_coord = &rect.y;
            rect_length = rect.h;
            generic_length = &size.h;
            generic_app_length = app_size.h;
            break;
        case Align::Horizontal:
            generic_non_expandable_widgets = this->m_horizontal_non_expandable;
            generic_total_layout_length = this->m_size.w;
            generic_max_layout_length = this->m_size.h;
            generic_position_coord = &pos.x;
            generic_rect_coord = &rect.x;
            rect_length = rect.w;
            generic_length = &size.w;
            generic_app_length = app_size.w;
            break;
    }

    float content_x = rect.x;
    float content_y = rect.y;
    if (this->m_align_policy == Align::Horizontal) {
        if (rect.w < generic_total_layout_length) {
            this->addScrollBar(Align::Horizontal);
        } else {
            this->removeScrollBar(Align::Horizontal);
        }
        if (rect.h < generic_max_layout_length) {
            this->addScrollBar(Align::Vertical);
        } else {
            this->removeScrollBar(Align::Vertical);
        }
        if (this->hasScrollBar(Align::Vertical)) {
            float vscroll = 0;
            if (this->hasScrollBar(Align::Horizontal)) vscroll += this->m_horizontal_scrollbar->sizeHint(dc).h;
            content_y -= this->m_vertical_scrollbar->m_slider->m_value * ((generic_max_layout_length + vscroll) - rect.h);
        }
        if (this->hasScrollBar(Align::Horizontal)) {
            float hscroll = 0;
            if (this->hasScrollBar(Align::Vertical)) hscroll += this->m_vertical_scrollbar->sizeHint(dc).w;
            content_x -= this->m_horizontal_scrollbar->m_slider->m_value * ((generic_total_layout_length + hscroll) - rect.w);
        }
    } else {
        if (rect.h < generic_total_layout_length) {
            this->addScrollBar(Align::Vertical);
        } else {
            this->removeScrollBar(Align::Vertical);
        }
        if (rect.w < generic_max_layout_length) {
            this->addScrollBar(Align::Horizontal);
        } else {
            this->removeScrollBar(Align::Horizontal);
        }
        if (this->hasScrollBar(Align::Vertical)) {
            float vscroll = 0;
            if (this->hasScrollBar(Align::Horizontal)) vscroll += this->m_horizontal_scrollbar->sizeHint(dc).h;
            content_y -= this->m_vertical_scrollbar->m_slider->m_value * ((generic_total_layout_length + vscroll) - rect.h);
        }
        if (this->hasScrollBar(Align::Horizontal)) {
            float hscroll = 0;
            if (this->hasScrollBar(Align::Vertical)) hscroll += this->m_vertical_scrollbar->sizeHint(dc).w;
            content_x -= this->m_horizontal_scrollbar->m_slider->m_value * ((generic_max_layout_length + hscroll) - rect.w);
        }
    }
    pos = Point(content_x, content_y);
    // if (((Application*)this->app)->hasLayoutChanged()) {
        this->sizeHint(dc);
        int child_count = this->m_visible_children - generic_non_expandable_widgets;
        if (!child_count) child_count = 1; // Protects from division by zero
        float expandable_length = (rect_length - generic_total_layout_length) / child_count;
        if (expandable_length < 0) expandable_length = 0;
        if (this->hasScrollBar(Align::Vertical)) rect.w -= m_vertical_scrollbar->sizeHint(dc).w;
        if (this->hasScrollBar(Align::Horizontal)) rect.h -= m_horizontal_scrollbar->sizeHint(dc).h;
        for (Widget* child : this->children) {
            Size child_hint = child->sizeHint(dc);
            switch (parent_layout) {
                case Align::Vertical:
                    switch (child->fillPolicy()) {
                        case Fill::Both: {
                            size = Size { 
                                rect.w > child_hint.w ? rect.w : child_hint.w, 
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
                                    child_hint.w + (expandable_length * child->proportion()), 
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
            // TODO shouldnt we set widget rect for all widgets?
            // especially since its calculated anyway?
            // and would that allow use to not set rect in draw??? (i dont think so??)
            if (*generic_position_coord < *generic_rect_coord) {
                if (*generic_position_coord + *generic_length < *generic_rect_coord) {
                    child->rect = widget_rect;
                }
                else goto DRAW_WIDGET;
            } else {
                DRAW_WIDGET:
                    if (child->isVisible()) {
                        child->draw(dc, widget_rect);
                    }
                    // TODO generic app length might need to be generic coord + generic length??
                    // update: definitely
                    if (*generic_position_coord > generic_app_length) {
                        break;
                    }
            }
            if (child->isVisible()) {
                *generic_position_coord += *generic_length;
            }
        }
        if (parent_layout == Align::Vertical) {
            if (m_vertical_scrollbar) {
                Size size = m_vertical_scrollbar->sizeHint(dc);
                float slider_size = rect.h * ((rect.h - size.h / 2) / generic_total_layout_length);
                float buttons_size = m_vertical_scrollbar->m_begin_button->sizeHint(dc).h + m_vertical_scrollbar->m_end_button->sizeHint(dc).h;
                if (slider_size < 20) {
                    slider_size = 20;
                } else if (slider_size > (rect.h - buttons_size - 10)) {
                    slider_size = rect.h - buttons_size - 10;
                }
                m_vertical_scrollbar->m_slider->m_slider_button_size = slider_size;
                m_vertical_scrollbar->draw(dc, Rect(
                    rect.x + rect.w, 
                    rect.y, 
                    size.w, 
                    rect.h > size.h ? rect.h : size.h
                ));
            }
            if (m_horizontal_scrollbar) {
                Size size = m_horizontal_scrollbar->sizeHint(dc);
                float slider_size = rect.w * ((rect.w - size.w / 2) / generic_max_layout_length);
                float buttons_size = m_horizontal_scrollbar->m_begin_button->sizeHint(dc).w + m_horizontal_scrollbar->m_end_button->sizeHint(dc).w;
                if (slider_size < 20) {
                    slider_size = 20;
                } else if (slider_size > (rect.w - buttons_size - 10)) {
                    slider_size = rect.w - buttons_size - 10;
                }
                m_horizontal_scrollbar->m_slider->m_slider_button_size = slider_size;
                m_horizontal_scrollbar->draw(dc, Rect(
                    rect.x, 
                    rect.y + rect.h, 
                    rect.w > size.w ? rect.w : size.w, 
                    size.h
                ));
            }
        } else {
            if (m_vertical_scrollbar) {
            Size size = m_vertical_scrollbar->sizeHint(dc);
            float slider_size = rect.h * ((rect.h - size.h / 2) / generic_max_layout_length);
            float buttons_size = m_vertical_scrollbar->m_begin_button->sizeHint(dc).h + m_vertical_scrollbar->m_end_button->sizeHint(dc).h;
            if (slider_size < 20) {
                slider_size = 20;
            } else if (slider_size > (rect.h - buttons_size - 10)) {
                slider_size = rect.h - buttons_size - 10;
            }
            m_vertical_scrollbar->m_slider->m_slider_button_size = slider_size;
            m_vertical_scrollbar->draw(dc, Rect(
                rect.x + rect.w, 
                rect.y, 
                size.w, 
                rect.h > size.h ? rect.h : size.h
            ));
        }
        if (m_horizontal_scrollbar) {
            Size size = m_horizontal_scrollbar->sizeHint(dc);
            float slider_size = rect.w * ((rect.w - size.w / 2) / generic_total_layout_length);
            float buttons_size = m_horizontal_scrollbar->m_begin_button->sizeHint(dc).w + m_horizontal_scrollbar->m_end_button->sizeHint(dc).w;
            if (slider_size < 20) {
                slider_size = 20;
            } else if (slider_size > (rect.w - buttons_size - 10)) {
                slider_size = rect.w - buttons_size - 10;
            }
            m_horizontal_scrollbar->m_slider->m_slider_button_size = slider_size;
            m_horizontal_scrollbar->draw(dc, Rect(
                rect.x, 
                rect.y + rect.h, 
                rect.w > size.w ? rect.w : size.w, 
                size.h
            ));
        }
        }
        if (m_vertical_scrollbar && m_horizontal_scrollbar) {
            dc->fillRect(Rect(
                rect.x + rect.w, 
                rect.y + rect.h, 
                m_vertical_scrollbar->sizeHint(dc).w, 
                m_horizontal_scrollbar->sizeHint(dc).h), 
                m_vertical_scrollbar->m_begin_button->background()
            );
        }
    // }
    // TODO provide a more efficient drawing operation
    // when the layout hasnt changed
}

void ScrolledBox::addScrollBar(Align alignment) {
    if (alignment == Align::Horizontal) {
        if (!this->m_horizontal_scrollbar) {
            this->m_horizontal_scrollbar = new ScrollBar(alignment);
        }
        if (this->m_horizontal_scrollbar) {
            this->m_horizontal_scrollbar->app = this->app;
            for (Widget *child : this->m_horizontal_scrollbar->children) {
                child->app = this->app;
                child->attachApp(this->app);
            }
        }
    }
    else {
        if (!this->m_vertical_scrollbar) {
            this->m_vertical_scrollbar = new ScrollBar(alignment);
        }
        if (this->m_vertical_scrollbar) {
            this->m_vertical_scrollbar->app = this->app;
            for (Widget *child : this->m_vertical_scrollbar->children) {
                child->app = this->app;
                child->attachApp(this->app);
            }
        }
    }
}

void ScrolledBox::removeScrollBar(Align alignment) {
    if (alignment == Align::Horizontal) {
        if (this->m_horizontal_scrollbar) {
            delete this->m_horizontal_scrollbar;
            this->m_horizontal_scrollbar = nullptr;
        }
    } else {
        if (this->m_vertical_scrollbar) {
            delete this->m_vertical_scrollbar;
            this->m_vertical_scrollbar = nullptr;
        }
    }
}

// TODO this is kinda useless considering we can just do if (m_vertical_scrollbar)
bool ScrolledBox::hasScrollBar(Align alignment) {
    if (alignment == Align::Horizontal) {
        if (m_horizontal_scrollbar) return true;
        else return false;
    } else {
        if (m_vertical_scrollbar) return true;
        else return false;
    }
}

bool ScrolledBox::isScrollable() {
    return true;
}

Size ScrolledBox::sizeHint(DrawingContext *dc) {
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

        return m_viewport;
    } else {
        return m_viewport;
    }
}