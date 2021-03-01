#include "util.hpp"
#include "custom_style.hpp"
#include "application.hpp"
#include "controls/box.hpp"
#include "controls/button.hpp"

int main(int argc, char **argv) { 
    Application *app = new Application("Application", Size(400, 400));
        app->mainWidget()->setBackground(COLOR_WHITE);
        app->append(new CustomStyle(), Fill::Both);
    app->run();

    return 0;
}