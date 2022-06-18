#include "style_editor.hpp"
#include "text_edit.hpp"
#include "group_box.hpp"

StyleEditor::StyleEditor() : ScrolledBox(Align::Vertical) {
    Style &s = Application::get()->mainWindow()->dc->default_style;

     Box *h_box = new Box(Align::Horizontal);
        Button *light = new Button("Light");
            light->onMouseClick.addEventListener([&](Widget *widget, MouseEvent event) {
                for (Window *window : Application::get()->m_windows) {
                    window->dc->default_style = Application::get()->mainWindow()->dc->default_light_style;
                    window->layout(LAYOUT_STYLE);
                }
            });
        h_box->append(light);
        Button *dark = new Button("Dark");
            dark->onMouseClick.addEventListener([&](Widget *widget, MouseEvent event) {
                for (Window *window : Application::get()->m_windows) {
                    window->dc->default_style = Application::get()->mainWindow()->dc->default_dark_style;
                    window->layout(LAYOUT_STYLE);
                }
            });
        h_box->append(dark);
        Button *reset = new Button("Reset");
            reset->style().widget_background_color = Color("#ff5555");
            reset->onMouseClick.addEventListener([&](Widget *button, MouseEvent event) {
                for (Window *window : Application::get()->m_windows) {
                    window->dc->default_style = Application::get()->mainWindow()->dc->default_light_style;
                    window->layout(LAYOUT_STYLE);
                }
            });
        h_box->append(reset);
    append(h_box);

    i16 *size_options[12] = {
        &s.margin.top, &s.margin.bottom, &s.margin.left, &s.margin.right,
        &s.border.top, &s.border.bottom, &s.border.left, &s.border.right,
        &s.padding.top, &s.padding.bottom, &s.padding.left, &s.padding.right
    };
    String size_options_names[3] = {
        "Margin", "Border", "Padding"
    };
    String size_options_sides_names[4] = {
        "Top", "Bottom", "Left", "Right"
    };
    Box *size_box = new Box(Align::Horizontal);
        for (i32 i = 0; i < 3; i++) {
            GroupBox *gb = new GroupBox(Align::Vertical, size_options_names[i]);
                auto line = new TextEdit("", "All", TextEdit::Mode::SingleLine, Size(100, 100));
                line->onTextChanged.addEventListener([=]() {
                    if (line->text().size()) {
                        try {
                            i32 value = std::stoi(line->text().data());
                            *size_options[i*4+0] = value;
                            *size_options[i*4+1] = value;
                            *size_options[i*4+2] = value;
                            *size_options[i*4+3] = value;
                            for (Window *window : Application::get()->m_windows) {
                                window->dc->default_style = Application::get()->mainWindow()->dc->default_style;
                                window->layout(LAYOUT_STYLE);
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
                            window->dc->default_style = Application::get()->mainWindow()->dc->default_style;
                            window->layout(LAYOUT_STYLE);
                        }
                    }
                });
                gb->append(line);
                for (i32 j = 0; j < 4; j++) {
                    auto line = new TextEdit("", size_options_sides_names[j].data(), TextEdit::Mode::SingleLine, Size(100, 100));
                    line->onTextChanged.addEventListener([=]() {
                        if (line->text().size()) {
                            try {
                                *size_options[i*4+j] = std::stoi(line->text().data());
                                for (Window *window : Application::get()->m_windows) {
                                    window->dc->default_style = Application::get()->mainWindow()->dc->default_style;
                                    window->layout(LAYOUT_STYLE);
                                }
                            } catch (std::invalid_argument &e) {
                                warn("Parsing error!");
                            } catch (std::out_of_range &e) {
                                warn("Parsing error!");
                            }
                        } else {
                            *size_options[i*4+j] = 0;
                            for (Window *window : Application::get()->m_windows) {
                                window->dc->default_style = Application::get()->mainWindow()->dc->default_style;
                                window->layout(LAYOUT_STYLE);
                            }
                        }
                    });
                    gb->append(line);
                }
            size_box->append(gb);
        }
        Color *border_color_all = &s.border_background_color;
        Color *border_color[4] = {
            &s.border_color.top, &s.border_color.bottom, &s.border_color.left, &s.border_color.right
        };
        GroupBox *gb = new GroupBox(Align::Vertical, "Border Color");
            TextEdit *line = new TextEdit("", "All", TextEdit::Mode::SingleLine, Size(100, 100));
                line->onTextChanged.addEventListener([=]() {
                    Color value = Color(line->text().data());
                    *border_color_all = value;
                    for (Window *window : Application::get()->m_windows) {
                        window->dc->default_style = Application::get()->mainWindow()->dc->default_style;
                    }
                });
        gb->append(line, Fill::Horizontal);
            for (i32 i = 0; i < 4; i++) {
                TextEdit *line = new TextEdit("", size_options_sides_names[i].data(), TextEdit::Mode::SingleLine, Size(100, 100));
                    line->onTextChanged.addEventListener([=]() {
                        *border_color[i] = Color(line->text().data());
                        for (Window *window : Application::get()->m_windows) {
                            window->dc->default_style = Application::get()->mainWindow()->dc->default_style;
                        }
                    });
                gb->append(line, Fill::Horizontal);
            }
        size_box->append(gb);
    append(size_box, Fill::Horizontal);

    Color *color_options[13] = {
        &s.window_background_color, &s.widget_background_color, &s.accent_widget_background_color,
        &s.text_foreground_color, &s.text_background_color, &s.text_selected_color, &s.text_disabled_color,
        &s.hovered_background_color, &s.pressed_background_color, &s.accent_hovered_background_color,
        &s.accent_pressed_background_color, &s.icon_foreground_color, &s.border_background_color
    };
    String color_option_names[13] = {
        "window_background_color", "widget_background_color", "accent_widget_background_color",
        "text_foreground_color", "text_background_color", "text_selected_color", "text_disabled_color",
        "hovered_background_color", "pressed_background_color", "accent_hovered_background_color",
        "accent_pressed_background_color", "icon_foreground_color", "border_background_color"
    };
    for (i32 i = 0; i < 13; i++) {
        Label *l = new Label(color_option_names[i]);
        append(l);

        TextEdit *e = new TextEdit("", color_options[i]->toString().data(), TextEdit::Mode::SingleLine, Size(150, 100));
            e->onTextChanged.addEventListener([=]() {
                *color_options[i] = Color(e->text().data());
                for (Window *window : Application::get()->m_windows) {
                    window->dc->default_style = Application::get()->mainWindow()->dc->default_style;
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
