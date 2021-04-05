#include "notebook.hpp"
#include "../application.hpp"

NoteBookTabBar::NoteBookTabBar() : Widget() {

}

NoteBookTabBar::~NoteBookTabBar() {
    delete m_horizontal_scrollbar;
}

void NoteBookTabBar::draw(DrawingContext *dc, Rect rect) {
    this->rect = rect;
    float x = rect.x;

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
            child->draw(dc, child_rect);
            if ((x + child_hint.w) > (rect.x + rect.w)) {
                break;
            }
        }
        x += child_hint.w;
    }

    if (m_horizontal_scrollbar) {
        Size scroll_size = m_horizontal_scrollbar->sizeHint(dc);
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
}

const char* NoteBookTabBar::name() {
    return "NoteBookTabBar";
}

Size NoteBookTabBar::sizeHint(DrawingContext *dc) {
    if (m_size_changed) {
        Size size = Size();
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

void* NoteBookTabBar::propagateMouseEvent(Window *window, State *state, MouseEvent event) {
    if (m_horizontal_scrollbar) {
        if ((event.x >= m_horizontal_scrollbar->rect.x && event.x <= m_horizontal_scrollbar->rect.x + m_horizontal_scrollbar->rect.w) &&
            (event.y >= m_horizontal_scrollbar->rect.y && event.y <= m_horizontal_scrollbar->rect.y + m_horizontal_scrollbar->rect.h)) {
            return (void*)m_horizontal_scrollbar->propagateMouseEvent(window, state, event);
        }
    }
    for (Widget *child : children) {
        if ((event.x >= child->rect.x && event.x <= child->rect.x + child->rect.w) &&
            (event.y >= child->rect.y && event.y <= child->rect.y + child->rect.h)) {
            void *last = nullptr;
            if (child->isLayout()) {
                last = (void*)child->propagateMouseEvent(window, state, event);
            } else {
                child->handleMouseEvent(window, state, event);
                last = (void*)child;
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

NoteBookTabButton::NoteBookTabButton(NoteBook *notebook, std::string text, Image *image,  bool close_button) : Button(text) {
    // TODO outdate beause of the new style
    // setBorderWidth(1);
    if (image) {
        setImage(image);
    }
    setCloseButton(close_button);
    m_close_image = (new Image("close_thin.png"))->setForeground(COLOR_BLACK);
    m_close_image->onMouseClick.addEventListener([=](Widget *widget,MouseEvent event) {
        notebook->destroyTab(m_close_image->parent->parent_index);
    });
    this->append(m_close_image);
    this->style.padding.type = STYLE_ALL;
    this->style.padding.top = 5;
    this->style.padding.bottom = 5;
    this->style.padding.left = 5;
    this->style.padding.right = 5;
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
        color = Color(0.5f, 0.5f, 0.5f);
    } else if (this->isPressed() && this->isHovered()) {
        color = this->m_pressed_bg; 
    } else if (this->isHovered()) {
        color = this->m_hovered_bg;
    } else {
        color = this->background();
    }
    
    dc->drawBorder(rect, style);
    dc->fillRect(rect, color);
    dc->padding(rect, style);

    Size text_size = dc->measureText(font(), text());
    if (this->m_image) {
        Size image_size = m_image->sizeHint(dc);
        float width = rect.w;
        if (m_close_button) {
            width -= 22;
        }
        dc->drawTexture(
            Point(
                round(rect.x + (width / 2 - text_size.w / 2) - image_size.w / 2), 
                round(rect.y + (rect.h * 0.5) - (image_size.h * 0.5))
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
            rect.w -= 22;
        }
    }
    HorizontalAlignment h_text_align = m_horizontal_align;
    VerticalAlignment v_text_align = m_vertical_align;
    if (m_image) {
        h_text_align = HorizontalAlignment::Center;
        v_text_align = VerticalAlignment::Center;
    }
    if (m_text.length()) {
        if (m_close_button && !m_image) {
            rect.w -= 22;
        }
        dc->fillTextAligned(
            font(),
            m_text,
            h_text_align,
            v_text_align,
            rect,
            0,
            m_fg
        );
        rect.x += text_size.w;
    }

    if (m_close_image) {
        // TODO would be nice if the icon lit up when being hover / active
        // 12 being the size of the icon in pixels
        m_close_image->draw(dc, Rect(rect.x + 10, rect.y + (rect.h / 2) - (12 / 2), 12, 12));
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
        
        dc->sizeHintBorder(size, style);
        dc->sizeHintPadding(size, style);

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

void* NoteBookTabButton::propagateMouseEvent(Window *window, State *state, MouseEvent event) {
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

// TODO maybe overwrite append just like TreeView
// internally we could still use Widget::append
NoteBook* NoteBook::appendTab(Widget *root, std::string text, Image *icon, bool close_button) {
    this->append(root, Fill::Both);
    NoteBookTabButton *tab_button = new NoteBookTabButton(this, text, icon, close_button);
    tab_button->onMouseClick.addEventListener([=](Widget *widget, MouseEvent event) {
        this->setCurrentTab(tab_button->parent_index);
    });

    m_tabs->append(tab_button, Fill::Both);
    if (this->children.size() == 1) {
        tab_button->setActive(true);
        layout();
    } else {
        update();
    }
    return this;
}

// NoteBook* NoteBook::insertTab(Widget *root, std::string text, Image *icon) {
//     // TODO implement
//     return this;
// }

NoteBook* NoteBook::destroyTab(int index) {
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

int NoteBook::currentTab() {
    return m_tab_index;
}

NoteBook* NoteBook::setCurrentTab(int index) {
    if (m_tab_index < children.size()) {
        ((NoteBookTabButton*)m_tabs->children[m_tab_index])->setActive(false);
    }
    if (index < children.size()) {
        ((NoteBookTabButton*)m_tabs->children[index])->setActive(true);
    }
    m_tab_index = index;
    layout();

    return this;
}

bool NoteBook::isLayout() {
    return true;
}

void* NoteBook::propagateMouseEvent(Window *window, State *state, MouseEvent event) {
    // Check event against NoteBookTabBar.
    if ((event.x >= m_tabs->rect.x && event.x <= m_tabs->rect.x + m_tabs->rect.w) &&
        (event.y >= m_tabs->rect.y && event.y <= m_tabs->rect.y + m_tabs->rect.h)) {
        return (void*)m_tabs->propagateMouseEvent(window, state, event);
    }

    // Check event against the tab content itself.
    Widget *child = this->children[m_tab_index];
    if ((event.x >= child->rect.x && event.x <= child->rect.x + child->rect.w) &&
        (event.y >= child->rect.y && event.y <= child->rect.y + child->rect.h)) {
        void *last = nullptr;
        if (child->isLayout()) {
            last = (void*)child->propagateMouseEvent(window, state, event);
        } else {
            child->handleMouseEvent(window, state, event);
            last = (void*)child;
        }
        return last;
    }

    this->handleMouseEvent(window, state, event);
    return this;
}