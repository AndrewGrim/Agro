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
    Application *app = new Application("Application", Size(1600, 1000));
        Font *mono = new Font("fonts/DejaVu/DejaVuSansMono.ttf", 16, Font::Type::Mono);
        Font *small = new Font("fonts/DejaVu/DejaVuSans.ttf", 12, Font::Type::Sans);
        Font *big = new Font("fonts/DejaVu/DejaVuSans-Bold.ttf", 22, Font::Type::Sans);
        app->onQuit = [&](Application *app) {
            delete mono;
            delete small;
            delete big;
            return true;
        };
        TreeView<Hidden> *tv;
        Box *box = new Box(Align::Horizontal);
        {
            Button *expand = new Button("Expand Selected");
                expand->onMouseClick = [&](MouseEvent event) {
                    tv->expand(tv->selected());
                };
            box->append(expand);
            Button *expand_recursive = new Button("Expand Recursively");
                expand_recursive->onMouseClick = [&](MouseEvent event) {
                    tv->expandRecursively(tv->selected());
                };
            box->append(expand_recursive);
            Button *expand_all = new Button("Expand All");
                expand_all->onMouseClick = [&](MouseEvent event) {
                    tv->expandAll();
                };
            box->append(expand_all);

            Label *space = new Label("");
            box->append(space, Fill::Both);

            Button *collapse = new Button("Collapse Selected");
                collapse->onMouseClick = [&](MouseEvent event) {
                    tv->collapse(tv->selected());
                };
            box->append(collapse);
            Button *collapse_recursive = new Button("Collapse Recursively");
                collapse_recursive->onMouseClick = [&](MouseEvent event) {
                    tv->collapseRecursively(tv->selected());
                };
            box->append(collapse_recursive);
            Button *collapse_all = new Button("Collapse All");
                collapse_all->onMouseClick = [&](MouseEvent event) {
                    tv->collapseAll();
                };
            box->append(collapse_all);
        }
        app->append(box, Fill::Horizontal);
        app->bind(SDLK_q, Mod::Ctrl, [&]{
            app->quit();
        });
        int id = 0;
        tv = new TreeView<Hidden>();
            // tv->setGridLines(GridLines::None);
            // tv->setIndent(12);
            // tv->onNodeHovered = [&](TreeNode<Hidden> *node) {
            //     println("Hovered ID: " + std::to_string(node->hidden->id));
            // };
            // tv->onNodeSelected = [&](TreeNode<Hidden> *node) {
            //     println("Selected ID: " + std::to_string(node->hidden->id));
            // };
            // tv->onNodeDeselected = [&](TreeNode<Hidden> *node) {
            //     println("Deselected ID: " + std::to_string(node->hidden->id));
            // };
            // tv->onNodeCollapsed = [&](TreeNode<Hidden> *node) {
            //     println("Collapsed ID: " + std::to_string(node->hidden->id));
            // };
            // tv->onNodeExpanded = [&](TreeNode<Hidden> *node) {
            //     println("Expanded ID: " + std::to_string(node->hidden->id));
            // };
            // tv->bind(SDLK_SPACE, Mod::Ctrl, [&]{
            //     tv->clear();
            // });
        {
            tv->append(new Button("============ Column: " + std::to_string(0) + " ============"));
            for (int i = 1; i < 7; i++) {
                tv->append(new Button("=== Column: " + std::to_string(i) + " ==="));
            }
            Tree<Hidden> *model = new Tree<Hidden>();
            {
                TreeNode<Hidden> *root = nullptr;
                for (int i = 0; i < 5; i++) {
                    std::vector<CellRenderer*> columns;
                        TextCellRenderer *renderer = new TextCellRenderer("Root: " + std::to_string(i));
                            renderer->font = mono;
                        columns.push_back(renderer);
                        for (int i = 1; i < 7; i++) {
                            TextCellRenderer *renderer = new TextCellRenderer("Column " + std::to_string(i));
                                renderer->align = HorizontalAlignment::Center;
                            if (i == 2) {
                                renderer->font = small;
                            } else if (i == 3) {
                                renderer->font = big;
                            } else {
                                renderer->font = mono;
                            }
                            columns.push_back(renderer);
                        }
                    TreeNode<Hidden> *node = new TreeNode<Hidden>(columns, new Hidden(id++));
                        // node->is_collapsed = true;
                    root = model->append(nullptr, node);
                    {
                        std::vector<CellRenderer*> columns;
                        TextCellRenderer *renderer = new TextCellRenderer("Gen: 1");
                        columns.push_back(renderer);
                        for (int i = 1; i < 7; i++) {
                            renderer = new TextCellRenderer("Column " + std::to_string(i));
                                renderer->align = HorizontalAlignment::Center;
                            columns.push_back(renderer);
                        }
                        TreeNode<Hidden> *node = new TreeNode<Hidden>(columns, new Hidden(id++));
                            // node->is_collapsed = true;
                        TreeNode<Hidden> *last = model->append(root, node);
                        {
                            for (int j = 0; j < 3; j++) {
                                std::vector<CellRenderer*> columns;
                                TextCellRenderer *renderer = new TextCellRenderer("Inner Gen: 1");
                                columns.push_back(renderer);
                                for (int i = 1; i < 7; i++) {
                                    renderer = new TextCellRenderer("Column " + std::to_string(i));
                                        renderer->align = HorizontalAlignment::Center;
                                    columns.push_back(renderer);
                                }
                                TreeNode<Hidden> *node = new TreeNode<Hidden>(columns, new Hidden(id++));
                                    // node->is_collapsed = true;
                                TreeNode<Hidden> *bruh = model->append(last, node);
                                {
                                    std::vector<CellRenderer*> columns;
                                    TextCellRenderer *renderer = new TextCellRenderer("Too Many Gens: 1");
                                    columns.push_back(renderer);
                                    for (int i = 1; i < 7; i++) {
                                        renderer = new TextCellRenderer("Column " + std::to_string(i));
                                            renderer->align = HorizontalAlignment::Center;
                                        columns.push_back(renderer);
                                    }
                                    model->append(bruh, new TreeNode<Hidden>(columns, new Hidden(id++)));
                                }
                            }
                        }
                    }
                    {
                        std::vector<CellRenderer*> columns;
                        TextCellRenderer *renderer = new TextCellRenderer("Gen: 2");
                        columns.push_back(renderer);
                        for (int i = 1; i < 7; i++) {
                            renderer = new TextCellRenderer("Column " + std::to_string(i));
                                renderer->align = HorizontalAlignment::Center;
                            columns.push_back(renderer);
                        }
                        TreeNode<Hidden> *node = new TreeNode<Hidden>(columns, new Hidden(id++));
                            // node->is_collapsed = true;
                        TreeNode<Hidden> *last = model->append(root, node);
                        {
                            for (int j = 0; j < 3; j++) {
                                std::vector<CellRenderer*> columns;
                                TextCellRenderer *renderer = new TextCellRenderer("Inner Gen: 2");
                                columns.push_back(renderer);
                                for (int i = 1; i < 7; i++) {
                                    renderer = new TextCellRenderer("Column " + std::to_string(i));
                                        renderer->align = HorizontalAlignment::Center;
                                    columns.push_back(renderer);
                                }
                                TreeNode<Hidden> *node = new TreeNode<Hidden>(columns, new Hidden(id++));
                                    // node->is_collapsed = true;
                                TreeNode<Hidden> *bruh = model->append(last, node);
                                {
                                    std::vector<CellRenderer*> columns;
                                    TextCellRenderer *renderer = new TextCellRenderer("Too Many Gens: 2");
                                    columns.push_back(renderer);
                                    for (int i = 1; i < 7; i++) {
                                        renderer = new TextCellRenderer("Column " + std::to_string(i));
                                            renderer->align = HorizontalAlignment::Center;
                                        columns.push_back(renderer);
                                    }
                                    model->append(bruh, new TreeNode<Hidden>(columns, new Hidden(id++)));
                                }
                            }
                        }
                    }
                    {
                        std::vector<CellRenderer*> columns;
                        TextCellRenderer *renderer = new TextCellRenderer("Gen: 3");
                        columns.push_back(renderer);
                        for (int i = 1; i < 7; i++) {
                            renderer = new TextCellRenderer("Column " + std::to_string(i));
                                renderer->align = HorizontalAlignment::Center;
                            columns.push_back(renderer);
                        }
                        TreeNode<Hidden> *node = new TreeNode<Hidden>(columns, new Hidden(id++));  
                            // node->is_collapsed = true;
                        TreeNode<Hidden> *last = model->append(root, node);
                        {
                            for (int j = 0; j < 3; j++) {
                                std::vector<CellRenderer*> columns;
                                TextCellRenderer *renderer = new TextCellRenderer("Inner Gen: 3");
                                columns.push_back(renderer);
                                for (int i = 1; i < 7; i++) {
                                    renderer = new TextCellRenderer("Column " + std::to_string(i));
                                        renderer->align = HorizontalAlignment::Center;
                                    columns.push_back(renderer);
                                }
                                TreeNode<Hidden> *node = new TreeNode<Hidden>(columns, new Hidden(id++));
                                    // node->is_collapsed = true;
                                TreeNode<Hidden> *bruh = model->append(last, node);
                                {
                                    std::vector<CellRenderer*> columns;
                                    TextCellRenderer *renderer = new TextCellRenderer("Too Many Gens: 3");
                                    columns.push_back(renderer);
                                    for (int i = 1; i < 7; i++) {
                                        renderer = new TextCellRenderer("Column " + std::to_string(i));
                                            renderer->align = HorizontalAlignment::Center;
                                        columns.push_back(renderer);
                                    }
                                    model->append(bruh, new TreeNode<Hidden>(columns, new Hidden(id++)));
                                }
                            }
                        }
                    }
                }
            }
            tv->setModel(model);
        }
        app->append(tv, Fill::Both);
    app->run();

    return 0;
}