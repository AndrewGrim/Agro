#include "../src/application.hpp"
#include "../src/controls/label.hpp"

int main(int argc, char **argv) { 
    Application *app = Application::get();
        app->onReady = [&](Window *window) {
            if (argc > 1) {
                if (std::string(argv[1]) == std::string("quit")) {
                    window->quit();
                }
            }
        };
        app->setTitle("One Million Labels Test");
        for (int i = 0; i < 1000000; i++) {
            app->append(new Label(std::to_string(i)), Fill::Both);
        }
    app->run();

    return 0; 
}