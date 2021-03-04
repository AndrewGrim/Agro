#include "../src/util.hpp"
#include "../src/application.hpp"
#include "../src/controls/button.hpp"
#include "../src/controls/notebook.hpp"

int main(int argc, char **argv) { 
    Application *app = Application::get();
        app->setTitle("Notebook Test");
        NoteBook *nb = new NoteBook();
        {
            for (int t = 1; t < 15; t++) {
                Box *_b = new Box(Align::Vertical);
                    for (int i = 0; i < 5; i++) {
                        _b->append((new Button(std::to_string(i)))->setBackground(Color(t / 100.0 * 5)), Fill::Both);
                    }
                nb->appendTab(_b, "Tab " + std::to_string(t), new Image("notes.png"), true);
            }
        }
        app->append(nb, Fill::Both);

    #ifdef TEST
        #include "headless.hpp"
    #else
        app->run();
    #endif

    return 0; 
}