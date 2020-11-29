#include <iostream>
#include <vector>
#include <memory>

#include "util.hpp"
#include "app.hpp"
#include "controls/widget.hpp"
#include "controls/box.hpp"
// #include "controls/button.hpp"
// #include "controls/slider.hpp"
// #include "controls/scrollbar.hpp"
// #include "controls/scrolledbox.hpp"
// #include "controls/lineedit.hpp"

int main() { 
    Application *app = new Application("Application", Size(500, 500));
        app->mainWidget()->setBackground(Color(0.5, 0.2, 0.4, 0.8));
        // app->append(new LineEdit("text 1"), Fill::Horizontal);
        // app->append(new LineEdit("text 2"), Fill::Horizontal);
    app->run();

    return 0; 
}
