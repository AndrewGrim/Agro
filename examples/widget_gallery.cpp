#include "../src/util.hpp"
#include "../src/application.hpp"
#include "../src/controls/box.hpp"
#include "../src/controls/group_box.hpp"
#include "../src/controls/note_book.hpp"
#include "../src/controls/label.hpp"
#include "../src/controls/button.hpp"
#include "../src/controls/line_edit.hpp"

Widget* simple(Application &app) {
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
            notebook->appendTab(simple(*app), "Simple", nullptr, false);
        app->append(notebook, Fill::Both);
    app->run();

    return 0;
}
