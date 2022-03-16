#include "drop_down.hpp"

DropDownList::DropDownList(Size min_size) : Scrollable(min_size) {
    onMouseDown.addEventListener([&](Widget *widget, MouseEvent event) {
        DrawingContext &dc = *Application::get()->dc;
        int y = rect.y - (m_vertical_scrollbar ? m_vertical_scrollbar->m_slider->m_value : 0.0) * ((m_size.h) - rect.h);
        int index = 0;
        for (Drawable *item : m_items) {
            Size item_size = item->sizeHint(dc);
            if (event.y >= y && event.y <= y + item_size.h) {
                m_focused = index;
                ((DropDown*)parent)->setCurrent(index);
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
        for (Drawable *item : m_items) {
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

DropDownList::~DropDownList() {
    for (Drawable *item : m_items) {
        delete item;
    }
}

const char* DropDownList::name() {
    return "DropDownList";
}

void DropDownList::draw(DrawingContext &dc, Rect rect, int state) {
    this->rect = rect;
    Rect previous_clip = dc.clip();
    clip();

    // TODO we draw the items over the border
    dc.drawBorder(rect, style, state);
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

Size DropDownList::sizeHint(DrawingContext &dc) {
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

int DropDownList::isFocusable() {
    return (int)FocusType::Focusable;
}

DropDown::DropDown() {
    append(m_list);
    onMouseDown.addEventListener([&](Widget *widget, MouseEvent event) {
        if (!m_is_open) {
            m_open_close->flipVertically();
            Window *current = Application::get()->getCurrentWindow();
            int x, y;
            SDL_GetWindowPosition(current->m_win, &x, &y);
            m_window = new Window(
                m_list->name(),
                Size(rect.w, m_list->m_viewport.h),
                Point(x + rect.x, y + rect.y + rect.h),
                SDL_WINDOW_OPENGL | SDL_WINDOW_RESIZABLE | SDL_WINDOW_BORDERLESS
            );
                m_window->is_owned = true;
                m_window->dc->default_font = current->dc->default_font;
                m_window->dc->default_style = current->dc->default_style;
                m_window->dc->default_light_style = current->dc->default_light_style;
                m_window->dc->default_dark_style = current->dc->default_dark_style;
            delete m_window->mainWidget();
            m_window->setMainWidget(m_list);
            m_window->onFocusLost = [&](Window *win) {
                m_open_close->flipVertically();
                m_window->m_main_widget = nullptr;
                m_window = nullptr;
                win->is_owned = false;
                win->quit();
                m_is_open = false;
            };
            m_window->run();
        }
        m_is_open = !m_is_open;
    });
    bind(SDLK_SPACE, Mod::None, [&](){
        activate();
    });
}

DropDown::~DropDown() {
    if (m_is_open) { m_window->onFocusLost(m_window); }
    delete m_open_close;
}

const char* DropDown::name() {
    return "DropDown";
}

void DropDown::draw(DrawingContext &dc, Rect rect, int state) {
    Color color;
    if (state & STATE_PRESSED && state & STATE_HOVERED) {
        color = dc.pressedBackground(style);
    } else if (state & STATE_HOVERED) {
        color = dc.hoveredBackground(style);
    } else {
        color = dc.widgetBackground(style);
    }

    dc.margin(rect, style);
    this->rect = rect;
    dc.drawBorder(rect, style, state);
    Rect focus_rect = rect;
    dc.fillRect(rect, color);
    dc.padding(rect, style);

    Size size = m_open_close->sizeHint(dc);
    rect.w -= size.w;
    if (m_list->m_items.size() && m_current > -1) {
        Drawable *item = getItem(m_current);
        item->draw(dc, rect, STATE_DEFAULT);
    }

    rect.x += rect.w;
    rect.w = size.w;
    dc.drawTextureAligned(
        rect,
        size,
        m_open_close->_texture(),
        m_open_close->coords(),
        HorizontalAlignment::Right,
        VerticalAlignment::Center,
        dc.iconForeground(style)
    );

    dc.drawKeyboardFocus(focus_rect, style, state);
}

Size DropDown::sizeHint(DrawingContext &dc) {
    if (m_size_changed) {
        Size size = m_biggest_item;
        size.w += m_open_close->m_size.w;
        if (m_open_close->m_size.h > size.h) { size.h = m_open_close->m_size.h; }

        dc.sizeHintMargin(size, style);
        dc.sizeHintBorder(size, style);
        dc.sizeHintPadding(size, style);

        m_size = size;
        m_size_changed = false;

        return size;
    } else {
        return m_size;
    }
}

int DropDown::isFocusable() {
    return (int)FocusType::Focusable;
}

int DropDown::current() {
    return m_current;
}

void DropDown::setCurrent(int index) {
    m_current = index;
    if (m_is_open) { m_window->onFocusLost(m_window); }
    onItemSelected.notify(this, getItem(index), index);
    update();
}

int DropDown::appendItem(Drawable *drawable) {
    m_list->m_items.push_back(drawable);
    DrawingContext &dc = *Application::get()->getCurrentWindow()->dc;
    Size new_item_size = drawable->sizeHint(dc);
    bool should_layout = false;
    if (new_item_size.w > m_biggest_item.w) {
        m_biggest_item.w = new_item_size.w;
        should_layout = true;
    }
    if (new_item_size.h > m_biggest_item.h) {
        m_biggest_item.h = new_item_size.h;
        should_layout = true;
    }
    if (should_layout) { layout(); } else { update(); }
    return m_list->m_items.size() - 1;
}

Drawable* DropDown::getItem(int index) {
    return m_list->m_items[index];
}

void DropDown::clear() {
    for (Drawable *item : m_list->m_items) {
        delete item;
    }
    m_list->m_items.clear();
}
