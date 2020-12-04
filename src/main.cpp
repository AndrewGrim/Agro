#include <iostream>
#include <vector>
#include <memory>
#include <ctime>

#include "util.hpp"
#include "app.hpp"
#include "controls/widget.hpp"
#include "controls/box.hpp"
#include "controls/button.hpp"
// #include "controls/slider.hpp"
// #include "controls/scrollbar.hpp"
// #include "controls/scrolledbox.hpp"
// #include "controls/lineedit.hpp"

int main() { 
    Application *app = new Application("Application", Size(500, 500));
    app->mainWidget()->setBackground(Color(0.5, 0.2, 0.4, 0.8));
            Color colors[] = {
                Color(1, 0.19, 0.19), Color(1, 0.6), Color(1, 1),
                Color(0.19, 0.19, 1), Color(0, 0.5, 1), Color(0, 1, 1),
            };
            Button *buttons[] = {
                (new Button("Button"))->setBackground(colors[0]),
                (new Button("Button"))->setBackground(colors[1]),
                (new Button("Button"))->setBackground(colors[2]),
                (new Button("Button"))->setBackground(colors[3]),
                (new Button("Button"))->setBackground(colors[4]),
                (new Button("Button"))->setBackground(colors[5]),
            };
            buttons[0]->setProportion(10);
            buttons[3]->setProportion(3);
            buttons[5]->setProportion(5);
            for (int i = 0; i < 6; i++) {
                auto b = buttons[i];
                // TODO maybe rename to onClick instead??
                b->onMouseClick = [=](MouseEvent event) {
                    auto t = std::time(nullptr);
                    b->setText(std::asctime(std::localtime(&t)));
                    if (buttons[1]->isVisible()) {
                        buttons[1]->hide();
                    } else {
                        buttons[1]->show();
                    }
                };
                if (i == 1) {
                    b->hide();
                }
                app->append(b, Fill::Both);
            }
        // app->append(new LineEdit("text 1"), Fill::Horizontal);
        // app->append(new LineEdit("text 2"), Fill::Horizontal);
    app->run();

    return 0; 
}
