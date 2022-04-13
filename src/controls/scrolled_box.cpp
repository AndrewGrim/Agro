#include "../application.hpp"
#include "scrolled_box.hpp"

ScrolledBox::ScrolledBox(Align align_policy, Size min_size) : Scrollable(min_size) {
    m_align_policy = align_policy;
}

ScrolledBox::~ScrolledBox() {
    // ScrollBars are taken care of by Scrollable.
}

const char* ScrolledBox::name() {
    return "ScrolledBox";
}

void ScrolledBox::draw(DrawingContext &dc, Rect rect, i32 state) {
    this->rect = rect;
    Rect previous_clip = dc.clip();
    // TODO clips before scrollbars which means that
    // some scrolled content can get drawn underneath when horizontall scrolling for example
    clip();
    dc.fillRect(rect, dc.windowBackground(style));
    layoutChildren(dc, rect);
    dc.setClip(previous_clip);
}

void ScrolledBox::layoutChildren(DrawingContext &dc, Rect rect) {
    Align parent_layout = m_align_policy;
    i32 generic_non_expandable_widgets;
    Point pos;
    i32 generic_total_layout_length;
    i32 *generic_position_coord;
    i32 *generic_rect_coord;
    i32 *rect_length;
    i32 *rect_opposite_length;
    Size size; // Individual widget size.
    i32 *generic_length;
    if (parent_layout == Align::Vertical) {
        generic_non_expandable_widgets = m_vertical_non_expandable;
        generic_total_layout_length = m_widgets_only.h;
        generic_position_coord = &pos.y;
        generic_rect_coord = &rect.y;
        rect_length = &rect.h;
        rect_opposite_length = &rect.w;
        generic_length = &size.h;
    } else {
        generic_non_expandable_widgets = m_horizontal_non_expandable;
        generic_total_layout_length = m_widgets_only.w;
        generic_position_coord = &pos.x;
        generic_rect_coord = &rect.x;
        rect_length = &rect.w;
        rect_opposite_length = &rect.h;
        generic_length = &size.w;
    }

    pos = automaticallyAddOrRemoveScrollBars(dc, rect, m_size);

    i32 child_count = m_visible_children - generic_non_expandable_widgets;
    if (!child_count) {
        child_count = 1; // Protects from division by zero
    }
    i32 expandable_length = (*rect_length - generic_total_layout_length) / child_count;
    i32 remainder = (*rect_length - generic_total_layout_length) % child_count;
    if (expandable_length < 0) {
        expandable_length = 0;
        remainder = 0;
    }

    size_t scroll_offset = 0;
    if (m_align_policy == Align::Vertical) {
        scroll_offset = (m_vertical_scrollbar->isVisible() ? m_vertical_scrollbar->m_slider->m_value : 0.0) * (m_size.h - rect.h);
    } else {
        scroll_offset = (m_horizontal_scrollbar->isVisible() ? m_horizontal_scrollbar->m_slider->m_value : 0.0) * (m_size.w - rect.w);
    }
    BinarySearchResult<Widget*> result = binarySearch(scroll_offset);
    if (result.value) {
        size_t i = result.index;
        Widget *child = children[i];
        // Move position to the start of the first visible widget.
        *generic_position_coord += m_children_positions[i].position;
        size_t expandable_coord = 0;
        while (true) {
            Size child_hint = child->sizeHint(dc);
            size = calculateChildSize(child_hint, expandable_length, *rect_opposite_length, child, remainder);
            Rect widget_rect = Rect(pos.x, pos.y, size.w, size.h);
            if (child->isVisible()) {
                if (expandable_length > 0) {
                    m_children_positions[i] = BinarySearchData{ expandable_coord, (size_t)*generic_length };
                    expandable_coord += *generic_length;
                }
                child->draw(dc, widget_rect, child->state());
                if ((*generic_position_coord + *generic_length) >= (*generic_rect_coord + *rect_length)) {
                    break;
                }
                *generic_position_coord += *generic_length;
            } else { m_children_positions[i] = BinarySearchData{ expandable_coord, 0 }; }
            i++;
            child = children[i];
            if (i == children.size()) { break; }
        }
        drawScrollBars(dc, rect, m_size);
    }
}

Size ScrolledBox::sizeHint(DrawingContext &dc) {
    u32 visible = 0;
    u32 vertical_non_expandable = 0;
    u32 horizontal_non_expandable = 0;
    if (m_size_changed) {
        Scrollable::sizeHint(dc);
        m_children_positions.clear();
        m_children_positions.resize(children.size());
        Size size = Size();
        if (m_align_policy == Align::Horizontal) {
            size_t scroll_offset = 0;
            size_t i = 0;
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
                    m_children_positions[i] = BinarySearchData{scroll_offset, (size_t)s.w};
                    scroll_offset += s.w;
                } else {
                    m_children_positions[i] = BinarySearchData{scroll_offset, 0};
                }
                i++;
            }
        } else {
            size_t scroll_offset = 0;
            size_t i = 0;
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
                    m_children_positions[i] = BinarySearchData{scroll_offset, (size_t)s.h};
                    scroll_offset += s.h;
                } else {
                    m_children_positions[i] = BinarySearchData{scroll_offset, 0};
                }
                i++;
            }
        }
        m_vertical_non_expandable = vertical_non_expandable;
        m_horizontal_non_expandable = horizontal_non_expandable;
        m_visible_children = visible;
        m_size = size;
        m_widgets_only = size;
        m_size_changed = false;

        return m_viewport;
    } else {
        return m_viewport;
    }
}

