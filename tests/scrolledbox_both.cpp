#include <iostream>
#include <vector>
#include <memory>

#include "../src/util.hpp"
#include "../src/application.hpp"
#include "../src/controls/box.hpp"
#include "../src/controls/button.hpp"
#include "../src/controls/scrolledbox.hpp"

int main(int argc, char **argv) { 
    Application *app = Application::get();
        app->setTitle("ScrolledBox Both Test");
        app->resize(500, 500);
        ScrolledBox *top = new ScrolledBox(Align::Vertical, Size(800, 400));
            for (char i = 'a'; i <= 'z'; i++) {
                top->append((new Button(std::string(100, i)))->setBackground(Color(0.6, 0.0, 0.6)), Fill::Both);
            }
        app->append(top, Fill::Both);
        ScrolledBox *bottom = new ScrolledBox(Align::Vertical, Size(800, 400));
            for (char i = 'A'; i < 'Z'; i++) {
                bottom->append((new Button(std::string(100, i)))->setBackground(Color(0.0, 0.6, 0.6)), Fill::Both);
            }
        app->append(bottom, Fill::Both);

        

    #ifdef TEST
        #include "headless.hpp"
    #else
        app->run();
    #endif

    return 0; 
}
