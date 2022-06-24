#include "combo_box.hpp"

ComboBox::ComboBox(std::function<bool(String query, CellRenderer *cell)> predicate, Cells items, Size min_size) : DropDown(items, min_size) {
    m_predicate = predicate;
    m_filter = new TextEdit("", "filter", TextEdit::Mode::SingleLine, min_size);
    m_filter->setMarginType(STYLE_NONE);
    m_filter->onTextChanged.addEventListener([&]() {
        m_list->m_query = m_filter->text();
    });
    m_filter->setFillPolicy(Fill::Horizontal);
    m_list->setFillPolicy(Fill::Both);
    m_list->m_predicate = predicate;
    onMouseDown.listeners.clear();
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
                Size(rect.w, m_list->m_viewport.h + m_filter->m_size.h),
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
            Box *box = new Box(Align::Vertical);
                box->setFillPolicy(Fill::Both);
                box->append(m_filter);
                box->append(m_list);
            m_window->setMainWidget(box);
            m_window->bind(SDLK_ESCAPE, Mod::None, [&]() {
                if (m_window) { m_window->onFocusLost(m_window); }
            });
            m_window->onFocusLost = [&, box](Window *win) {
                m_open_close->flipVertically();
                m_filter->setText("");
                box->remove(m_filter->parent_index);
                box->remove(m_list->parent_index);
                m_window = nullptr;
                win->is_owned = false;
                win->quit();
                m_must_close = isHovered() ? true : false;
                Application::get()->currentWindow()->update();
            };
            m_window->run();
            SDL_RaiseWindow(m_window->m_win);
            Application::get()->setCurrentWindow(m_window);
            m_filter->activate();
        }
    });
}

ComboBox::~ComboBox() {
    if (!m_window) { delete m_filter; }
}

const char* ComboBox::name() {
    return "ComboBox";
}

// TODO we could try to draw the filter box on top of the current item but maybe later
// and i think it would still hit the dropdown event wise so it would be all good
// but im not sure how that would work with onFocusLost?
void ComboBox::draw(DrawingContext &dc, Rect rect, i32 state) {
    DropDown::draw(dc, rect, state);
}
