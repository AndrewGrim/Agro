#include "../src/application.hpp"
#include "../src/controls/label.hpp"

int main(int argc, char **argv) { 
    Application *app = Application::get();
        app->setTitle("One Million Labels Test");
        for (int i = 0; i < 1000000; i++) {
            app->append(new Label(std::to_string(i)), Fill::Both);
        }

    #ifdef TEST
        #include "headless.hpp"
    #else
        app->run();
    #endif

    return 0; 
}