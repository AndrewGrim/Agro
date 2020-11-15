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
        ScrolledBox *top = new ScrolledBox(Align::Vertical, Size<float>(200, 200));
            top->append(new LineEdit("this is a widget that has editable text but is always one line"), Fill::Horizontal);
        app->append(top, Fill::Both);
    app->run();

    return 0; 
}
