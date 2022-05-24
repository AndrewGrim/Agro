#include "../src/application.hpp"
#include "../src/controls/tree_view.hpp"
#include "../src/controls/text_edit.hpp"

struct Hidden {
    i32 id = -1;
    Hidden(i32 id) : id{id} {}
};

int main(int argc, char **argv) {
    Application *app = Application::get();
        app->mainWindow()->onReady = [&](Window *window) {
            if (argc > 1) {
                if (String(argv[1]) == "quit") {
                    window->quit();
                }
            }
        };
        app->mainWindow()->setTitle("TreeView");
        app->mainWindow()->resize(1200, 600);
        app->mainWindow()->center();
        TreeView<Hidden> *tv = nullptr;
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
        app->mainWindow()->append(h_box, Fill::Horizontal);
        tv = new TreeView<Hidden>();
            for (i32 i = 0; i < 4; i++) {
                auto c = new Column<Hidden>(
                    "Column: " + toString(i),
                    nullptr,
                    HorizontalAlignment::Center,
                    [](TreeNode<Hidden> *lhs, TreeNode<Hidden> *rhs) {
                        return lhs->hidden->id > rhs->hidden->id;
                    }
                );
                if (i == 1) c->setExpand(true);
                if (i == 2) c->setWidth(350);
                tv->append(c);
            }
            Tree<Hidden> *model = new Tree<Hidden>();
                for (i32 i = 0; i < 1000; i++) {
                    std::vector<Drawable*> columns = {
                        new TextCellRenderer("Row: " + toString(i) + "\nColumn: 0"),
                        new TextCellRenderer("Row: " + toString(i) + ", Column: 1"),
                        new ImageTextCellRenderer((new Image(app->icons["close"]))->setForeground(COLOR_BLACK), "Row: " + toString(i) + "\nColumn: 2"),
                        i % 2 == 0 ? new Button("Button") : ((Widget*)(new TextEdit())->setPlaceholderText("Row: " + toString(i) + ", Col: 3")),
                    };
                    ((TextCellRenderer*)columns[0])->h_align = HorizontalAlignment::Left;
                    ((TextCellRenderer*)columns[1])->h_align = HorizontalAlignment::Right;
                    ((ImageTextCellRenderer*)columns[2])->h_align = HorizontalAlignment::Center;
                    ((Widget*)columns[3])->onMouseDown.addEventListener([](Widget *widget, MouseEvent event) {
                        println(widget->name());
                    });
                    TreeNode<Hidden> *node = new TreeNode<Hidden>(columns, new Hidden(i));
                    model->append(nullptr, node);
                }
            tv->setModel(model);
            tv->setTableMode(true);
            tv->setMode(Mode::Unroll);
        app->mainWindow()->append(tv, Fill::Both);
    app->run();

    return 0;
}
