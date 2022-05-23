#include "../src/util.hpp"
#include "../src/application.hpp"
#include "../src/controls/button.hpp"
#include "../src/controls/note_book.hpp"
#include "resources.hpp"

int main(int argc, char **argv) {
    Application *app = Application::get();
        app->mainWindow()->onReady = [&](Window *window) {
            if (argc > 1) {
                if (std::string(argv[1]) == std::string("quit")) {
                    window->quit();
                }
            }
        };
        app->mainWindow()->setTitle("Notebook Test");
        NoteBook *nb = new NoteBook();
        {
            for (i32 t = 1; t < 10; t++) {
                Box *_b = new Box(Align::Vertical);
                    for (i32 i = 0; i < 10; i++) {
                        _b->append(new Button(std::to_string(i).data()), Fill::Both);
                    }
                if (t % 3 == 0) {
                    nb->appendTab(_b, String() + "Tab " + std::to_string(t).data(), new Image(notes_png, notes_png_length), true);
                } else if (t % 2 == 0) {
                    nb->appendTab(_b, String() + "Tab " + std::to_string(t).data(), new Image(notes_png, notes_png_length), false);
                } else {
                    nb->appendTab(_b, String() + "Tab " + std::to_string(t).data(), nullptr, false);
                }
            }
        }
        app->mainWindow()->append(nb, Fill::Both);
    app->run();

    return 0;
}
