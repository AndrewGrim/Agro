#include "util.hpp"
#include "application.hpp"

int main(int argc, char **argv) { 
    Application *app = Application::get();
    app->run();

    return 0;
}