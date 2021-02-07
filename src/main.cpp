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

// Example `hidden` struct within a TreeNode
// this can be used for storing data that is not
// meant to be show but is still useful to keep
// around. Furthermore, it can be used for sorting
// columns that are not sortable based on what they
// draw.
struct Hidden {
    size_t id;

    Hidden(size_t id) {
        this->id = id;
    }
};

int main(int argc, char **argv) { 
    Application *app = new Application("Application", Size(500, 500));
        app->append(new Button("Top"), Fill::Horizontal);
        app->bind(SDLK_q, Mod::Ctrl, [&]{
            app->quit();
        });
        Box *box = new Box(Align::Horizontal);
            box->append(new Button("Left"), Fill::Vertical);
            TreeView<Hidden> *tv = new TreeView<Hidden>();
            {
                for (int i = 0; i < 7; i++) {
                    tv->append(new Button("Column: " + std::to_string(i)));
                }
                Tree<Hidden> *model = new Tree<Hidden>();
                {
                    TreeNode<Hidden> *root = nullptr;
                    for (int i = 0; i < 100; i++) {
                        std::vector<CellRenderer*> columns;
                            columns.push_back(new TextCellRenderer("Root: " + std::to_string(i)));
                            columns.push_back(new TextCellRenderer("Column 2"));
                            columns.push_back(new TextCellRenderer("Column 3"));
                            columns.push_back(new TextCellRenderer("Column 4"));
                        TreeNode<Hidden> *node = new TreeNode<Hidden>(columns, new Hidden(i));                    
                        root = model->append(nullptr, node);
                        for (int i = 0; i < 5; i++) {
                            std::vector<CellRenderer*> columns;
                                columns.push_back(new TextCellRenderer("Second Gen: " + std::to_string(i)));
                                columns.push_back(new TextCellRenderer("2"));
                                columns.push_back(new TextCellRenderer("3"));
                                columns.push_back(new TextCellRenderer("4"));
                            TreeNode<Hidden> *node = new TreeNode<Hidden>(columns, new Hidden(i));       
                            model->append(root, node);
                        }
                    }
                }
                tv->setModel(model);
            }
            box->append(tv, Fill::Both);
        app->append(box, Fill::Both);
    app->run();

    return 0;
}