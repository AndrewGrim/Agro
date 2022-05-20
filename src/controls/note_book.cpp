#include "note_book.hpp"
#include "../application.hpp"
#include "widget.hpp"

NoteBookTabBar::NoteBookTabBar(Widget *notebook_parent) : Widget(), m_notebook_parent{notebook_parent} {
    setBorderType(STYLE_BOTTOM);
}

NoteBookTabBar::~NoteBookTabBar() {
    delete m_horizontal_scrollbar;
}

void NoteBookTabBar::draw(DrawingContext &dc, Rect rect, i32 state) {
    this->rect = rect;

    dc.drawBorder(rect, style(), state);
    rect.x += 5; // account for the 5 pixels on the left to give the first tab some breathing room
    rect.w -= 5; // account for the 5 pixels on the left to give the first tab some breathing room
    i32 x = rect.x;

    if (rect.w < m_size.w) {
        if (!m_horizontal_scrollbar) {
            m_horizontal_scrollbar = new SimpleScrollBar(Align::Horizontal, Size(3, 3));
        }
        x -= m_horizontal_scrollbar->m_slider->m_value * (m_size.w - rect.w);
    } else {
        if (m_horizontal_scrollbar) {
            delete m_horizontal_scrollbar;
            m_horizontal_scrollbar = nullptr;
        }
    }

    for (Widget *child : children) {
        Size child_hint = child->sizeHint(dc);
        Rect child_rect = Rect(x, rect.y, child_hint.w, rect.h);
        if (x + child_hint.w < rect.x) {
            child->rect = child_rect;
        } else {
            child->draw(dc, child_rect, child->state());
            if ((x + child_hint.w) > (rect.x + rect.w)) {
                break;
            }
        }
        x += child_hint.w;
    }

    if (m_horizontal_scrollbar) {
        Size scroll_size = m_horizontal_scrollbar->sizeHint(dc);
        i32 slider_size = rect.w * ((rect.w - scroll_size.w / 2) / m_size.w);
        if (slider_size < 20) {
            slider_size = 20;
        } else if (slider_size > (rect.w - 10)) {
            slider_size = rect.w - 10;
        }
        m_horizontal_scrollbar->m_slider->m_slider_button_size = slider_size;
        m_horizontal_scrollbar->draw(
            dc,
            Rect(
                rect.x - 5, // account for the 5 pixel padding on the left
                (rect.y + rect.h) - scroll_size.h,
                rect.w + 5 > scroll_size.w ? rect.w + 5 : scroll_size.w, // account for the 5 pixel padding on the left
                scroll_size.h
            ),
            m_horizontal_scrollbar->state()
        );
    }
}

const char* NoteBookTabBar::name() {
    return "NoteBookTabBar";
}

Size NoteBookTabBar::sizeHint(DrawingContext &dc) {
    if (m_size_changed) {
        Size size = Size(5, 0); // 5 extra pixels so the first tab button doesnt look weird
        dc.sizeHintBorder(size, style());
        for (Widget *child : children) {
            Size s = child->sizeHint(dc);
            size.w += s.w;
            if (s.h > size.h) {
                size.h = s.h;
            }
        }

        m_size = size;
        m_size_changed = false;
        return size;
    } else {
        return m_size;
    }
}

bool NoteBookTabBar::isLayout() {
    return true;
}

i32 NoteBookTabBar::isFocusable() {
    return (i32)FocusType::Passthrough;
}

Widget* NoteBookTabBar::handleFocusEvent(FocusEvent event, State *state, FocusPropagationData data) {
    if (data.origin == children[((NoteBook*)m_notebook_parent)->currentTab()]) {
        assert(event == FocusEvent::Reverse && "Got invalid focus event for this scenario!");
        return m_notebook_parent->handleFocusEvent(event, state, FocusPropagationData(this, Option<i32>()));
    } else {
        return children[((NoteBook*)m_notebook_parent)->currentTab()]->handleFocusEvent(event, state, data);
    }
    return nullptr;
}

Widget* NoteBookTabBar::propagateMouseEvent(Window *window, State *state, MouseEvent event) {
    if (m_horizontal_scrollbar) {
        if ((event.x >= m_horizontal_scrollbar->rect.x && event.x <= m_horizontal_scrollbar->rect.x + m_horizontal_scrollbar->rect.w) &&
            (event.y >= m_horizontal_scrollbar->rect.y && event.y <= m_horizontal_scrollbar->rect.y + m_horizontal_scrollbar->rect.h)) {
            return m_horizontal_scrollbar->propagateMouseEvent(window, state, event);
        }
    }
    for (Widget *child : children) {
        if ((event.x >= child->rect.x && event.x <= child->rect.x + child->rect.w) &&
            (event.y >= child->rect.y && event.y <= child->rect.y + child->rect.h)) {
            Widget *last = nullptr;
            if (child->isLayout()) {
                last = child->propagateMouseEvent(window, state, event);
            } else {
                child->handleMouseEvent(window, state, event);
                last = child;
            }
            return last;
        }
    }

    this->handleMouseEvent(window, state, event);
    return this;
}

