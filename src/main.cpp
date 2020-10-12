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
        ScrolledBox *top = new ScrolledBox(Align::Vertical, Size<float>(200, 200));
            for (char i = 'a'; i <= 'z'; i++) {
                top->append((new Button(std::string(1, i)))->set_background(Color(0.6, 0.0, 0.6)), Fill::Both);
            }
        app->append(top, Fill::Both);
        ScrolledBox *bottom = new ScrolledBox(Align::Vertical, Size<float>(200, 200));
            for (int i = 1; i < 51; i++) {
                bottom->append((new Button(std::to_string(i)))->set_background(Color(0.0, 0.6, 0.6)), Fill::Both);
            }
        app->append(bottom, Fill::Both);
    app->run();

    return 0; 
}
