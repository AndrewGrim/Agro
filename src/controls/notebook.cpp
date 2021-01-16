#include "notebook.hpp"

NoteBookTabBar::NoteBookTabBar() : Widget() {

}

NoteBookTabBar::~NoteBookTabBar() {
    delete m_horizontal_scrollbar;
}

void NoteBookTabBar::draw(DrawingContext *dc, Rect rect) {
    float x = rect.x;
    x -= m_horizontal_scrollbar->m_slider->m_value * (m_size.w - rect.w);

    for (Widget *child : children) {
        Size child_hint = child->sizeHint(dc);
        Rect child_rect = Rect(x, rect.y, child_hint.w, child_hint.h);
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

    // TODO change to simple scrollbar
    Size size = m_horizontal_scrollbar->sizeHint(dc);
    float slider_size = rect.w * ((rect.w - size.w / 2) / m_size.w);
    float buttons_size = m_horizontal_scrollbar->m_begin_button->sizeHint(dc).w + m_horizontal_scrollbar->m_end_button->sizeHint(dc).w;
    if (slider_size < 20) {
        slider_size = 20;
    } else if (slider_size > (rect.w - buttons_size - 10)) {
        slider_size = rect.w - buttons_size - 10;
    }
    m_horizontal_scrollbar->m_slider->m_slider_button_size = slider_size;
    m_horizontal_scrollbar->draw(dc, Rect(
        rect.x, 
        (rect.y + rect.h) - size.h, 
        rect.w > size.w ? rect.w : size.w, 
        size.h
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
            size.h += m_horizontal_scrollbar->sizeHint(dc).h;
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

NoteBook* NoteBook::appendTab(Widget *root, std::string text, Image *icon) {
    // TODO either override Widget::append or manually append in this method.
    this->append(root, Fill::Both);
    Button *tab_button = new Button(text);
    if (icon) {
        tab_button->setImage(icon);
    }
    m_tabs->append(tab_button);
    if (this->children.size() == 1) {
        m_size_changed = true;
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
        m_tab_index = index;
        m_size_changed = true;
        update();
        layout();
    }
    return this;
}