bool NoteBookTabBar::handleScrollEvent(ScrollEvent event) {
    if (this->m_horizontal_scrollbar) {
        return this->m_horizontal_scrollbar->m_slider->handleScrollEvent(event);
    }
    return false;
}

NoteBookTabButton::NoteBookTabButton(NoteBook *notebook, String text, Image *image,  bool close_button) : Button(text) {
    // TODO holy shit this is some bad and brittle code!
    if (image) {
        setImage(image);
    }
    setCloseButton(close_button);
    m_close_image = new IconButton((new Image(Application::get()->icons["close_thin"]))->setMinSize(Size(12, 12)));
    m_close_image->onMouseClick.addEventListener([=](Widget *widget,MouseEvent event) {
        notebook->destroyTab(m_close_image->parent->parent_index);
    });
    m_close_image->setBorderType(STYLE_NONE);
    m_close_image->setMarginType(STYLE_NONE);
    m_close_image->setPaddingType(STYLE_NONE);
    m_close_image->setWidgetBackgroundColor(COLOR_NONE);
    append(m_close_image);

    // Setup the border to get the correct sizeHint.
    DrawingContext &dc = *Application::get()->currentWindow()->dc;
    setBorderType(STYLE_TOP|STYLE_LEFT|STYLE_RIGHT);
    setBorderTop(dc.default_style.border.top * 2 > m_min_tab_accent_width ? dc.default_style.border.top * 2 : m_min_tab_accent_width);

    bind(SDLK_LEFT, Mod::None, [&]{
        if (parent->children.size()) {
            if (parent_index) {
                parent->children[parent_index - 1]->activate();
            } else if (parent_index == 0) {
                parent->children[parent->children.size() - 1]->activate();
            }
        }
    });
    bind(SDLK_RIGHT, Mod::None, [&]{
        if (parent->children.size()) {
            if (parent_index < (i32)parent->children.size() - 1) {
                parent->children[parent_index + 1]->activate();
            } else if (parent_index == (i32)parent->children.size() - 1) {
                parent->children[0]->activate();
            }
        }
    });
}

NoteBookTabButton::~NoteBookTabButton() {

}

const char* NoteBookTabButton::name() {
    return "NoteBookTabButton";
}

void NoteBookTabButton::draw(DrawingContext &dc, Rect rect, i32 state) {
    this->rect = rect;
    Color color;

    if (isActive()) {
        style().border_color.top = Application::get()->currentWindow()->dc->accentWidgetBackground(style());
        // We draw over the border of the NoteBookTabBar to emulate
        // whan an open page in a folder would look like
        // so it looks like the first one
        //   _____       _____
        // __|   |__   __|___|__
        rect.h += parent->borderBottom();
        color = dc.windowBackground(style());
    } else if (state & STATE_PRESSED && state & STATE_HOVERED) {
        color = dc.pressedBackground(style());
    } else if (state & STATE_HOVERED) {
        color = dc.hoveredBackground(style());
    } else {
        color = dc.windowBackground(style());
    }

    if (isActive()) {
        dc.drawBorder(rect, style(), state);
    }
    Rect focus_rect = rect;
    dc.fillRect(rect, color);
    dc.padding(rect, style());

    Size text_size = dc.measureText(font(), text());
    if (this->m_image) {
        Size image_size = m_image->sizeHint(dc);
        i32 width = rect.w;
        if (m_close_button) {
            width -= 22; // 12 icon size + 10 padding
        }
        dc.drawTexture(
            Point(
                rect.x + (width / 2 - text_size.w / 2) - image_size.w / 2,
                rect.y + (rect.h * 0.5) - (image_size.h * 0.5)
            ),
            image_size,
            m_image->_texture(),
            m_image->coords(),
            m_image->foreground()
        );
        // Resize rect to account for image before the label is drawn.
        rect.x += image_size.w;
        rect.w -= image_size.w;
        if (m_close_button) {
            rect.w -= 22; // 12 icon size + 10 padding
        }
    }
    HorizontalAlignment h_text_align = m_horizontal_align;
    VerticalAlignment v_text_align = m_vertical_align;
    if (m_image) {
        h_text_align = HorizontalAlignment::Center;
        v_text_align = VerticalAlignment::Center;
    }
    if (m_text.size()) {
        if (m_close_button && !m_image) {
            rect.w -= 22; // 12 icon size + 10 padding
        }
        dc.fillTextAligned(
            font(),
            m_text,
            h_text_align,
            v_text_align,
            rect,
            0,
            dc.textForeground(style())
        );
        rect.x += text_size.w;
    }

    if (m_close_button) {
        // 12 being the size of the icon in pixels
        m_close_image->draw(dc, Rect(rect.x + 10 + (paddingRight() / 2), rect.y + (rect.h / 2) - (12 / 2), 12, 12), m_close_image->state());
    }

    dc.drawKeyboardFocus(focus_rect, style(), state);
}

