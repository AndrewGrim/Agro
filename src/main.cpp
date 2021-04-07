#include "util.hpp"
#include "application.hpp"
#include "controls/treeview.hpp"

int main(int argc, char **argv) { 
    Application *app = Application::get();
        app->resize(900, 900);
        app->center();
        auto tv = new TreeView<void>();
            for (int i = 0; i < 5; i++) {
                tv->append(new Column<void>("Column: " + std::to_string(i)));
            }
            auto model = new Tree<void>();
                for (int row = 0; row < 1000; row++) {
                    std::vector<CellRenderer*> columns = {
                        new TextCellRenderer("Row: " + std::to_string(row) + ", Column: 1"),
                        new TextCellRenderer("Row: " + std::to_string(row) + ", Column: 2"),
                        new TextCellRenderer("Row: " + std::to_string(row) + ", Column: 3"),
                        new TextCellRenderer("Row: " + std::to_string(row) + ", Column: 4"),
                        new TextCellRenderer("Row: " + std::to_string(row) + ", Column: 5"),
                    };
                    auto node = new TreeNode<void>(columns, nullptr);
                    model->append(nullptr, node);
                }
            tv->setTableMode(true);
            tv->setModel(model);
        app->append(tv, Fill::Both);
    app->run();

    return 0;
}