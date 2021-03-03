#include "util.hpp"
#include "application.hpp"

int main(int argc, char **argv) { 
    Application *app = new Application("Application", Size(600, 400));
    app->run();

    return 0;
}