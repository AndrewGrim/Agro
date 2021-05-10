#include <iostream>
#include <vector>

#include "../../../build/gui/application.hpp"
#include "../../../build/gui/controls/treeview.hpp"
#include "../../../build/gui/controls/spacer.hpp"

#include "database.hpp"
#include "contact.hpp"
#include "flag.hpp"

struct Hidden {
    int category = -1;
    int row = -1;
    std::string countrycode;
    Hidden(int _category, int _row, std::string _countrycode) : category{_category}, row{_row}, countrycode{_countrycode} {}
};

int main(int argc, char **argv) {
    Application *app = Application::get();
        app->onReady = [&](Window *window) {
            if (argc > 1) {
                if (std::string(argv[1]) == std::string("quit")) {
                    window->quit();
                }
            }
        };
        app->setTitle("Contacts 2021");
        app->resize(1200, 800);
        app->center();
        app->setMainWidget(new Box(Align::Horizontal));
        TreeView<Hidden> *tv;

        Box *box = new Box(Align::Vertical);
            Box *buttons = new Box(Align::Horizontal);
                Button *epxand = new Button("Expand All");
                    epxand->onMouseClick.addEventListener([&](Widget *button, MouseEvent event) {
                        tv->expandAll();
                    });
                buttons->append(epxand);
                Button *collapse = new Button("Collapse All");
                    collapse->onMouseClick.addEventListener([&](Widget *button, MouseEvent event) {
                        tv->collapseAll();
                    });
                buttons->append(collapse);
                Spacer *spacer = new Spacer();
                    spacer->setBackground(COLOR_WHITE);
                buttons->append(spacer, Fill::Both);
                Button *tree_mode = new Button("Tree Mode");
                    tree_mode->onMouseClick.addEventListener([&](Widget *button, MouseEvent event) {
                        tv->setTableMode(false);
                    });
                buttons->append(tree_mode);
                Button *table_mode = new Button("Table Mode");
                    table_mode->onMouseClick.addEventListener([&](Widget *button, MouseEvent event) {
                        tv->setTableMode(true);
                    });
                buttons->append(table_mode);
            box->append(buttons, Fill::Horizontal);

            tv = new TreeView<Hidden>(Size(50, 50));
                Column<Hidden> *row = new Column<Hidden>(
                    "Row", nullptr, HorizontalAlignment::Center,
                    [&](TreeNode<Hidden> *lhs, TreeNode<Hidden> *rhs) {
                        return lhs->hidden->row > rhs->hidden->row;
                    }
                );
                tv->append(row);
                Column<Hidden> *first_name = new Column<Hidden>(
                    "First Name", nullptr, HorizontalAlignment::Center,
                    [&](TreeNode<Hidden> *lhs, TreeNode<Hidden> *rhs) {
                        return ((TextCellRenderer*)lhs->columns[1])->text > ((TextCellRenderer*)rhs->columns[1])->text;
                    });
                tv->append(first_name);
                Column<Hidden> *last_name = new Column<Hidden>(
                    "Last Name", nullptr, HorizontalAlignment::Center,
                    [&](TreeNode<Hidden> *lhs, TreeNode<Hidden> *rhs) {
                        return ((TextCellRenderer*)lhs->columns[2])->text > ((TextCellRenderer*)rhs->columns[2])->text;
                    });
                tv->append(last_name);
                Column<Hidden> *email = new Column<Hidden>(
                    "Email", nullptr, HorizontalAlignment::Center,
                    [&](TreeNode<Hidden> *lhs, TreeNode<Hidden> *rhs) {
                        return ((TextCellRenderer*)lhs->columns[3])->text > ((TextCellRenderer*)rhs->columns[3])->text;
                    });
                tv->append(email);
                Column<Hidden> *countrycode = new Column<Hidden>(
                    "Countrycode", nullptr, HorizontalAlignment::Center,
                    [&](TreeNode<Hidden> *lhs, TreeNode<Hidden> *rhs) {
                        return lhs->hidden->countrycode > rhs->hidden->countrycode;
                    }
                );
                tv->append(countrycode);
                Column<Hidden> *id = new Column<Hidden>(
                    "ID", nullptr, HorizontalAlignment::Center,
                    [&](TreeNode<Hidden> *lhs, TreeNode<Hidden> *rhs) {
                        return ((TextCellRenderer*)lhs->columns[5])->text > ((TextCellRenderer*)rhs->columns[5])->text;
                    });
                tv->append(id);
                Column<Hidden> *category = new Column<Hidden>(
                    "Category", nullptr, HorizontalAlignment::Center,
                    [&](TreeNode<Hidden> *lhs, TreeNode<Hidden> *rhs) {
                        return lhs->hidden->category > rhs->hidden->category;
                    }
                );
                tv->append(category);
                Column<Hidden> *country_flag = new Column<Hidden>(
                    "Flag", nullptr, HorizontalAlignment::Center,
                    [&](TreeNode<Hidden> *lhs, TreeNode<Hidden> *rhs) {
                        return lhs->hidden->countrycode > rhs->hidden->countrycode;
                    }
                );
                tv->append(country_flag);

                Database db = Database("contacts2021.db");
                std::vector<Contact> contacts = db.loadAllContacts();
                std::unordered_map<std::string, std::shared_ptr<Texture>> textures = db.loadAllTextures();
                std::unordered_map<std::string, TreeNode<Hidden>*> country_nodes;
                Tree<Hidden> *model = new Tree<Hidden>();
                    { // Data
                        int row = 0;
                        for (auto c : contacts) {
                            std::vector<CellRenderer*> columns;
                                columns.push_back(new TextCellRenderer(std::to_string(row)));
                                    ((TextCellRenderer*)columns[0])->background = Color(0.9, 0.4, 0.25);
                                columns.push_back(new TextCellRenderer(c.first_name));
                                    ((TextCellRenderer*)columns[1])->background = Color(0.9, 0.7, 0.30);
                                columns.push_back(new TextCellRenderer(c.last_name));
                                    ((TextCellRenderer*)columns[2])->background = Color(0.9, 0.9, 0.35);
                                columns.push_back(new TextCellRenderer(c.email));
                                    ((TextCellRenderer*)columns[3])->background = Color(0.6, 0.9, 0.40);
                                columns.push_back(new TextCellRenderer(c.countrycode));
                                    ((TextCellRenderer*)columns[4])->background = Color(0.6, 0.7, 0.9);
                                columns.push_back(new TextCellRenderer(c.id));
                                    ((TextCellRenderer*)columns[5])->background = Color(0.9, 0.7, 0.9);
                                columns.push_back(new TextCellRenderer(c.category));
                                    ((TextCellRenderer*)columns[6])->background = Color(0.6, 0.3, 0.9);
                                auto search_texture = textures.find(c.countrycode);
                                if (search_texture != textures.end()) {
                                    Image *img = new Image(search_texture->second);
                                        img->setMinSize(Size(56, 32));
                                        img->setBackground(Color(0.9f, 0.9f, 0.9f));
                                    columns.push_back(new ImageCellRenderer(img));
                                }
                            TreeNode<Hidden> *node = new TreeNode<Hidden>(columns, new Hidden(c.category[0], row, c.countrycode));
                            auto search_country = country_nodes.find(c.countrycode);
                            if (search_country != country_nodes.end()) {
                                model->append(search_country->second, node);
                            } else {
                                country_nodes.insert(std::make_pair(c.countrycode, node));
                                model->append(nullptr, node);
                            }
                            row++;
                        }
                    }
                tv->setModel(model);
            box->append(tv, Fill::Both);
        app->append(box, Fill::Both);

        Box *vbox = new Box(Align::Vertical);
            vbox->setBackground(Color(0.9f, 0.9f, 0.9f));
            Image *s_img = new Image("lena.png");
                s_img->setMinSize(Size(364, 256));
                // TODO we could implement style for image to
                // to make this even better
                s_img->setBackground(Color(0.9f, 0.9f, 0.9f));
            vbox->append(s_img, Fill::Horizontal);
            Label *s_first_name = new Label("first_name");
            vbox->append(s_first_name, Fill::Horizontal);
            Label *s_last_name = new Label("last_name");
            vbox->append(s_last_name, Fill::Horizontal);
            Label *s_email = new Label("email");
            vbox->append(s_email, Fill::Horizontal);
            Label *s_countrycode = new Label("countrycode");
            vbox->append(s_countrycode, Fill::Horizontal);
            Label *s_id = new Label("id");
            vbox->append(s_id, Fill::Horizontal);
            Label *s_category = new Label("category");
            vbox->append(s_category, Fill::Horizontal);
            tv->onNodeSelected = [&](TreeView<Hidden> *treeview, TreeNode<Hidden> *node) {
                s_img->setTexture(((ImageCellRenderer*)node->columns[7])->image->texture());
                s_first_name->setText("First Name: " + ((TextCellRenderer*)node->columns[1])->text);
                s_last_name->setText("Last Name: " + ((TextCellRenderer*)node->columns[2])->text);
                s_email->setText("Email: " + ((TextCellRenderer*)node->columns[3])->text);
                s_countrycode->setText("Countrycode: " + ((TextCellRenderer*)node->columns[4])->text);
                s_id->setText("ID: " + ((TextCellRenderer*)node->columns[5])->text);
                s_category->setText("Category: " + ((TextCellRenderer*)node->columns[6])->text);
            };
        app->append(vbox, Fill::Vertical);
        app->onReady = [&](Window *win) {
            tv->select((size_t)0);
        };        
    app->run();

    return 0;
}