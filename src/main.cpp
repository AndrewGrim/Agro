#include "util.hpp"
#include "application.hpp"
#include "controls/treeview.hpp"

int main(int argc, char **argv) { 
    Application *app = Application::get();
        app->resize(400, 800);
        TreeView<void> *tv = new TreeView<void>();
            tv->append(new Column<void>("Testing treelines at more than 1 depth!"));
            Tree<void> *model = new Tree<void>();
                for (int n = 0; n < 2; n++) {
                    TreeNode<void> *parent = nullptr;
                    for (int i = 0; i < 5; i++) {
                        std::vector<CellRenderer*> columns = { new TextCellRenderer(std::to_string(i) + ":1") };
                        TreeNode<void> *node = new TreeNode<void>(columns, nullptr);
                        parent = model->append(parent, node);
                        {
                            std::vector<CellRenderer*> columns = { new TextCellRenderer(std::to_string(i) + ":2") };
                            TreeNode<void> *node = new TreeNode<void>(columns, nullptr);
                            model->append(parent->parent, node);
                        }
                    }
                }
            tv->setModel(model);
        app->append(tv, Fill::Both);
    app->run();

    return 0;
}