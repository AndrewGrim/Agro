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
        Font *mono = new Font("fonts/DejaVu/DejaVuSansMono.ttf", 16, Font::Type::Mono);
        app->onQuit = [&](Application *app) {
            delete mono;
            return true;
        };
        app->append(new Button("Top"), Fill::Horizontal);
        app->bind(SDLK_q, Mod::Ctrl, [&]{
            app->quit();
        });
        Box *box = new Box(Align::Horizontal);
            box->append(new Button("Left"), Fill::Vertical);
            TreeView<Hidden> *tv = new TreeView<Hidden>();
                tv->bind(SDLK_SPACE, Mod::Ctrl, [&]{
                    tv->clear();
                });
            {
                for (int i = 0; i < 7; i++) {
                    tv->append(new Button("Column: " + std::to_string(i)));
                }
                Tree<Hidden> *model = new Tree<Hidden>();
                {
                    TreeNode<Hidden> *root = nullptr;
                    for (int i = 0; i < 100; i++) {
                        std::vector<CellRenderer*> columns;
                            TextCellRenderer *renderer = new TextCellRenderer("Root: " + std::to_string(i));
                                renderer->font = mono;
                            columns.push_back(renderer);
                            for (int i = 1; i < 4; i++) {
                                TextCellRenderer *renderer = new TextCellRenderer("Column " + std::to_string(i));
                                    renderer->font = mono;
                                columns.push_back(renderer);
                            }
                        TreeNode<Hidden> *node = new TreeNode<Hidden>(columns, new Hidden(i));                    
                        root = model->append(nullptr, node);
                        for (int i = 0; i < 5; i++) {
                            std::vector<CellRenderer*> columns;
                                columns.push_back(new TextCellRenderer("Second Gen: " + std::to_string(i)));
                                columns.push_back(new TextCellRenderer("Row " + std::to_string(i)));
                                columns.push_back(new TextCellRenderer("Row " + std::to_string(i)));
                                columns.push_back(new TextCellRenderer("Row " + std::to_string(i)));
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