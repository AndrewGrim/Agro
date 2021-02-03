#include "notebook.hpp"
#include "../app.hpp"

NoteBookTabBar::NoteBookTabBar() : Widget() {

}

NoteBookTabBar::~NoteBookTabBar() {
    delete m_horizontal_scrollbar;
}

void NoteBookTabBar::draw(DrawingContext *dc, Rect rect) {
    this->rect = rect;
    float x = rect.x;
    x -= m_horizontal_scrollbar->m_slider->m_value * (m_size.w - rect.w);

    Size scroll_size = m_horizontal_scrollbar->sizeHint(dc);
    for (Widget *child : children) {
        Size child_hint = child->sizeHint(dc);
        Rect child_rect = Rect(x, rect.y, child_hint.w, rect.h - scroll_size.h);
        if (x + child_hint.w < rect.x) {
            child->rect = child_rect;
        } else {
            child->draw(dc, child_rect);
            if ((x + child_hint.w) > (rect.x + rect.w)) {
                break;
            }
        }
        x += child_hint.w;
    }

    float slider_size = rect.w * ((rect.w - scroll_size.w / 2) / m_size.w);
    if (slider_size < 20) {
        slider_size = 20;
    } else if (slider_size > (rect.w - 10)) {
        slider_size = rect.w - 10;
    }
    m_horizontal_scrollbar->m_slider->m_slider_button_size = slider_size;
    m_horizontal_scrollbar->draw(dc, Rect(
        rect.x, 
        (rect.y + rect.h) - scroll_size.h, 
        rect.w > scroll_size.w ? rect.w : scroll_size.w, 
        scroll_size.h
    ));
}

const char* NoteBookTabBar::name() {
    return "NoteBookTabBar";
}

