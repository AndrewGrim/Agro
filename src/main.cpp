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
            Button *b = new Button("Button");
                // TODO maybe rename to onClick instead??
                b->onMouseClick = [&](MouseEvent event) {
                    auto t = std::time(nullptr);
                    b->setText(std::asctime(std::localtime(&t)));
                };
            app->append(b);
        // app->append(new LineEdit("text 1"), Fill::Horizontal);
        // app->append(new LineEdit("text 2"), Fill::Horizontal);
    app->run();

    return 0; 
}
