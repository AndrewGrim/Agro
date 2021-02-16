#include "../src/util.hpp"
#include "../src/app.hpp"
#include "../src/controls/button.hpp"

int main(int argc, char **argv) { 
    Application *app = new Application("Keybindings Test", Size(400, 400));
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

    #ifdef TEST
        #include "headless.hpp"
    #else
        app->run();
    #endif

    return 0; 
}