Size NoteBookTabBar::sizeHint(DrawingContext *dc) {
    // TODO probably need `|| ((Application*)this->app)->hasLayoutChanged()`
    // but for that to matter we will need the ability to change the tab
    // buttons at runtime
    if (m_size_changed) {
        Size size = Size();
        for (Widget *child : children) {
            Size s = child->sizeHint(dc);
            size.w += s.w;
            if (s.h > size.h) {
                size.h = s.h;
            }
        }
        size.h += m_horizontal_scrollbar->sizeHint(dc).h;
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

void* NoteBookTabBar::propagateMouseEvent(State *state, MouseEvent event) {
    if ((event.x >= m_horizontal_scrollbar->rect.x && event.x <= m_horizontal_scrollbar->rect.x + m_horizontal_scrollbar->rect.w) &&
        (event.y >= m_horizontal_scrollbar->rect.y && event.y <= m_horizontal_scrollbar->rect.y + m_horizontal_scrollbar->rect.h)) {
        return (void*)m_horizontal_scrollbar->propagateMouseEvent(state, event);
    }
    for (Widget *child : children) {
        if ((event.x >= child->rect.x && event.x <= child->rect.x + child->rect.w) &&
            (event.y >= child->rect.y && event.y <= child->rect.y + child->rect.h)) {
            void *last = nullptr;
            if (child->isLayout()) {
                last = (void*)child->propagateMouseEvent(state, event);
            } else {
                child->handleMouseEvent(state, event);
                last = (void*)child;
            }
            return last;
        }
    }

    this->handleMouseEvent(state, event);
    return this;
}

bool NoteBookTabBar::handleScrollEvent(ScrollEvent event) {
    if (this->m_horizontal_scrollbar) {
        return this->m_horizontal_scrollbar->m_slider->handleScrollEvent(event);
    }
    return false;
}

NoteBookTabButton::NoteBookTabButton(std::string text, Image *image,  bool close_button) : Button(text) {
    setBorderWidth(1);
    if (image) {
        setImage(image);
    }
    setCloseButton(close_button);
    m_close_image = (new Image("close_thin.png"))->setForeground(Color());
}

NoteBookTabButton::~NoteBookTabButton() {

}

const char* NoteBookTabButton::name() {
    return "NoteBookTabButton";
}

void NoteBookTabButton::draw(DrawingContext *dc, Rect rect) {
    this->rect = rect;
    Color color; 
    if (isActive()) {
        color = Color(0.5, 0.5, 0.5);
    } else if (this->isPressed() && this->isHovered()) {
        color = this->m_pressed_bg; 
    } else if (this->isHovered()) {
        color = this->m_hovered_bg;
    } else {
        color = this->background();
    }
    
    // Draw border and shrink rectangle to prevent drawing over the border
    dc->fillRect(rect, foreground());
    rect.shrink(m_border_width);

    dc->fillRect(rect, color);
    // Pad the rectangle with some empty space.
    rect.shrink(m_padding);
    Size text_size = dc->measureText(this->font() ? this->font() : dc->default_font, text());
    if (this->m_image) {
        Size image_size = m_image->sizeHint(dc);
        float width = rect.w;
        if (m_close_button) {
            width -= 22;
        }
        dc->drawImageAtSize(
            Point(
                round(rect.x + (width / 2 - text_size.w / 2) - image_size.w / 2), 
                round(rect.y + (rect.h * 0.5) - (image_size.h * 0.5))
            ),
            image_size,
            m_image,
            m_image->foreground()
        );
        // Resize rect to account for image before the label is drawn.
        rect.x += image_size.w;
        rect.w -= image_size.w;
        if (m_close_button) {
            rect.w -= 22;
        }
    }
    HorizontalAlignment h_text_align = m_horizontal_align;
    VerticalAlignment v_text_align = m_vertical_align;
    if (m_image) {
        h_text_align = HorizontalAlignment::Center;
        v_text_align = VerticalAlignment::Center;
    }
    if (this->m_text.length()) {
        dc->fillTextAligned(
            this->font() ? this->font() : dc->default_font,
            this->m_text,
            h_text_align,
            v_text_align,
            rect,
            0,
            this->m_fg
        );
        rect.x += text_size.w;
    }
    if (m_close_button) {
        dc->drawImageAligned(
            Rect(rect.x, rect.y, 22, rect.h),
            HorizontalAlignment::Center,
            VerticalAlignment::Center,
            m_close_image,
            m_close_image->foreground()
        );
    }
}

Size NoteBookTabButton::sizeHint(DrawingContext *dc) {
    if (this->m_size_changed) {
        Size size = dc->measureText(this->font() ? this->font() : dc->default_font, text());
        if (m_image) {
            Size i = m_image->sizeHint(dc);
            size.w += i.w;
            if (i.h > size.h) {
                size.h = i.h;
            }
        }
        // Note: full padding is applied on both sides but
        // border width is divided in half for each side
        size.w += this->m_padding * 2 + this->m_border_width;
        size.h += this->m_padding * 2 + this->m_border_width;

        // Account for the close button if present;
        if (m_close_button) {
            if (size.h < 12) {
                size.h = 12;
            }
            size.w += 22;
        }

        this->m_size = size;
        this->m_size_changed = false;

        return size;
    } else {
        return this->m_size;
    }
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
        m_size_changed = true;
        update();
        layout();
    }
}

NoteBook::NoteBook() {

}

NoteBook::~NoteBook() {

}

void NoteBook::draw(DrawingContext *dc, Rect rect) {
    this->rect = rect;
    if (m_tabs->children.size()) {
        // NoteBookTabBar.
        Size tab_bar_size = m_tabs->sizeHint(dc);
        m_tabs->draw(dc, Rect(rect.x, rect.y, rect.w, tab_bar_size.h));
        rect.y += tab_bar_size.h;
        rect.h -= tab_bar_size.h;

        // Tab content.
        this->children[m_tab_index]->draw(dc, rect);
    }
}

const char* NoteBook::name() {
    return "NoteBook";
}

Size NoteBook::sizeHint(DrawingContext *dc) {
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

NoteBook* NoteBook::appendTab(Widget *root, std::string text, Image *icon, bool close_button) {
    // We need to attach app at run time to new tabs / tab buttons.
    if (app) {
        root->app = app;
        root->attachApp(app);
    }
    this->append(root, Fill::Both);
    NoteBookTabButton *tab_button = new NoteBookTabButton(text, icon, close_button);
    tab_button->onMouseClick = [=](MouseEvent event) {
        this->setCurrentTab(tab_button->parent_index);
    };
    // We need to attach app at run time to new tabs / tab buttons.
    if (app) {
        tab_button->app = app;
        tab_button->attachApp(app);
    }
    m_tabs->append(tab_button, Fill::Both);
    if (this->children.size() == 1) {
        m_size_changed = true;
        tab_button->setActive(true);
        update();
        layout();
    } else {
        update();
    }
    return this;
}

NoteBook* NoteBook::insertTab(Widget *root, std::string text, Image *icon) {
    // TODO implement
    return this;
}

NoteBook* NoteBook::removeTab(int index) {
    // TODO implement
    return this;
}

NoteBook* NoteBook::setTabWidget(Widget *root) {
    // TODO implement
    return this;
}

NoteBook* NoteBook::setTabText(std::string text) {
    // TODO implement
    return this;
}

NoteBook* NoteBook::setTabIcon(Image *icon) {
    // TODO implement
    return this;
}

int NoteBook::currentTab() {
    return m_tab_index;
}

NoteBook* NoteBook::setCurrentTab(int index) {
    if (m_tab_index != index) {
        ((NoteBookTabButton*)m_tabs->children[m_tab_index])->setActive(false);
        ((NoteBookTabButton*)m_tabs->children[index])->setActive(true);
        m_tab_index = index;
        m_size_changed = true;
        update();
        layout();
    }
    return this;
}

bool NoteBook::isLayout() {
    return true;
}

void* NoteBook::propagateMouseEvent(State *state, MouseEvent event) {
    // Check event against NoteBookTabBar.
    if ((event.x >= m_tabs->rect.x && event.x <= m_tabs->rect.x + m_tabs->rect.w) &&
        (event.y >= m_tabs->rect.y && event.y <= m_tabs->rect.y + m_tabs->rect.h)) {
        return (void*)m_tabs->propagateMouseEvent(state, event);
    }

    // Check event against the tab content itself.
    Widget *child = this->children[m_tab_index];
    if ((event.x >= child->rect.x && event.x <= child->rect.x + child->rect.w) &&
        (event.y >= child->rect.y && event.y <= child->rect.y + child->rect.h)) {
        void *last = nullptr;
        if (child->isLayout()) {
            last = (void*)child->propagateMouseEvent(state, event);
        } else {
            child->handleMouseEvent(state, event);
            last = (void*)child;
        }
        return last;
    }

    this->handleMouseEvent(state, event);
    return this;
}

Widget* NoteBook::attachApp(void *app) {
    // Attach app to all tab roots.
    for (Widget *child : this->children) {
        child->app = app;
        child->attachApp(app);
    }

    // Attach app to scrollbar of NoteBookTabBar.
    m_tabs->m_horizontal_scrollbar->app = app;
    for (Widget *child : m_tabs->m_horizontal_scrollbar->children) {
        child->app = app;
        child->attachApp(app);
    }

    // Attach app to all buttons of NoteBookTabBar.
    m_tabs->app = app;
    for (Widget *child : m_tabs->children) {
        child->app = app;
        child->attachApp(app);
    }
    
    return this;
}