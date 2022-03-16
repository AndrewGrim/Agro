#include "style_editor.hpp"
#include "line_edit.hpp"
#include "group_box.hpp"


StyleEditor::StyleEditor() : ScrolledBox(Align::Vertical) {
    Style &s = Application::get()->dc->default_style;

     Box *h_box = new Box(Align::Horizontal);
        Button *light = new Button("Light");
            light->onMouseClick.addEventListener([&](Widget *widget, MouseEvent event) {
                for (Window *window : Application::get()->m_windows) {
                    window->dc->default_style = Application::get()->dc->default_light_style;
                    window->layout();
                    window->update();
                }
            });
        h_box->append(light);
        Button *dark = new Button("Dark");
            dark->onMouseClick.addEventListener([&](Widget *widget, MouseEvent event) {
                for (Window *window : Application::get()->m_windows) {
                    window->dc->default_style = Application::get()->dc->default_dark_style;
                    window->layout();
                    window->update();
                }
            });
        h_box->append(dark);
        Button *reset = new Button("Reset");
            reset->style.widget_background = Color("#ff5555");
            reset->onMouseClick.addEventListener([&](Widget *button, MouseEvent event) {
                for (Window *window : Application::get()->m_windows) {
                    window->dc->default_style = Application::get()->dc->default_light_style;
                    window->layout();
                    window->update();
                }
            });
        h_box->append(reset);
    append(h_box);

    int *size_options[12] = {
        &s.margin.top, &s.margin.bottom, &s.margin.left, &s.margin.right,
        &s.border.top, &s.border.bottom, &s.border.left, &s.border.right,
        &s.padding.top, &s.padding.bottom, &s.padding.left, &s.padding.right
    };
    std::string size_options_names[3] = {
        "Margin", "Border", "Padding"
    };
    std::string size_options_sides_names[4] = {
        "Top", "Bottom", "Left", "Right"
    };
    Box *size_box = new Box(Align::Horizontal);
        for (int i = 0; i < 3; i++) {
            GroupBox *gb = new GroupBox(Align::Vertical, size_options_names[i]);
                auto line = new LineEdit("", "All", 100);
                line->onTextChanged.addEventListener([=]() {
                    if (line->text().length()) {
                        try {
                            int value = std::stoi(line->text().c_str());
                            *size_options[i*4+0] = value;
                            *size_options[i*4+1] = value;
                            *size_options[i*4+2] = value;
                            *size_options[i*4+3] = value;
                            for (Window *window : Application::get()->m_windows) {
                                window->dc->default_style = Application::get()->dc->default_style;
                                window->layout();
                                window->update();
                            }
                        } catch (std::invalid_argument &e) {
                            warn("Parsing error!");
                        } catch (std::out_of_range &e) {
                            warn("Parsing error!");
                        }
                    } else {
                        *size_options[i*4+0] = 0;
                        *size_options[i*4+1] = 0;
                        *size_options[i*4+2] = 0;
                        *size_options[i*4+3] = 0;
                        for (Window *window : Application::get()->m_windows) {
                            window->dc->default_style = Application::get()->dc->default_style;
                            window->layout();
                            window->update();
                        }
                    }
                });
                gb->append(line);
                for (int j = 0; j < 4; j++) {
                    auto line = new LineEdit("", size_options_sides_names[j], 100);
                    line->onTextChanged.addEventListener([=]() {
                        if (line->text().length()) {
                            try {
                                *size_options[i*4+j] = std::stoi(line->text().c_str());
                                for (Window *window : Application::get()->m_windows) {
                                    window->dc->default_style = Application::get()->dc->default_style;
                                    window->layout();
                                    window->update();
                                }
                            } catch (std::invalid_argument &e) {
                                warn("Parsing error!");
                            } catch (std::out_of_range &e) {
                                warn("Parsing error!");
                            }
                        } else {
                            *size_options[i*4+j] = 0;
                            for (Window *window : Application::get()->m_windows) {
                                window->dc->default_style = Application::get()->dc->default_style;
                                window->layout();
                                window->update();
                            }
                        }
                    });
                    gb->append(line);
                }
            size_box->append(gb);
        }
        Color *border_color[4] = {
            &s.border.color_top, &s.border.color_bottom, &s.border.color_left, &s.border.color_right
        };
        GroupBox *gb = new GroupBox(Align::Vertical, "Border Color");
            LineEdit *line = new LineEdit("", "All", 100);
                line->onTextChanged.addEventListener([=]() {
                    Color value = Color(line->text().c_str());
                    *border_color[0] = value;
                    *border_color[1] = value;
                    *border_color[2] = value;
                    *border_color[3] = value;
                    for (Window *window : Application::get()->m_windows) {
                        window->dc->default_style = Application::get()->dc->default_style;
                    }
                });
        gb->append(line, Fill::Horizontal);
            for (int i = 0; i < 4; i++) {
                LineEdit *line = new LineEdit("", size_options_sides_names[i], 100);
                    line->onTextChanged.addEventListener([=]() {
                        *border_color[i] = Color(line->text().c_str());
                        for (Window *window : Application::get()->m_windows) {
                            window->dc->default_style = Application::get()->dc->default_style;
                        }
                    });
                gb->append(line, Fill::Horizontal);
            }
        size_box->append(gb);
    append(size_box, Fill::Horizontal);

    Color *color_options[13] = {
        &s.window_background, &s.widget_background, &s.accent_widget_background,
        &s.text_foreground, &s.text_background, &s.text_selected, &s.text_disabled,
        &s.hovered_background, &s.pressed_background, &s.accent_hovered_background,
        &s.accent_pressed_background, &s.icon_foreground, &s.border_background
    };
    std::string color_option_names[13] = {
        "window_background", "widget_background", "accent_widget_background",
        "text_foreground", "text_background", "text_selected", "text_disabled",
        "hovered_background", "pressed_background", "accent_hovered_background",
        "accent_pressed_background", "icon_foreground", "border_background"
    };
    for (int i = 0; i < 13; i++) {
        Label *l = new Label(color_option_names[i]);
        append(l);

        LineEdit *e = new LineEdit("", color_options[i]->toString(), 150);
            e->onTextChanged.addEventListener([=]() {
                *color_options[i] = Color(e->text().c_str());
                for (Window *window : Application::get()->m_windows) {
                    window->dc->default_style = Application::get()->dc->default_style;
                }
            });
        append(e, Fill::Horizontal);
    }
}

StyleEditor::~StyleEditor() {

}

Window* StyleEditor::asWindow(const char *title, Size size) {
    Window *window = new Window(title, size, Point());
        delete window->mainWidget();
        Widget *styling_widget = new StyleEditor();
        styling_widget->setFillPolicy(Fill::Both);
        window->setMainWidget(styling_widget);
    return window;
}
