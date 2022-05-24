#include "../src/util.hpp"
#include "../src/application.hpp"
#include "../src/controls/button.hpp"
#include "../src/controls/tooltip.hpp"

int main(int argc, char **argv) {
    Application *app = Application::get();
        app->mainWindow()->onReady = [&](Window *window) {
            if (argc > 1) {
                if (String(argv[1]) == "quit") {
                    window->quit();
                }
            }
        };
        app->mainWindow()->setTitle("Tooltips");
        Button *top = new Button("Top");
            top->tooltip = new Tooltip("top");
        Button *bottom = new Button("Bottom");
            bottom->tooltip = new Tooltip("bottom");
        app->mainWindow()->append(top, Fill::Both);
        app->mainWindow()->append(bottom, Fill::Both);
    app->run();

    return 0;
}
