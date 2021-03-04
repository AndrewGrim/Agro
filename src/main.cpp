#include "util.hpp"
#include "application.hpp"

int main(int argc, char **argv) { 
    Application *app = new Application("Application", Size(400, 400));
        app->append(new Button(new Image("lena.png")), Fill::Both);
    app->run();

    return 0;
}