bool ScrolledBox::handleScrollEvent(ScrollEvent event) {
    SDL_Keymod mod = SDL_GetModState();
    if (mod & Mod::Shift) {
        if (m_horizontal_scrollbar->isVisible()) {
            return m_horizontal_scrollbar->m_slider->handleScrollEvent(event);
        }
    } else {
        if (m_vertical_scrollbar->isVisible()) {
            return m_vertical_scrollbar->m_slider->handleScrollEvent(event);
        }
    }
    return false;
}

void ScrolledBox::setAlignPolicy(Align align_policy) {
    if (m_align_policy != align_policy) {
        m_align_policy = align_policy;
        layout();
    }
}

Align ScrolledBox::alignPolicy() {
    return m_align_policy;
}

Size ScrolledBox::calculateChildSize(Size child_hint, i32 expandable_length, i32 rect_opposite_length, Widget *child, i32 &remainder) {
    Size size;
    i32 child_expandable_length = expandable_length;
    switch (m_align_policy) {
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
    return size;
}

BinarySearchResult<Widget*> ScrolledBox::binarySearch(size_t target) {
    if (!children.size()) { return BinarySearchResult<Widget*>{ 0, Option<Widget*>() }; }
    size_t lower = 0;
    size_t upper = children.size() - 1;
    size_t mid = 0;
    BinarySearchData point = {0, 0};

    while (lower <= upper) {
        mid = (lower + upper) / 2;
        point = m_children_positions[mid];
        if (target < point.position) {
            upper = mid - 1;
        } else if (target > point.position + point.length) {
            lower = mid + 1;
        } else {
            break;
        }
    }

    if (point.position <= target && point.position + point.length >= target) {
       return BinarySearchResult<Widget*>{ mid, Option<Widget*>(children[mid]) };
    }

    return BinarySearchResult<Widget*>{ 0, Option<Widget*>() };
}

Widget* ScrolledBox::propagateMouseEvent(Window *window, State *state, MouseEvent event) {
    if (m_vertical_scrollbar->isVisible()) {
        if ((event.x >= m_vertical_scrollbar->rect.x && event.x <= m_vertical_scrollbar->rect.x + m_vertical_scrollbar->rect.w) &&
            (event.y >= m_vertical_scrollbar->rect.y && event.y <= m_vertical_scrollbar->rect.y + m_vertical_scrollbar->rect.h)) {
            return m_vertical_scrollbar->propagateMouseEvent(window, state, event);
        }
    }
    if (m_horizontal_scrollbar->isVisible()) {
        if ((event.x >= m_horizontal_scrollbar->rect.x && event.x <= m_horizontal_scrollbar->rect.x + m_horizontal_scrollbar->rect.w) &&
            (event.y >= m_horizontal_scrollbar->rect.y && event.y <= m_horizontal_scrollbar->rect.y + m_horizontal_scrollbar->rect.h)) {
            return m_horizontal_scrollbar->propagateMouseEvent(window, state, event);
        }
    }
    if (m_vertical_scrollbar->isVisible() && m_horizontal_scrollbar->isVisible()) {
        if ((event.x > m_horizontal_scrollbar->rect.x + m_horizontal_scrollbar->rect.w) &&
            (event.y > m_vertical_scrollbar->rect.y + m_vertical_scrollbar->rect.h)) {
            if (state->hovered) {
                state->hovered->onMouseLeft.notify(this, event);
            }
            state->hovered = nullptr;
            update();
            return nullptr;
        }
    }

    if (m_align_policy == Align::Vertical) {
        size_t scroll_offset = (m_vertical_scrollbar->isVisible() ? m_vertical_scrollbar->m_slider->m_value : 0.0) * (m_size.h - rect.h);
        Option<Widget*> child = binarySearch((event.y - rect.y) + scroll_offset).value;
        if (child &&
            ((event.x >= child.value->rect.x && event.x <= child.value->rect.x + child.value->rect.w) &&
            (event.y >= child.value->rect.y && event.y <= child.value->rect.y + child.value->rect.h))) {
            if (child.value->isLayout()) {
                child.value->propagateMouseEvent(window, state, event);
                return child.value;
            }
            child.value->handleMouseEvent(window, state, event);
            return child.value;
        }
    } else {
        size_t scroll_offset = (m_horizontal_scrollbar->isVisible() ? m_horizontal_scrollbar->m_slider->m_value : 0.0) * (m_size.w - rect.w);
        Option<Widget*> child = binarySearch((event.x - rect.x) + scroll_offset).value;
        if (child &&
            ((event.x >= child.value->rect.x && event.x <= child.value->rect.x + child.value->rect.w) &&
            (event.y >= child.value->rect.y && event.y <= child.value->rect.y + child.value->rect.h))) {
            if (child.value->isLayout()) {
                child.value->propagateMouseEvent(window, state, event);
                return child.value;
            }
            child.value->handleMouseEvent(window, state, event);
            return child.value;
        }
    }

    handleMouseEvent(window, state, event);
    return this;
}
