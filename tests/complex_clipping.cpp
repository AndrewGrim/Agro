#include "../src/application.hpp"
#include "../src/controls/treeview.hpp"
#include "../src/controls/splitter.hpp"
#include "../src/controls/lineedit.hpp"

int main(int argc, char **argv) { 
    Application *app = Application::get();
        app->onReady = [&](Window *window) {
            if (argc > 1) {
                if (std::string(argv[1]) == std::string("quit")) {
                    window->quit();
                }
            }
        };
        app->setTitle("Complex Clipping Test");
        app->resize(1000, 1000);
        app->center();
        auto h_split = new Splitter(Align::Horizontal);
            ScrolledBox *left = new ScrolledBox(Align::Vertical, Size(800, 400));
                for (char i = 'a'; i <= 'z'; i++) {
                    left->append(new LineEdit(std::string(30, i)), Fill::Both);
                }
                for (char i = 'A'; i <= 'Z'; i++) {
                    left->append(new LineEdit(std::string(30, i)), Fill::Both);
                }
            h_split->left(left);
            ScrolledBox *right = new ScrolledBox(Align::Vertical, Size(800, 400));
                ScrolledBox *inner_right = new ScrolledBox(Align::Vertical, Size(400, 400));
                    for (char i = 'A'; i < 'Z'; i++) {
                        inner_right->append(new Button(std::string(100, i)), Fill::Both);
                    }
                right->append(inner_right, Fill::Both);
                auto tv = new TreeView<void>(Size(400, 800));
                    for (int i = 0; i < 5; i++) {
                        tv->append(new Column<void>("Column: " + std::to_string(i)));
                    }
                    auto model = new Tree<void>();
                        for (int row = 0; row < 100; row++) {
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
                right->append(tv, Fill::Both);
            h_split->right(right);
            h_split->setSplit(0.2f);
        app->append(h_split, Fill::Both);
    app->run();

    return 0;
}