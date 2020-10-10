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
        for (int i = 0; i < 1001; i++) {
            app->append(new Button(std::to_string(i)), Fill::Both);
        }
    app->run();

    return 0; 
}
