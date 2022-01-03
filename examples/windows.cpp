#include "../src/application.hpp"

int main(int argc, char **argv) {
    Application *app = Application::get();
        app->onReady = [&](Window *window) {
            if (argc > 1) {
                if (std::string(argv[1]) == std::string("quit")) {
                    window->quit();
                }
            }
        };
        app->setTitle("Window 1");
        app->append(new Button("Window 1"), Fill::Vertical);

        Window *window2 = new Window("Window 2", Size(200, 200));
            window2->append(new Button("Window 2"));
        window2->run();

        Window *window3 = new Window("Window 3", Size(100, 100));
            window3->append(new Button("Window 3"), Fill::Both);
        window3->run();
    app->run();

    return 0;
}
