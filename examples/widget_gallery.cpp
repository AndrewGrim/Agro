#include "../src/util.hpp"
#include "../src/application.hpp"
#include "../src/controls/box.hpp"
#include "../src/controls/group_box.hpp"
#include "../src/controls/note_book.hpp"
#include "../src/controls/label.hpp"
#include "../src/controls/button.hpp"
#include "../src/controls/line_edit.hpp"
#include "../src/controls/splitter.hpp"

Widget* basic(Application &app) {
    Box *box = new Box(Align::Vertical);
        Box *labels_and_buttons = new Box(Align::Horizontal);
            GroupBox *labels = new GroupBox(Align::Vertical, "Labels");
                Label *lr = new Label("This text occupies one line and is right aligned.");
                    lr->setHorizontalAlignment(HorizontalAlignment::Right);
                Label *lc = new Label("This text\nspans\nmultiple lines,\nand is center aligned.");
                    lc->setHorizontalAlignment(HorizontalAlignment::Center);
                labels->append(new Label("This text occupies one line and is left aligned."), Fill::Both);
                labels->append(lr, Fill::Both);
                labels->append(lc, Fill::Both);

            labels_and_buttons->append(labels, Fill::Both);

            GroupBox *buttons = new GroupBox(Align::Vertical, "Buttons");
                buttons->append(new Button("Button"));
                buttons->append(new Button((new Image(app.icons["close_thin"]))->setForeground(COLOR_BLACK)));
                Button *image_and_text = new Button(new Image(app.icons["close_thin"]));
                    image_and_text->m_image->setForeground(COLOR_BLACK);
                    image_and_text->setText("Button");
                buttons->append(image_and_text, Fill::Both);
            labels_and_buttons->append(buttons, Fill::Both);
        box->append(labels_and_buttons, Fill::Both);

        GroupBox *line_edits = new GroupBox(Align::Vertical, "LineEdits");
            line_edits->append(new LineEdit("", "Placeholder text", 200));
            line_edits->append(new LineEdit("Default text", "", 300));
            line_edits->append(new LineEdit("", ""), Fill::Horizontal);
        box->append(line_edits, Fill::Horizontal);

        GroupBox *images = new GroupBox(Align::Horizontal, "Images");
            auto lena = std::make_shared<Texture>("lena.png");
            images->append((new Image(lena))->setMinSize(Size(24, 24))->setForeground(Color("#ff5555")));
            images->append((new Image(lena))->setMinSize(Size(56, 56))->setForeground(Color("#55ff55")));
            images->append((new Image(lena))->setMinSize(Size(72, 72))->setForeground(Color("#5555ff")));
            images->append((new Image(lena))->setMinSize(Size(24, 24))->setExpand(true), Fill::Both);
            images->append((new Image(lena))->setMinSize(Size(24, 24))->setExpand(true)->setMaintainAspectRatio(false), Fill::Both);
        box->append(images, Fill::Both);

    return box;
}

Widget* slidersAndScrollbars(Application &app) {
    Box *box = new Box(Align::Vertical);
        GroupBox *sliders = new GroupBox(Align::Horizontal, "Sliders");
            Slider *sh = new Slider(Align::Horizontal, 0.5);
                sh->m_slider_button_size = 50;
            sliders->append(sh, Fill::Horizontal);
            sliders->append(new Slider(Align::Vertical, 0.5), Fill::Vertical);
        box->append(sliders, Fill::Both);

        GroupBox *scroll_bars = new GroupBox(Align::Horizontal, "ScrollBars");
            ScrollBar *sbh = new ScrollBar(Align::Horizontal);
                sbh->m_slider->m_slider_button_size = 50;
            scroll_bars->append(sbh, Fill::Horizontal);
            scroll_bars->append(new ScrollBar(Align::Vertical), Fill::Vertical);
        box->append(scroll_bars, Fill::Both);

    return box;
}

Widget* splitter(Application &app) {
    Box * box = new Box(Align::Vertical);
        Splitter *h = new Splitter(Align::Horizontal);
            h->left(new Button("Left"));
            h->right(new Button("Right"));
        box->append(h, Fill::Both);

        Splitter *v = new Splitter(Align::Vertical);
            v->top(new Button("Top"));
            v->bottom(new Button("Bottom"));
        box->append(v, Fill::Both);

    return box;
}

int main(int argc, char **argv) {
    Application *app = Application::get();
        Color default_widget_background = app->dc->default_style.widget_background;
        delete app->mainWidget();
        app->setMainWidget(new Box(Align::Vertical));
        app->onReady = [&](Window *window) {
            if (argc > 1) {
                if (std::string(argv[1]) == std::string("quit")) {
                    window->quit();
                }
            }
        };
        app->resize(800, 600);
        app->center();
        app->setTitle("Widget Gallery");
        LineEdit *change_background = new LineEdit("", "Enter color ex: #ff000055");
            change_background->onTextChanged = [&]() {
                if (change_background->text().length()) {
                    app->dc->default_style.widget_background = Color(change_background->text().c_str());
                } else {
                    app->dc->default_style.widget_background = Color(default_widget_background);
                }
                app->update();
            };
        app->append(change_background, Fill::Horizontal);
        NoteBook *notebook = new NoteBook();
            notebook->appendTab(basic(*app), "Basic", nullptr, false);
            notebook->appendTab(slidersAndScrollbars(*app), "Sliders and ScrollBars", nullptr, false);
            notebook->appendTab(splitter(*app), "Splitter", nullptr, false);
        app->append(notebook, Fill::Both);
    app->run();

    return 0;
}
