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
        app->setTitle("Keybindings");
        int map_key = app->bind(
            SDLK_q,
            Mod::None, 
            [&]{
                println("Q");
            }
        );
        app->bind(
            SDLK_q,
            Mod::Ctrl,
            [&]{
                println("CTRL+Q");
            }
        );
        app->bind(
            SDLK_q,
            Mod::Ctrl|Mod::Shift|Mod::Alt,
            [&]{
                println("CTRL+Shift+Alt+Q");
            }
        );
        app->bind(
            SDLK_q,
            Mod::Ctrl|Mod::Shift,
            [&]{
                println("CTRL+Shift+Q");
            }
        );
        Button *unbind_button = new Button("Unbind `Q`");
            unbind_button->onMouseClick.addEventListener([=](Widget *widget, MouseEvent event) {
                app->unbind(map_key);
            });
        app->append(unbind_button, Fill::Both);
    app->run();

    return 0; 
}