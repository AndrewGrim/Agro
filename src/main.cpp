#include "util.hpp"
#include "application.hpp"

int main(int argc, char **argv) { 
    Application &app = Application::get();
        // app.setTitle("New Title");
        app.resize(500, 500);
        app.append(new Button(new Image("lena.png")), Fill::Both);
    app.run();

    return 0;
}