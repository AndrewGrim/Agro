#include "notebook.hpp"

NoteBook::NoteBook() {

}

NoteBook::~NoteBook() {
    for (NoteBookTab tab : m_tabs) {
        if (tab.icon) {
            delete tab.icon;
        }
    }
}

void NoteBook::draw(DrawingContext *dc, Rect rect) {
    this->rect = rect;
    for (NoteBookTab tab : m_tabs) {
        float tab_button_height = font() ? font()->max_height + 10 * 2 : dc->default_font->max_height + 10 * 2;
        if (tab.icon) {
            dc->drawImageAlignedAtSize(
                Rect(rect.x, rect.y, rect.w, tab_button_height), 
                HorizontalAlignment::Left, 
                VerticalAlignment::Center,
                Size(24, 24),
                tab.icon
            );
            // 34
            rect.x += 24; // Image width + padding.
        }
        float text_width = dc->measureText(font() ? font() : dc->default_font, tab.text).w + 10 * 2;
        dc->fillRect(Rect(rect.x, rect.y, text_width, tab_button_height), Color(0.2, 0.5, 0.2));
        dc->fillTextAligned(
            font() ? font() : dc->default_font,
            tab.text,
            HorizontalAlignment::Left,
            VerticalAlignment::Center,
            Rect(rect.x, rect.y, rect.w, tab_button_height),
            10,
            foreground()
        );
        rect.x += text_width; // The padding on the right
    }
    if (m_tabs.size()) {
        float tab_button_height = font() ? font()->max_height + 10 * 2 : dc->default_font->max_height + 10 * 2;
        m_tabs[m_tab_index].root->draw(
            dc, 
            Rect(
                this->rect.x, 
                tab_button_height,
                rect.w, 
                rect.h - tab_button_height
            )
        );
    }
}

const char* NoteBook::name() {
    return "NoteBook";
}

Size NoteBook::sizeHint(DrawingContext *dc) {
    if (m_size_changed) {
        // TODO temporary solution
        // i think ideally we should have a tabbar widget
        // which would take care of all the tab buttons and its size
        Size size = Size();
        size.h = font() ? font()->max_height : dc->default_font->max_height;
        size.h += 10 * 2;
        if (m_tabs.size()) {
            Size tab_size = m_tabs[m_tab_index].root->sizeHint(dc);
            size.h += tab_size.h;
            size.w = tab_size.w;
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
    m_tabs.push_back({root, text, icon});
    if (m_tabs.size() == 1) {
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
