#include "../src/util.hpp"
#include "../src/application.hpp"
#include "../src/controls/button.hpp"

int main(int argc, char **argv) { 
    Application *app = Application::get();
        app->setTitle("Tooltips");
        Button *top = new Button("Top");
            top->tooltip = "top";
        Button *bottom = new Button("Bottom");
            bottom->tooltip = "bottom";
        app->append(top, Fill::Both);
        app->append(bottom, Fill::Both);

    #ifdef TEST
        #include "../tests/headless.hpp"
    #else
        app->run();
    #endif

    return 0; 
}