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
                Color(1, 0.1, 0.1), Color(1, 0.6), Color(1, 1),
                Color(0.1, 0.1, 1), Color(0, 0.5, 1), Color(0, 1, 1),
            };
            for (int i = 0; i < 6; i++) {
                Button *b = new Button("Button");
                    b->setBackground(colors[i]);
                    // TODO should they even be expanding??? the rect should be 500 / 28? 17ish buttons on screen at once
                    // TODO maybe rename to onClick instead??
                    b->onMouseClick = [=](MouseEvent event) {
                        auto t = std::time(nullptr);
                        b->setText(std::asctime(std::localtime(&t)));
                    };
                    if (i == 2) {
                        b->hide();
                    }
                app->append(b, Fill::Both);
            }
        // app->append(new LineEdit("text 1"), Fill::Horizontal);
        // app->append(new LineEdit("text 2"), Fill::Horizontal);
    app->run();

    return 0; 
}
