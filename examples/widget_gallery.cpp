#include "../src/util.hpp"
#include "../src/application.hpp"
#include "../src/controls/box.hpp"
#include "../src/controls/note_book.hpp"
#include "../src/controls/label.hpp"
#include "../src/controls/button.hpp"
#include "../src/controls/line_edit.hpp"

Widget* simple(Application &app) {
    Box *box = new Box(Align::Vertical);
        Box *labels_and_buttons = new Box(Align::Horizontal);
            Box *labels = new Box(Align::Vertical);
                labels->append(new Label("Label that occupies one line."));
                labels->append(new Label("Label that\nspans\nmultiple lines."));
            labels_and_buttons->append(labels, Fill::Both);

            Box *buttons = new Box(Align::Vertical);
                buttons->append(new Button("Button"));
                buttons->append(new Button((new Image(app.icons["close_thin"]))->setForeground(COLOR_BLACK)));
                Button *image_and_text = new Button(new Image(app.icons["close_thin"]));
                    image_and_text->m_image->setForeground(COLOR_BLACK);
                    image_and_text->setText("Button");
                buttons->append(image_and_text);
            labels_and_buttons->append(buttons, Fill::Both);
        box->append(labels_and_buttons, Fill::Both);

        Box *line_edits = new Box(Align::Vertical);
            line_edits->append(new LineEdit("", "LineEdit placeholder text"), Fill::Horizontal);
            line_edits->append(new LineEdit("LineEdit default text", ""), Fill::Horizontal);
            line_edits->append(new LineEdit("", ""), Fill::Horizontal);
        box->append(line_edits, Fill::Both);

    return box;
}

int main(int argc, char **argv) {
    Application *app = Application::get();
        delete app->mainWidget();
        app->setMainWidget(new Box(Align::Vertical));
        app->onReady = [&](Window *window) {
            if (argc > 1) {
                if (std::string(argv[1]) == std::string("quit")) {
                    window->quit();
                }
            }
        };
        app->setTitle("Widget Gallery");
         LineEdit *change_background = new LineEdit("", "Enter color ex: #ff000055");
            change_background->onTextChanged = [&]() {
                app->dc->default_style.widget_background = Color(change_background->text().c_str());
                app->update();
            };
        app->append(change_background, Fill::Horizontal);
        NoteBook *notebook = new NoteBook();
            notebook->appendTab(simple(*app), "Simple", nullptr, false);
        app->append(notebook, Fill::Both);
    app->run();

    return 0;
}
