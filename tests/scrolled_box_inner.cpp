#include <iostream>
#include <vector>
#include <memory>

#include "../src/util.hpp"
#include "../src/application.hpp"
#include "../src/controls/box.hpp"
#include "../src/controls/button.hpp"
#include "../src/controls/scrolled_box.hpp"

int main(int argc, char **argv) {
    Application *app = Application::get();
        app->onReady = [&](Window *window) {
            if (argc > 1) {
                if (std::string(argv[1]) == std::string("quit")) {
                    window->quit();
                }
            }
        };
        app->setTitle("ScrolledBox Inner Test");
        app->resize(500, 500);
        ScrolledBox *top = new ScrolledBox(Align::Vertical, Size(200, 200));
            for (char i = 'a'; i <= 'z'; i++) {
                top->append(new Button(std::string(100, i)), Fill::Both);
            }
        app->append(top, Fill::Both);
        ScrolledBox *bottom = new ScrolledBox(Align::Vertical, Size(200, 200));
            for (char i = 'A'; i < 'Z'; i++) {
                bottom->append(new Button(std::string(100, i)), Fill::Both);
            }
        app->append(bottom, Fill::Both);
    app->run();

    return 0;
}