Size NoteBookTabButton::sizeHint(DrawingContext &dc) {
    if (this->m_size_changed) {
        Size size = dc.measureText(font(), text());
        if (m_image) {
            Size i = m_image->sizeHint(dc);
            size.w += i.w;
            if (i.h > size.h) {
                size.h = i.h;
            }
        }

        dc.sizeHintBorder(size, style());
        dc.sizeHintPadding(size, style());

        // Account for the close button if present;
        if (m_close_button) {
            if (size.h < 12) {
                size.h = 12;
            }
            size.w += 22; // 10px padding between text and close button + 12px size of close button
        }

        this->m_size = size;
        this->m_size_changed = false;

        return size;
    } else {
        return this->m_size;
    }
}

bool NoteBookTabButton::isLayout() {
    return true;
}

i32 NoteBookTabButton::isFocusable() {
    return (i32)FocusType::Focusable;
}

Widget* NoteBookTabButton::handleFocusEvent(FocusEvent event, State *state, FocusPropagationData data) {
    if (data.origin == this) {
        assert(parent && "NoteBookTabButton should always have a parent!");
        if (event == FocusEvent::Forward) {
            return ((NoteBookTabBar*)parent)->m_notebook_parent->children[parent_index]->handleFocusEvent(event, state, FocusPropagationData());
        } else {
            return parent->handleFocusEvent(event, state, FocusPropagationData(this, parent_index));
        }
    } else {
        return setSoftFocus(event, state);
    }
    return nullptr;
}

Widget* NoteBookTabButton::propagateMouseEvent(Window *window, State *state, MouseEvent event) {
    if ((event.x >= m_close_image->rect.x && event.x <= m_close_image->rect.x + m_close_image->rect.w) &&
        (event.y >= m_close_image->rect.y && event.y <= m_close_image->rect.y + m_close_image->rect.h)) {
        m_close_image->handleMouseEvent(window, state, event);
        return m_close_image;
    }

    this->handleMouseEvent(window, state, event);
    return this;
}

bool NoteBookTabButton::isActive() {
    return m_is_active;
}

void NoteBookTabButton::setActive(bool is_active) {
    if (m_is_active != is_active) {
        m_is_active = is_active;
        update();
    }
}

bool NoteBookTabButton::hasCloseButton() {
    return m_close_button;
}

void NoteBookTabButton::setCloseButton(bool close_button) {
    if (m_close_button != close_button) {
        m_close_button = close_button;
        layout(LAYOUT_CHILD);
    }
}

bool NoteBookTabButton::handleLayoutEvent(LayoutEvent event) {
    if (event) {
        if (event & LAYOUT_BORDER) {
            DrawingContext &dc = *Application::get()->currentWindow()->dc;
            style().border.top = dc.default_style.border.top * 2 > m_min_tab_accent_width ? dc.default_style.border.top * 2 : m_min_tab_accent_width;
        }
        if (m_size_changed) { return true; }
        m_size_changed = true;
    }
    return false;
}

NoteBook::NoteBook() {

}

NoteBook::~NoteBook() {
    delete m_tabs;
}

void NoteBook::draw(DrawingContext &dc, Rect rect, i32 state) {
    this->rect = rect;
    if (m_tabs->children.size()) {
        // NoteBookTabBar.
        Size tab_bar_size = m_tabs->sizeHint(dc);
        m_tabs->draw(dc, Rect(rect.x, rect.y, rect.w, tab_bar_size.h), m_tabs->state());
        rect.y += tab_bar_size.h;
        rect.h -= tab_bar_size.h;

        // Tab content.
        this->children[m_tab_index]->draw(dc, rect, children[m_tab_index]->state());
    }
}

const char* NoteBook::name() {
    return "NoteBook";
}

