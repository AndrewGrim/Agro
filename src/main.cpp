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
        app->append(new Button("Top"), Fill::Horizontal);
        Box *box = new Box(Align::Horizontal);
            box->append(new Button("Left"), Fill::Vertical);
            TestTree *tt = new TestTree();
            box->append(tt, Fill::Both);
        app->append(box, Fill::Both);
    app->run();

    return 0;
}