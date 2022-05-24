#include "../src/application.hpp"

int main(int argc, char **argv) {
    Application *app = Application::get();
        app->mainWindow()->onReady = [&](Window *window) {
            if (argc > 1) {
                if (String(argv[1]) == "quit") {
                    window->quit();
                }
            }
        };
        app->mainWindow()->setTitle("Window 1");
        app->mainWindow()->append(new Button("Window 1"), Fill::Vertical);

        Window *window2 = new Window("Window 2", Size(200, 200), Point());
            window2->append(new Button("Window 2"));
        window2->run();

        Window *window3 = new Window("Window 3", Size(100, 100), Point());
            window3->append(new Button("Window 3"), Fill::Both);
        window3->run();
    app->run();

    return 0;
}
