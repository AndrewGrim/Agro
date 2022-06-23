#include "drop_down.hpp"

DropDown::DropDown(Cells items, Size min_size) {
    m_list = new List(items, min_size);
    m_list->onItemSelected.addEventListener([&](Widget *widget, CellRenderer *cell, i32 index) {
        setCurrent(index);
    });
    onMouseDown.addEventListener([&](Widget *widget, MouseEvent event) {
        if (m_must_close) {
            m_must_close = false;
        } else {
            m_open_close->flipVertically();
            Window *current = Application::get()->currentWindow();
            i32 x, y;
            SDL_GetWindowPosition(current->m_win, &x, &y);
            m_window = new Window(
                m_list->name(),
                Size(rect.w, m_list->m_viewport.h),
                // TODO make sure to try to create the window within the bounds of the screen
                // otherwise it will most likely be moved by the window manager
                Point(x + rect.x, y + rect.y + rect.h),
                SDL_WINDOW_OPENGL | SDL_WINDOW_BORDERLESS
            );
            m_window->is_owned = true;
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
                m_must_close = isHovered() ? true : false;
                Application::get()->currentWindow()->update();
            };
            m_window->run();
            SDL_RaiseWindow(m_window->m_win);
        }
    });
    bind(SDLK_SPACE, Mod::None, [&](){
        activate();
    });
}

DropDown::~DropDown() {
    if (m_window) { m_window->onFocusLost(m_window); }
    delete m_open_close;
    delete m_list;
}

const char* DropDown::name() {
    return "DropDown";
}

void DropDown::draw(DrawingContext &dc, Rect rect, i32 state) {
    Color color;
    if (state & STATE_PRESSED && state & STATE_HOVERED) {
        color = dc.pressedBackground(style());
    } else if (state & STATE_HOVERED) {
        color = dc.hoveredBackground(style());
    } else {
        color = dc.widgetBackground(style());
    }

    dc.margin(rect, style());
    this->rect = rect;
    dc.drawBorder(rect, style(), state);
    Rect focus_rect = rect;
    dc.fillRect(rect, color);
    dc.padding(rect, style());

    Size size = m_open_close->sizeHint(dc);
    rect.w -= size.w;
    if (m_list->m_items.data->size() && current() > -1) {
        CellRenderer *item = getItem(current());
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
        dc.iconForeground(style())
    );

    dc.drawKeyboardFocus(focus_rect, style(), state);
}

Size DropDown::sizeHint(DrawingContext &dc) {
    if (m_biggest_item.w < m_list->m_items.max_cell_size->w ||
        m_biggest_item.h < m_list->m_items.max_cell_size->h
    ) {
        m_biggest_item = *m_list->m_items.max_cell_size;
        m_size_changed = true;
    }
    if (m_size_changed) {
        Size size = m_biggest_item;
        size.w += m_open_close->m_size.w;
        if (m_open_close->m_size.h > size.h) { size.h = m_open_close->m_size.h; }

        dc.sizeHintMargin(size, style());
        dc.sizeHintBorder(size, style());
        dc.sizeHintPadding(size, style());

        m_size = size;
        m_size_changed = false;

        return size;
    } else {
        return m_size;
    }
}

i32 DropDown::isFocusable() {
    return (i32)FocusType::Focusable;
}

i32 DropDown::current() {
    return m_list->current();
}

void DropDown::setCurrent(i32 index) {
    m_list->m_focused = index;
    if (m_window) { m_window->onFocusLost(m_window); }
    onItemSelected.notify(this, getItem(index), index);
    update();
}

i32 DropDown::appendItem(CellRenderer *cell) {
    m_list->appendItem(cell);
    DrawingContext &dc = *Application::get()->currentWindow()->dc;
    Size new_item_size = cell->sizeHint(dc);
    bool should_layout = false;
    if (new_item_size.w > m_biggest_item.w) {
        m_biggest_item.w = new_item_size.w;
        should_layout = true;
    }
    if (new_item_size.h > m_biggest_item.h) {
        m_biggest_item.h = new_item_size.h;
        should_layout = true;
    }
    if (should_layout) { layout(LAYOUT_CHILD); } else { update(); }
    return m_list->m_items.data->size() - 1;
}

CellRenderer* DropDown::getItem(i32 index) {
    return m_list->getItem(index);
}

void DropDown::clear() {
    m_list->clear();
}
