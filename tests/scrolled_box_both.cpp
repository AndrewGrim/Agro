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
        app->mainWindow()->onReady = [&](Window *window) {
            if (argc > 1) {
                if (String(argv[1]) == "quit") {
                    window->quit();
                }
            }
        };
        app->mainWindow()->setTitle("ScrolledBox Both Test");
        app->mainWindow()->resize(500, 500);
        ScrolledBox *top = new ScrolledBox(Align::Vertical, Size(800, 400));
            for (u8 i = 'a'; i <= 'z'; i++) {
                top->append(new Button(String::repeat(i, 100)), Fill::Both);
            }
        app->mainWindow()->append(top, Fill::Both);
        ScrolledBox *bottom = new ScrolledBox(Align::Vertical, Size(800, 400));
            for (u8 i = 'A'; i < 'Z'; i++) {
                bottom->append(new Button(String::repeat(i, 100)), Fill::Both);
            }
        app->mainWindow()->append(bottom, Fill::Both);
    app->run();

    return 0;
}
