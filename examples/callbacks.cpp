#include "../src/util.hpp"
#include "../src/application.hpp"
#include "../src/controls/button.hpp"

int main(int argc, char **argv) { 
    Application *app = Application::get();
        app->setTitle("Callbacks");
    app->onReady = [](Window *win) {
        println("READY");
    };
    bool quit = true;
    app->onQuit = [&](Window *win) -> bool {
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

        app->onResize = [&](Window *win) {
            button->setText("Window Size:" + std::to_string((int)win->size.w) + ", " + std::to_string((int)win->size.h));
        };

    #ifdef TEST
        #include "../tests/headless.hpp"
    #else
        app->run();
    #endif

    return 0; 
}