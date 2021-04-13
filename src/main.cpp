#include "util.hpp"
#include "application.hpp"
#include "controls/treeview.hpp"
#include "controls/splitter.hpp"
#include "controls/lineedit.hpp"

int main(int argc, char **argv) { 
    Application *app = Application::get();
    app->run();

    return 0;
}