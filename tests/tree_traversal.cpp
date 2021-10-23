#include <cassert>

#include "../src/application.hpp"
#include "../src/controls/tree_view.hpp"
#include "../src/util.hpp"

int main(int argc, char **argv) {
    Application *app = Application::get();
        app->onReady = [&](Window *window) {
            if (argc > 1) {
                if (std::string(argv[1]) == std::string("quit")) {
                    window->quit();
                }
            }
        };
        auto tv = new TreeView<char>();
        tv->append(new Column<char>("col 1"));
            auto model = new Tree<char>();
                auto one = model->append(nullptr, new TreeNode<char>({new TextCellRenderer("1")}, nullptr));
                model->append(one, new TreeNode<char>({new TextCellRenderer("2")}, nullptr));
                auto three = model->append(nullptr, new TreeNode<char>({new TextCellRenderer("3")}, nullptr));
                model->append(three, new TreeNode<char>({new TextCellRenderer("4")}, nullptr));
                auto five = model->append(nullptr, new TreeNode<char>({new TextCellRenderer("5")}, nullptr));
                model->append(five, new TreeNode<char>({new TextCellRenderer("6")}, nullptr));
            tv->setModel(model);
        app->append(tv, Fill::Both);

        {
            int count = 0;
            tv->model()->forEachNode(tv->model()->roots, [&](TreeNode<char> *node) -> Traversal {
                int n = std::stoi(((TextCellRenderer*)node->columns[0])->text);
                if (n == 3) {
                    count++;
                    return Traversal::Continue;
                }
                count++;
                return Traversal::Continue;
            });
            assert(count == 6 && "When using continue we should traverse the entire tree!");
        }
        {
            int count = 0;
            tv->model()->forEachNode(tv->model()->roots, [&](TreeNode<char> *node) -> Traversal {
                int n = std::stoi(((TextCellRenderer*)node->columns[0])->text);
                if (n == 3) {
                    return Traversal::Next;
                }
                count++;
                return Traversal::Continue;
            });
            assert(count == 4 && "When using next we should jump to the next branch in the tree!");
        }
        {
            int count = 0;
            tv->model()->forEachNode(tv->model()->roots, [&](TreeNode<char> *node) -> Traversal {
                int n = std::stoi(((TextCellRenderer*)node->columns[0])->text);
                if (n == 3) {
                    return Traversal::Break;
                }
                count++;
                return Traversal::Continue;
            });
            assert(count == 2 && "When using break we should end the traversal immediately!");
        }
    app->run();

    return 0;
}