Size NoteBook::sizeHint(DrawingContext &dc) {
    if (m_size_changed) {
        Size size = m_tabs->sizeHint(dc);
        if (this->children.size()) {
            Size current_tab_size = this->children[m_tab_index]->sizeHint(dc);
            size.h += current_tab_size.h;
            size.w = current_tab_size.w;
        }
        m_size = size;
        m_size_changed = false;
        return size;
    } else {
        return m_size;
    }
}

// TODO maybe overwrite append just like TreeView
// internally we could still use Widget::append
NoteBook* NoteBook::appendTab(Widget *root, String text, Image *icon, bool close_button) {
    this->append(root, Fill::Both);
    NoteBookTabButton *tab_button = new NoteBookTabButton(this, text, icon, close_button);
    tab_button->onMouseDown.addEventListener([=](Widget *widget, MouseEvent event) {
        this->setCurrentTab(tab_button->parent_index);
    });

    m_tabs->append(tab_button, Fill::Both);
    if (this->children.size() == 1) {
        setCurrentTab(0);
        layout(LAYOUT_STYLE);
    } else {
        update();
    }
    return this;
}

// NoteBook* NoteBook::insertTab(Widget *root, String text, Image *icon) {
//     // TODO implement
//     return this;
// }

NoteBook* NoteBook::destroyTab(u64 index) {
    if (children.size() > 1) { // TODO this should not be a library level thing, should be up to the user
        if (index < this->children.size()) {
            m_tabs->children[index]->destroy();
            this->children[index]->destroy();
            if (index < m_tab_index) {
                m_tab_index -= 1;
            } else if (index == m_tab_index) {
                if (currentTab() < this->children.size()) {
                    this->setCurrentTab(currentTab());
                } else if (currentTab() - 1 < this->children.size()) {
                    this->setCurrentTab(currentTab() - 1);
                } else {
                    this->setCurrentTab(0);
                }
            }
        }
    }

    return this;
}

// NoteBook* NoteBook::setTabWidget(Widget *root) {
//     // TODO implement
//     return this;
// }

u64 NoteBook::currentTab() {
    return m_tab_index;
}

NoteBook* NoteBook::setCurrentTab(u64 index) {
    if (m_tab_index < children.size()) {
        ((NoteBookTabButton*)m_tabs->children[m_tab_index])->setActive(false);
    }
    if (index < children.size()) {
        ((NoteBookTabButton*)m_tabs->children[index])->setActive(true);
        m_tabs->children[index]->setHardFocus(Application::get()->currentWindow()->m_state);
    }
    m_tab_index = index;
    layout(LAYOUT_STYLE);

    return this;
}

bool NoteBook::isLayout() {
    return true;
}

i32 NoteBook::isFocusable() {
    return (i32)FocusType::Passthrough;
}

Widget* NoteBook::handleFocusEvent(FocusEvent event, State *state, FocusPropagationData data) {
    if (data.origin == m_tabs) {
        assert(event == FocusEvent::Reverse && "Got invalid focus event for this scenario!");
    } else if (data.origin == children[currentTab()]) {
        if (event == FocusEvent::Reverse) {
            return m_tabs->children[currentTab()]->handleFocusEvent(event, state, data);
        }
    } else {
        if (event == FocusEvent::Reverse) {
            return children[currentTab()]->handleFocusEvent(event, state, data);
        }
        return m_tabs->handleFocusEvent(event, state, data);
    }
    if (parent) {
        return parent->handleFocusEvent(event, state, FocusPropagationData(this, parent_index));
    }
    return nullptr;
}

Widget* NoteBook::propagateMouseEvent(Window *window, State *state, MouseEvent event) {
    // Check event against NoteBookTabBar.
    if ((event.x >= m_tabs->rect.x && event.x <= m_tabs->rect.x + m_tabs->rect.w) &&
        (event.y >= m_tabs->rect.y && event.y <= m_tabs->rect.y + m_tabs->rect.h)) {
        return m_tabs->propagateMouseEvent(window, state, event);
    }

    // Check event against the tab content itself.
    Widget *child = this->children[m_tab_index];
    if ((event.x >= child->rect.x && event.x <= child->rect.x + child->rect.w) &&
        (event.y >= child->rect.y && event.y <= child->rect.y + child->rect.h)) {
        Widget *last = nullptr;
        if (child->isLayout()) {
            last = child->propagateMouseEvent(window, state, event);
        } else {
            child->handleMouseEvent(window, state, event);
            last = child;
        }
        return last;
    }

    this->handleMouseEvent(window, state, event);
    return this;
}

void NoteBook::forEachDrawable(std::function<void(Drawable *drawable)> action) {
    action(this);
    for (Widget *child : children) {
        child->forEachDrawable(action);
    }
    action(m_tabs);
    for (Widget *tab : m_tabs->children) {
        tab->forEachDrawable(action);
    }
}
