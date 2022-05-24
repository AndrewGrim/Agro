#include "../src/util.hpp"
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
        app->mainWindow()->setTitle("Keybindings");
        i32 map_key = app->mainWindow()->bind(
            SDLK_q,
            Mod::None,
            [&]{
                println("Q");
            }
        );
        app->mainWindow()->bind(
            SDLK_q,
            Mod::Ctrl,
            [&]{
                println("CTRL+Q");
            }
        );
        app->mainWindow()->bind(
            SDLK_q,
            Mod::Ctrl|Mod::Shift|Mod::Alt,
            [&]{
                println("CTRL+Shift+Alt+Q");
            }
        );
        app->mainWindow()->bind(
            SDLK_q,
            Mod::Ctrl|Mod::Shift,
            [&]{
                println("CTRL+Shift+Q");
            }
        );
        Button *unbind_button = new Button("Unbind `Q`");
            unbind_button->onMouseClick.addEventListener([=](Widget *widget, MouseEvent event) {
                app->mainWindow()->unbind(map_key);
            });
        app->mainWindow()->append(unbind_button, Fill::Both);
    app->run();

    return 0;
}
