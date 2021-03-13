#include "../src/util.hpp"
#include "../src/application.hpp"
#include "../src/controls/button.hpp"

int main(int argc, char **argv) { 
    Application *app = Application::get();
        app->onReady = [&](Window *window) {
            if (argc > 1) {
                if (std::string(argv[1]) == std::string("quit")) {
                    window->quit();
                }
            }
        };
        app->setTitle("Tooltips");
        Button *top = new Button("Top");
            top->tooltip = "top";
        Button *bottom = new Button("Bottom");
            bottom->tooltip = "bottom";
        app->append(top, Fill::Both);
        app->append(bottom, Fill::Both);
    app->run();

    return 0; 
}