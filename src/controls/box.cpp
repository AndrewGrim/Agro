#include "../app.hpp"
#include "box.hpp"

Box::Box(Align align_policy) {
    this->m_align_policy = align_policy;
}

Box::~Box() {}

const char* Box::name() {
    return this->m_name;
}

void Box::draw(DrawingContext *dc, Rect<float> rect) {
    this->rect = rect;
    dc->fillRect(rect, this->bg);

    dc->render();
    glEnable(GL_SCISSOR_TEST);
        Size<int> window = ((Application*)this->m_app)->m_size;
        glScissor(rect.x, window.h - (rect.y + rect.h), rect.w, rect.h);
        layout_children(dc, rect);
        dc->render();
    glDisable(GL_SCISSOR_TEST);
}

void Box::layout_children(DrawingContext *dc, Rect<float> rect) {
    int non_expandable_widgets = 0;
    Size<float> total_children_size;
    Align parent_layout = this->m_align_policy;
    for (Widget* child : this->children) {
        Size<float> child_size = child->size_hint(dc);
        Fill child_layout = child->fill_policy();
        if (parent_layout == Align::Vertical) {
            if (child_layout == Fill::Horizontal || child_layout == Fill::None) {
                non_expandable_widgets += 1;
            }
        } else if (parent_layout == Align::Horizontal) {
            if (child_layout == Fill::Vertical || child_layout == Fill::None) {
                non_expandable_widgets += 1;
            }
        }
        total_children_size.w += child_size.w;
        total_children_size.h += child_size.h;
    }

    int child_count = this->children.size() - non_expandable_widgets;
    if (!child_count) child_count = 1; // Protects from division by zero
    Point<float> pos = Point<float>(rect.x, rect.y);
    switch (parent_layout) {
        case Align::Vertical: {
            float available_height = rect.h;
            float expandable_height = (available_height - total_children_size.h) / child_count;
            if (expandable_height < 0) expandable_height = 0;
            for (Widget* child : this->children) {
                Size<float> size;
                Size<float> child_hint = child->size_hint(dc);
                switch (child->fill_policy()) {
                    case Fill::Both: {
                        size = Size<float> { 
                            rect.w > child_hint.w ? rect.w : child_hint.w, 
                            child_hint.h + expandable_height
                        };
                        break;
                    }
                    case Fill::Vertical: {
                        size = Size<float> { 
                            child_hint.w, 
                            child_hint.h + expandable_height
                        };
                        break;
                    }
                    case Fill::Horizontal:
                        size = Size<float> { rect.w > child_hint.w ? rect.w : child_hint.w, child_hint.h };
                        break;
                    case Fill::None:
                    default:
                        size = child_hint;
                }
                Rect<float> widget_rect = Rect<float>(pos.x, pos.y, size.w, size.h);
                if (pos.y < 0) {
                    if (pos.y + size.h < 0) {
                        child->rect = widget_rect;
                    }
                    else goto DRAW_VERTICAL;
                } else {
                    DRAW_VERTICAL:
                        child->draw(dc, widget_rect);
                        if (pos.y > ((Application*)this->m_app)->m_size.h) break;
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
                Size<float> size;
                Size<float> child_hint = child->size_hint(dc);
                switch (child->fill_policy()) {
                    case Fill::Both: {
                        size = Size<float> { 
                            child_hint.w + expandable_width, 
                            rect.h > child_hint.h ? rect.h : child_hint.h
                        };
                        break;
                    }
                    case Fill::Vertical:
                        size = Size<float> { child_hint.w, rect.h > child_hint.h ? rect.h : child_hint.h };
                        break;
                    case Fill::Horizontal: {
                        size = Size<float> { 
                            child_hint.w + expandable_width, 
                            child_hint.h
                        };
                        break;
                    }
                    case Fill::None:
                    default:
                        size = child_hint;
                }
                Rect<float> widget_rect = Rect<float>(pos.x, pos.y, size.w, size.h);
                if (pos.x < 0) {
                    if (pos.x + size.w < 0) {
                        child->rect = widget_rect;
                    }
                    else goto DRAW_HORIZONTAL;
                } else {
                    DRAW_HORIZONTAL:
                        child->draw(dc, widget_rect);
                        if (pos.x > ((Application*)this->m_app)->m_size.w) break;
                }
                pos.x += size.w;
            }
            break;
        }   
    }
}

Size<float> Box::size_hint(DrawingContext *dc) {
    if (this->size_changed || ((Application*)this->m_app)->m_layout_changed) {
        Size<float> size = Size<float> { 0, 0 };
        if (this->m_align_policy == Align::Horizontal) {
            for (Widget* child : this->children) {
                Size<float> s = child->size_hint(dc);
                size.w += s.w;
                if (s.h > size.h) {
                    size.h = s.h;
                }
            }
        } else {
            for (Widget* child : this->children) {
                Size<float> s = child->size_hint(dc);
                size.h += s.h;
                if (s.w > size.w) {
                    size.w = s.w;
                }
            }
        }
        this->m_size = size;
        this->size_changed = false;

        return size;
    } else {
        return this->m_size;
    }
}

Color Box::background() {
    return this->bg;
}

Box* Box::set_background(Color background) {
    this->bg = background;
    this->update();

    return this;
}

bool Box::is_layout() {
    return true;
}