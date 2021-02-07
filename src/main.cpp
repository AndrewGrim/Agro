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
                for (int i = 0; i < 5; i++) {
                    tv->append(new Button("Header: Col. " + std::to_string(i)));
                }
            }
            box->append(tv, Fill::Both);
        app->append(box, Fill::Both);

        Tree<Hidden> *model = new Tree<Hidden>();
        {
            std::vector<CellRenderer*> columns;
                columns.push_back(new TextCellRenderer("Root"));
            TreeNode<Hidden> *node = new TreeNode<Hidden>(columns, new Hidden(1));
            TreeNode<Hidden> *child = model->append(nullptr, node);
            std::vector<CellRenderer*> second_columns;
                second_columns.push_back(new TextCellRenderer("1st generation"));
            TreeNode<Hidden> *second_child = model->append(child, new TreeNode<Hidden>(second_columns, new Hidden(2)));
            std::vector<CellRenderer*> third_columns;
                third_columns.push_back(new TextCellRenderer("2nd generation"));
            TreeNode<Hidden> *third_child = new TreeNode<Hidden>(third_columns, new Hidden(3));
                third_child->is_collapsed = true;
            model->append(second_child, third_child);
            std::vector<CellRenderer*> fourth_columns;
                fourth_columns.push_back(new TextCellRenderer("2nd generation"));
            TreeNode<Hidden> *fourth_child = new TreeNode<Hidden>(fourth_columns, new Hidden(4));
            model->append(second_child, fourth_child);
            for (TreeNode<Hidden> *root : model->roots) {
                pprint(0, ((TextCellRenderer*)root->columns[0])->text);
                pprint(0, root);
                pprint(0, root->parent);
                pprint(0, model->roots.size());
                pprint(0, root->hidden->id);
                for (TreeNode<Hidden> *child : root->children) {
                    pprint(4, ((TextCellRenderer*)child->columns[0])->text);
                    pprint(4, child);
                    pprint(4, child->parent);
                    pprint(4, root->children.size());
                    pprint(4, child->hidden->id);
                    for (TreeNode<Hidden> *last : child->children) {
                        pprint(8, ((TextCellRenderer*)last->columns[0])->text);
                        pprint(8, last);
                        pprint(8, last->parent);
                        pprint(8, child->children.size());
                        pprint(8, last->hidden->id);
                    }
                }
            }
        }
        delete model;


    app->run();

    return 0;
}