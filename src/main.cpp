#include <iostream>
#include <vector>
#include <memory>
#include <ctime>

#include "util.hpp"
#include "app.hpp"
#include "controls/box.hpp"
#include "controls/button.hpp"
#include "controls/notebook.hpp"
#include "controls/treeview.hpp"

int main(int argc, char **argv) { 
    Application *app = new Application("Application", Size(500, 500));
        TestTree *tt = new TestTree();
        app->append(tt, Fill::Both);
    app->run();

    return 0;
}