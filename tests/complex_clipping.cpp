#include "../src/application.hpp"
#include "../src/controls/tree_view.hpp"
#include "../src/controls/splitter.hpp"
#include "../src/controls/text_edit.hpp"

int main(int argc, char **argv) {
    Application *app = Application::get();
        app->mainWindow()->onReady = [&](Window *window) {
            if (argc > 1) {
                if (String(argv[1]) == "quit") {
                    window->quit();
                }
            }
        };
        app->mainWindow()->setTitle("Complex Clipping Test");
        app->mainWindow()->resize(1000, 1000);
        app->mainWindow()->center();
        auto h_split = new Splitter(Align::Horizontal);
            ScrolledBox *left = new ScrolledBox(Align::Vertical, Size(800, 400));
                for (u8 i = 'a'; i <= 'z'; i++) {
                    left->append(new TextEdit(String::repeat(i, 30)), Fill::Both);
                }
                for (u8 i = 'A'; i <= 'Z'; i++) {
                    left->append(new TextEdit(String::repeat(i, 30)), Fill::Both);
                }
            h_split->left(left);
            ScrolledBox *right = new ScrolledBox(Align::Vertical, Size(800, 400));
                ScrolledBox *inner_right = new ScrolledBox(Align::Vertical, Size(400, 400));
                    for (u8 i = 'A'; i < 'Z'; i++) {
                        inner_right->append(new Button(String::repeat(i, 100)), Fill::Both);
                    }
                right->append(inner_right, Fill::Both);
                auto tv = new TreeView<u8>(Size(400, 800));
                    for (i32 i = 0; i < 5; i++) {
                        tv->append(new Column<u8>("Column: " + toString(i)));
                    }
                    auto model = new Tree<u8>();
                        for (i32 row = 0; row < 100; row++) {
                            std::vector<Drawable*> columns = {
                                new TextCellRenderer("Row: " + toString(row) + ", Column: 1"),
                                new TextCellRenderer("Row: " + toString(row) + ", Column: 2"),
                                new TextCellRenderer("Row: " + toString(row) + ", Column: 3"),
                                new TextCellRenderer("Row: " + toString(row) + ", Column: 4"),
                                new TextCellRenderer("Row: " + toString(row) + ", Column: 5"),
                            };
                            auto node = new TreeNode<u8>(columns, nullptr);
                            model->append(nullptr, node);
                        }
                    tv->setTableMode(true);
                    tv->setModel(model);
                right->append(tv, Fill::Both);
            h_split->right(right);
            h_split->setSplit(0.2);
        app->mainWindow()->append(h_split, Fill::Both);
    app->run();

    return 0;
}
