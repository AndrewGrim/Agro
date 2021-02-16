#include "../src/util.hpp"
#include "../src/app.hpp"
#include "../src/controls/button.hpp"

int main(int argc, char **argv) { 
    Application *app = new Application("Keybindings Test", Size(400, 400));
    app->onReady = [](Application *app) {
        println("READY");
    };
    bool quit = true;
    app->onQuit = [&](Application *app) -> bool {
        println("QUIT");
        return quit;
        // return false to not quit or true to quit
    };
        Button *button = new Button("Click Me!");
            button->onMouseDown.addEventListener([&](Widget *widget, MouseEvent event) {
                button->setText("Mouse Down!");
            });
            button->onMouseUp.addEventListener([&](Widget *widget, MouseEvent event) {
                button->setText("Mouse Up!");
            });
            // onMouseClick would happend immediately after onMouseUp.
            // button->onMouseClick.addEventListener([&](Widget *widget, MouseEvent event) {
            //     button->setText("Mouse Click!");
            // });
            button->onMouseEntered.addEventListener([&](Widget *widget, MouseEvent event) {
                button->setText("Mouse Entered!");
            });
            button->onMouseLeft.addEventListener([&](Widget *widget, MouseEvent event) {
                button->setText("Mouse Left!");
            });
        app->append(button, Fill::Both);
        Button *disable_enable_quit = new Button("Disable Quit");
            disable_enable_quit->onMouseClick.addEventListener([&](Widget *widget, MouseEvent event) {
                if (quit) {
                    disable_enable_quit->setText("Enable Quit");
                    quit = false;
                } else {
                    disable_enable_quit->setText("Disable Quit");
                    quit = true;
                }
            });
        app->append(disable_enable_quit, Fill::Both);

    #ifdef TEST
        #include "headless.hpp"
    #else
        app->run();
    #endif

    return 0; 
}