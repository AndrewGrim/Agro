#include <iostream>
#include <vector>
#include <memory>
#include <ctime>

#include "util.hpp"
#include "app.hpp"
#include "option.hpp"
#include "controls/widget.hpp"
#include "controls/box.hpp"
#include "controls/button.hpp"
#include "controls/image.hpp"
#include "controls/label.hpp"
#include "controls/slider.hpp"
#include "controls/scrollbar.hpp"
#include "controls/scrolledbox.hpp"
#include "controls/lineedit.hpp"
#include "controls/notebook.hpp"

#include "custom_widget.hpp"

int main(int argc, char **argv) { 
    Application *app = new Application("Application", Size(500, 500));
    Font *font = new Font("fonts/DejaVu/DejaVuSansMono.ttf", 18, Font::Type::Mono);
        NoteBook *nb = new NoteBook();
        {
            Box *_b = new Box(Align::Vertical);
                // _b->setBackground(Color(0, 0, 0, 0));
                _b->setBackground(Color(1, 0, 1, 1));
                LineEdit *ln = new LineEdit("Fill::None");
                    // ln->setMinLength(80);
                    // ln->bind(SDLK_a, Mod::Ctrl, []{
                    //     println("Ctrl+A");
                    // });
                _b->append(ln, Fill::None);
                LineEdit *lb = new LineEdit("Fill::Both Fill::Both Fill::Both Fill::Both Fill::Both Fill::Both");
                    lb->setPlaceholderText("Nothing to see here!");
                    // lb->bind(SDLK_SPACE, Mod::None, [&]{
                    //     lb->clear();
                    // });
                _b->append(lb, Fill::Both);
            nb->appendTab(_b, "Tab 1");
        }
        {
            Box *_b = new Box(Align::Vertical);
                for (int i = 0; i < 5; i++) {
                    _b->append((new Button(std::to_string(i)))->setBackground(Color(0, 0, 1)), Fill::Both);
                }
            nb->appendTab(_b, "Tab 2", new Image("notes.png"));
        }
        {
            for (int t = 3; t < 15; t++) {
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
