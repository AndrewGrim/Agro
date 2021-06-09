#include "../src/util.hpp"
#include "../src/application.hpp"
#include "../src/controls/button.hpp"
#include "../src/controls/note_book.hpp"

int main(int argc, char **argv) {
    Application *app = Application::get();
        app->onReady = [&](Window *window) {
            if (argc > 1) {
                if (std::string(argv[1]) == std::string("quit")) {
                    window->quit();
                }
            }
        };
        app->setTitle("Notebook Test");
        NoteBook *nb = new NoteBook();
        {
            for (int t = 1; t < 10; t++) {
                Box *_b = new Box(Align::Vertical);
                    for (int i = 0; i < 10; i++) {
                        _b->append(new Button(std::to_string(i)), Fill::Both);
                    }
                if (t % 3 == 0) {
                    nb->appendTab(_b, "Tab " + std::to_string(t), new Image("notes.png"), true);
                } else if (t % 2 == 0) {
                    nb->appendTab(_b, "Tab " + std::to_string(t), new Image("notes.png"), false);
                } else {
                    nb->appendTab(_b, "Tab " + std::to_string(t), nullptr, false);
                }
            }
        }
        app->append(nb, Fill::Both);
    app->run();

    return 0;
}
