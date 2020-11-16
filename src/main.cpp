#include <iostream>
#include <vector>
#include <memory>

#include "util.hpp"
#include "app.hpp"
#include "controls/box.hpp"
#include "controls/button.hpp"
#include "controls/slider.hpp"
#include "controls/scrollbar.hpp"
#include "controls/scrolledbox.hpp"
#include "controls/lineedit.hpp"

int main() { 
    Application *app = new Application("Application", Size<int>(500, 500));
        app->append(new LineEdit("this is a widget that has editable text but is always one line"), Fill::Horizontal);
    app->run();

    return 0; 
}
