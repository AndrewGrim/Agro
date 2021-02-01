#include <iostream>
#include <vector>
#include <memory>
#include <ctime>

#include "util.hpp"
#include "app.hpp"
#include "controls/box.hpp"
#include "controls/button.hpp"
#include "controls/notebook.hpp"

int main(int argc, char **argv) { 
    Application *app = new Application("Application", Size(500, 500));
        NoteBook *nb = new NoteBook();
        {
            for (int t = 1; t < 15; t++) {
                Box *_b = new Box(Align::Vertical);
                    for (int i = 0; i < 5; i++) {
                        _b->append((new Button(std::to_string(i)))->setBackground(Color(t / 100.0 * 5)), Fill::Both);
                    }
                nb->appendTab(_b, "Tab " + std::to_string(t));
            }
        }
        app->append(nb, Fill::Both);
    app->run();

    return 0; 
}
