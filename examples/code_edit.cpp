#include "../src/util.hpp"
#include "../src/stop_watch.hpp"
#include "../src/application.hpp"
#include "../src/core/fs.hpp"
#include "../src/controls/code_edit.hpp"
#include "../src/controls/tree_view.hpp"
#include "../src/controls/splitter.hpp"

struct Hidden {
    fs::Dir::Iterator::Entry::Type type;
    Hidden(fs::Dir::Iterator::Entry::Type type) : type{type} {}
};

void recurseDir(Tree<Hidden> *model, TreeNode<Hidden> *parent_node, String path) {
    fs::Dir dir(path);
    auto iter = dir.iter();
    while ((iter = iter.next())) {
        if (iter.entry.type == fs::Dir::Iterator::Entry::Type::Directory) {
            TreeNode<Hidden> *new_node = model->append(parent_node, new TreeNode<Hidden>(
                {
                    new ImageTextCellRenderer(new Image(Application::get()->icons["directory"]), iter.entry.name)
                },
                new Hidden(iter.entry.type)
            ));
            String child_path = String::format("%s/%s", dir.path.data(), iter.entry.name.data());
            recurseDir(model, new_node, child_path);
        } else {
            model->append(parent_node, new TreeNode<Hidden>(
                {
                    new ImageTextCellRenderer(new Image(Application::get()->icons["file"]), iter.entry.name)
                },
                new Hidden(iter.entry.type)
            ));
        }
    }
}

int main(int argc, char **argv) {
    Application *app = Application::get();
        app->mainWindow()->onReady = [&](Window *window) {
            if (argc > 1) {
                if (String(argv[1]) == "quit") {
                    window->quit();
                }
            }
        };
        app->mainWindow()->resize(1400, 1000);
        app->mainWindow()->center();
        auto split = new Splitter(Align::Horizontal);
        split->setSplit(0.25);
        app->mainWindow()->append(split, Fill::Both);
        TreeView<Hidden> *tv = nullptr;
        CodeEdit *edit = nullptr;
        {
            tv = new TreeView<Hidden>();
                tv->setGridLines(GridLines::None);
                tv->hideColumnHeaders();
                auto col = new Column<Hidden>(
                    "Name",
                    nullptr,
                    HorizontalAlignment::Center,
                    [](TreeNode<Hidden> *lhs, TreeNode<Hidden> *rhs) {
                        if ((i32)lhs->hidden->type == (i32)rhs->hidden->type) {
                            auto l = (ImageTextCellRenderer*)lhs->columns[0];
                            auto r = (ImageTextCellRenderer*)rhs->columns[0];
                            return memcmp(
                                ("" + l->text).toLower().data(),
                                ("" + r->text).toLower().data(),
                                l->text.size() < r->text.size() ? l->text.size() : r->text.size()
                            ) < 0;
                        }
                        return (i32)lhs->hidden->type > (i32)rhs->hidden->type;
                    }
                );
                col->setExpand(true);
                tv->append(col);
                Tree<Hidden> *model = new Tree<Hidden>();
                recurseDir(model, nullptr, ".");
                tv->setModel(model);
                col->sort(Sort::Ascending);
                tv->collapseAll();
                tv->onNodeSelected.addEventListener([&](TreeView<Hidden> *tv, TreeNode<Hidden> *node) {
                    if (node->hidden->type == fs::Dir::Iterator::Entry::Type::Directory) { return; }
                    String path = ((ImageTextCellRenderer*)node->columns[0])->text;
                    auto current = node;
                    while (current->parent) {
                        current = current->parent;
                        path.insert(0, "/");
                        path.insert(0, ((ImageTextCellRenderer*)current->columns[0])->text);
                    }
                    if (fs::getSize(path)) {
                        edit->setText(fs::load(path).unwrap());
                    }
                });
            split->left(tv);
        }
        {
            std::shared_ptr<Font> mono = std::shared_ptr<Font>(new Font(Application::get(), "fonts/JetBrainsMono/JetBrainsMono-Regular.ttf", 14, Font::Type::Mono));
            edit = new CodeEdit(fs::load("src/core/string.cpp").unwrap(), Size(200, 200));
            edit->setFont(mono);
            split->right(edit);
        }
    app->run();

    return 0;
}
