#include "../src/application.hpp"
#include "../src/controls/treeview.hpp"

int main(int argc, char **argv) { 
    Application *app = Application::get();
        app->onReady = [&](Window *window) {
            if (argc > 1) {
                if (std::string(argv[1]) == std::string("quit")) {
                    window->quit();
                }
            }
        };
        app->setTitle("TreeView");
        app->resize(1200, 600);
        app->center();
        TreeView<void> *tv = nullptr;
        Box *h_box = new Box(Align::Horizontal);
            Button *scroll = new Button("Set Scroll");
                scroll->onMouseClick.addEventListener([&](Widget *button, MouseEvent event) {
                    tv->setMode(Mode::Scroll);
                });
            h_box->append(scroll, Fill::Both);
            Button *unroll = new Button("Set Unroll");
                unroll->onMouseClick.addEventListener([&](Widget *button, MouseEvent event) {
                    tv->setMode(Mode::Unroll);
                });
            h_box->append(unroll, Fill::Both);
        app->append(h_box, Fill::Horizontal); 
        tv = new TreeView<void>();
            for (int i = 0; i < 7; i++) {
                tv->append(new Column<void>("Column: " + std::to_string(i)));
            }
            Tree<void> *model = new Tree<void>();
                for (int i = 0; i < 1000; i++) {
                    std::vector<CellRenderer*> columns = {
                        new TextCellRenderer("Row: " + std::to_string(i) + ", Column: 0"),
                        new TextCellRenderer("Row: " + std::to_string(i) + ", Column: 1"),
                        new TextCellRenderer("Row: " + std::to_string(i) + ", Column: 2"),
                        new TextCellRenderer("Row: " + std::to_string(i) + ", Column: 3"),
                        new TextCellRenderer("Row: " + std::to_string(i) + ", Column: 4"),
                        new TextCellRenderer("Row: " + std::to_string(i) + ", Column: 5"),
                        new TextCellRenderer("Row: " + std::to_string(i) + ", Column: 6"),
                    };
                    TreeNode<void> *node = new TreeNode<void>(columns, nullptr);
                    model->append(nullptr, node);
                }
            tv->setModel(model);
            tv->setTableMode(true);
            tv->setMode(Mode::Unroll);
        app->append(tv, Fill::Both);
    app->run();

    return 0; 
}