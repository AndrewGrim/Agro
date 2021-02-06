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
        app->bind(SDLK_q, Mod::Ctrl, [&]{
            app->quit();
        });
        Box *box = new Box(Align::Horizontal);
            box->append(new Button("Left"), Fill::Vertical);
            TestTree *tt = new TestTree();
            {
                for (int i = 0; i < 5; i++) {
                    tt->append(new Button("Header: Col. " + std::to_string(i)));
                }
            }
            box->append(tt, Fill::Both);
        app->append(box, Fill::Both);

        Tree<void*> model;
        {
            std::vector<CellRenderer> columns;
                columns.push_back(CellRenderer("Root Node!"));
            TreeNode<void*> node = TreeNode<void*>(columns, nullptr);
            model.append(nullptr, node);
        }
        println(model.roots.size());
        println(model.roots[0].columns[0].text);

    app->run();

    return 0;
}