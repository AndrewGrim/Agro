#include "../src/application.hpp"
#include "../src/controls/button.hpp"

int main(int argc, char **argv) {
    Application *app = Application::get();
        app->mainWindow()->onReady = [&](Window *window) {
            if (argc > 1) {
                if (String(argv[1]) == "quit") {
                    window->quit();
                }
            }
        };
        app->mainWindow()->setTitle("One Million Buttons Test");
        for (i32 i = 0; i < 1000000; i++) {
            app->mainWindow()->append(new Button(toString(i)), Fill::Both);
        }
    app->run();

    return 0;
}
