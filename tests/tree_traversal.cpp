#include <cassert>

#include "../src/application.hpp"
#include "../src/controls/tree_view.hpp"
#include "../src/util.hpp"

int main(int argc, char **argv) {
    Application *app = Application::get();
        app->mainWindow()->onReady = [&](Window *window) {
            if (argc > 1) {
                if (String(argv[1]) == "quit") {
                    window->quit();
                }
            }
        };
        app->mainWindow()->center();
        auto tv = new TreeView<u8>();
        auto col = new Column<u8>("col 1");
        col->setExpand(true);
        // Note: you can use this to specify which drop should be drawn.
        // The default is 0b1111 with all enabled: Root, Child, Above, Below.
        // tv->drop_allow = 0b0011;
        tv->onNodeSelected.addEventListener([&](TreeView<u8> *widget, TreeNode<u8> *node) {
            app->startDrag(widget, node->columns[0], cast(TextCellRenderer*, node->columns[0])->text, &tv->m_focused);
        });
        tv->onDragDropped.addEventListener([&](Widget *widget, DragEvent event) {
            // TODO nemo doesnt allow for dropping any of the selected nodes on themselvs, reasonable
            // but atm we would draw them as if we allowed drops (something to think about and tweak)
            // perhaps we want something like ondraghovered where we call into client code
            // for actually showing the drops and it would be much easier then for them to reject
            // or accept specific stuff

            // TODO but what if we separately select child node of a parent thats already select and drop it on a different node
            // what nemo seems to do is to just treat them as one, which makes sense, we just need to detect that
            // some sort of processing step that deduplicates children that already have a parent selected

            ArrayList<TreeNode<u8>*> &selection = *cast(ArrayList<TreeNode<u8>*>*, event.data);
            if (selection.contains(tv->drop_action.node)) {
                tv->deselectAll();
                return;
            }

            bool parent_to_child = false;
            for (TreeNode<u8> *n : selection) {
                tv->m_model->forEachNode(cast(TreeNode<u8>*, n)->children, [&](TreeNode<u8> *node) -> Traversal {
                    if (node == tv->drop_action.node) { parent_to_child = true; return Traversal::Break; }
                    return Traversal::Continue;
                });
                if (parent_to_child) {
                    tv->deselectAll();
                    return;
                }
            }

            auto m = tv->m_model;
            for (TreeNode<u8> *n : selection) {
                m->remove(cast(TreeNode<u8>*, n));
            }
            switch (tv->drop_action.type) {
                case TreeView<u8>::DropAction::Type::Root: {
                    for (TreeNode<u8> *n : selection) {
                        m->insert(nullptr, m->roots.size(), cast(TreeNode<u8>*, n));
                    }
                    break;
                }
                case TreeView<u8>::DropAction::Type::Child: {
                    for (TreeNode<u8> *n : selection) {
                        m->insert(tv->drop_action.node, tv->drop_action.node->children.size(), cast(TreeNode<u8>*, n));
                    }
                    break;
                }
                case TreeView<u8>::DropAction::Type::Above: {
                    usize i = 0;
                    for (TreeNode<u8> *n : selection) {
                        m->insert(tv->drop_action.node->parent, tv->drop_action.node->parent_index + i++, cast(TreeNode<u8>*, n));
                    }
                    break;
                }
                case TreeView<u8>::DropAction::Type::Below: {
                    usize i = 0;
                    for (TreeNode<u8> *n : selection) {
                        m->insert(tv->drop_action.node->parent, tv->drop_action.node->parent_index + i++ + 1, cast(TreeNode<u8>*, n));
                    }
                    break;
                }
            }
            tv->deselectAll();

            // TODO workaround for model not updating when being changed
            tv->m_model = nullptr;
            tv->setModel(m);
        });
        tv->append(col);
            auto model = new Tree<u8>();
                auto one = model->append(nullptr, new TreeNode<u8>({new TextCellRenderer("1")}, nullptr));
                model->append(one, new TreeNode<u8>({new TextCellRenderer("2")}, nullptr));
                auto three = model->append(nullptr, new TreeNode<u8>({new TextCellRenderer("3")}, nullptr));
                model->append(three, new TreeNode<u8>({new TextCellRenderer("4")}, nullptr));
                auto five = model->append(nullptr, new TreeNode<u8>({new TextCellRenderer("5")}, nullptr));
                model->append(five, new TreeNode<u8>({new TextCellRenderer("6")}, nullptr));
            tv->setModel(model);
        app->mainWindow()->append(tv, Fill::Both);

        {
            i32 count = 0;
            tv->model()->forEachNode(tv->model()->roots, [&](TreeNode<u8> *node) -> Traversal {
                i32 n = std::stoi(((TextCellRenderer*)node->columns[0])->text.data());
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
            i32 count = 0;
            tv->model()->forEachNode(tv->model()->roots, [&](TreeNode<u8> *node) -> Traversal {
                i32 n = std::stoi(((TextCellRenderer*)node->columns[0])->text.data());
                if (n == 3) {
                    return Traversal::Next;
                }
                count++;
                return Traversal::Continue;
            });
            assert(count == 4 && "When using next we should jump to the next branch in the tree!");
        }
        {
            i32 count = 0;
            tv->model()->forEachNode(tv->model()->roots, [&](TreeNode<u8> *node) -> Traversal {
                i32 n = std::stoi(((TextCellRenderer*)node->columns[0])->text.data());
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
