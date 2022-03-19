#include "list.hpp"
#include "../application.hpp"

List::List(Size min_size) : Scrollable(min_size) {
    // TODO could do with binary search lookup
    onMouseDown.addEventListener([&](Widget *widget, MouseEvent event) {
        DrawingContext &dc = *Application::get()->dc;
        int y = rect.y - (m_vertical_scrollbar ? m_vertical_scrollbar->m_slider->m_value : 0.0) * ((m_size.h) - rect.h);
        int index = 0;
        for (CellRenderer *item : m_items) {
            Size item_size = item->sizeHint(dc);
            if (event.y >= y && event.y <= y + item_size.h) {
                setCurrent(index);
                return;
            }
            y += item_size.h;
            index++;
        }
        m_focused = -1;
    });
    onMouseMotion.addEventListener([&](Widget *widget, MouseEvent event) {
        DrawingContext &dc = *Application::get()->dc;
        int y = rect.y - (m_vertical_scrollbar ? m_vertical_scrollbar->m_slider->m_value : 0.0) * ((m_size.h) - rect.h);
        int index = 0;
        for (CellRenderer *item : m_items) {
            Size item_size = item->sizeHint(dc);
            if (event.y >= y && event.y <= y + item_size.h) {
                m_hovered = index;
                return;
            }
            y += item_size.h;
            index++;
        }
        m_hovered = -1;
    });
    onMouseLeft.addEventListener([&](Widget *widget, MouseEvent event) {
        m_hovered = -1;
    });
}

List::~List() {
    for (CellRenderer *item : m_items) {
        delete item;
    }
}

const char* List::name() {
    return "List";
}

void List::draw(DrawingContext &dc, Rect rect, int state) {
    this->rect = rect;
    Rect previous_clip = dc.clip();
    clip();

    dc.drawBorder(rect, style, state);
    dc.setClip(rect.clipTo(previous_clip));
    sizeHint(dc);
    Point pos;
    dc.fillRect(rect, dc.textBackground(style));
    pos = automaticallyAddOrRemoveScrollBars(dc, rect, m_size);
    int index = 0;
    for (Drawable *item : m_items) {
        Size item_size = item->sizeHint(dc);
        if (pos.y + item_size.h >= rect.y) {
            int item_state = STATE_DEFAULT;
            if (index == m_focused) { item_state |= STATE_HARD_FOCUSED; }
            if (index == m_hovered) { item_state |= STATE_HOVERED; }
            item->draw(dc, Rect(pos.x, pos.y, rect.w, item_size.h), item_state);
        }
        if (pos.y + item_size.h >= rect.y + rect.h) { break; }
        pos.y += item_size.h;
        index++;
    }
    drawScrollBars(dc, rect, m_size);

    dc.setClip(previous_clip);
}

Size List::sizeHint(DrawingContext &dc) {
    if (m_size_changed) {
        Size virtual_size = Size();
        for (Drawable *item : m_items) {
            Size item_size = item->sizeHint(dc);
            virtual_size.h += item_size.h;
            if (item_size.w > virtual_size.w) { virtual_size.w = item_size.w; }
        }
        m_size = virtual_size;

        Size viewport_and_style = m_viewport;
            dc.sizeHintBorder(viewport_and_style, style);
        return viewport_and_style;
    } else {
        Size viewport_and_style = m_viewport;
            dc.sizeHintBorder(viewport_and_style, style);
        return viewport_and_style;
    }
}

int List::isFocusable() {
    return (int)FocusType::Focusable;
}

int List::current() {
    return m_focused;
}

void List::setCurrent(int index) {
    m_focused = index;
    onItemSelected.notify(this, getItem(index), index);
    update();
}

int List::appendItem(CellRenderer *cell) {
    m_items.push_back(cell);
    return m_items.size() - 1;
}

CellRenderer* List::getItem(int index) {
    return m_items[index];
}

void List::clear() {
    for (CellRenderer *item : m_items) {
        delete item;
    }
    m_items.clear();
}
