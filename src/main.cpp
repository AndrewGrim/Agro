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

int main() { 
    Application *app = new Application();
        // Box *sizer = new Box(Align::Vertical);
        for (int i = 0; i < 51; i++) {
            app->append(new Button(std::to_string(i) + " This is a really long string so that we can test the horizontal scrollbar!"), Fill::Both);
        }
        // app->append(sizer, Fill::Both);
    app->run();

    return 0; 
}
