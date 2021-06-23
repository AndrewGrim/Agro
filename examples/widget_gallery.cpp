#include "../src/util.hpp"
#include "../src/application.hpp"
#include "../src/controls/box.hpp"
#include "../src/controls/group_box.hpp"
#include "../src/controls/note_book.hpp"
#include "../src/controls/label.hpp"
#include "../src/controls/button.hpp"
#include "../src/controls/line_edit.hpp"
#include "../src/controls/splitter.hpp"
#include "../src/controls/tree_view.hpp"
#include "../src/controls/color_picker.hpp"

Widget* basic(Application &app) {
    Box *box = new Box(Align::Vertical);
        Box *labels_and_buttons = new Box(Align::Horizontal);
            GroupBox *labels = new GroupBox(Align::Vertical, "Labels");
                Label *lr = new Label("Right");
                    lr->setHorizontalAlignment(HorizontalAlignment::Right);
                Label *lc = new Label("This text\nspans\nmultiple lines,\nand is center aligned.");
                    lc->setHorizontalAlignment(HorizontalAlignment::Center);
                labels->append(new Label("Left"), Fill::Both);
                labels->append(lr, Fill::Both);
                labels->append(lc, Fill::Both);

            labels_and_buttons->append(labels, Fill::Both);

            GroupBox *buttons = new GroupBox(Align::Vertical, "Buttons");
                buttons->append(new Button("Button"));
                buttons->append(new Button((new Image(app.icons["close_thin"]))->setForeground(app.dc->iconForeground(box->style))));
                Button *image_and_text = new Button(new Image(app.icons["close_thin"]));
                    image_and_text->m_image->setForeground(app.dc->iconForeground(box->style));
                    image_and_text->setText("Button");
                buttons->append(image_and_text, Fill::Both);
            labels_and_buttons->append(buttons, Fill::Both);
        box->append(labels_and_buttons, Fill::Both);

        Box *images_and_color = new Box(Align::Horizontal);
            GroupBox *images = new GroupBox(Align::Horizontal, "Images");
                auto lena = std::make_shared<Texture>("lena.png");
                images->append((new Image(lena))->setMinSize(Size(24, 24))->setForeground(Color("#ff0000")));
                images->append((new Image(lena))->setMinSize(Size(56, 56))->setForeground(Color("#00ff00")));
                images->append((new Image(lena))->setMinSize(Size(72, 72))->setForeground(Color("#0000ff")));
                images->append((new Image(lena))->setMinSize(Size(24, 24))->setExpand(true), Fill::Both);
                images->append((new Image(lena))->setMinSize(Size(24, 24))->setExpand(true)->setMaintainAspectRatio(false), Fill::Both);
            images_and_color->append(images, Fill::Both);

            GroupBox *color_picker = new GroupBox(Align::Horizontal, "ColorPicker");
                color_picker->append(new ColorPicker());
            images_and_color->append(color_picker);
        box->append(images_and_color, Fill::Horizontal);

        GroupBox *line_edits = new GroupBox(Align::Vertical, "LineEdits");
            line_edits->append(new LineEdit("", "Placeholder text", 200));
            line_edits->append(new LineEdit("Default text", "", 300));
            line_edits->append(new LineEdit("", ""), Fill::Horizontal);
        box->append(line_edits, Fill::Horizontal);

    return box;
}

Widget* slidersAndScrollbars(Application &app) {
    Box *box = new Box(Align::Vertical);
        GroupBox *sliders = new GroupBox(Align::Horizontal, "Sliders");
            Slider *sh = new Slider(Align::Horizontal, 0.5);
                sh->m_slider_button_size = 50;
            sliders->append(sh, Fill::Horizontal);
            sliders->append(new Slider(Align::Vertical, 0.5), Fill::Vertical);
        box->append(sliders, Fill::Both);

        GroupBox *scroll_bars = new GroupBox(Align::Horizontal, "ScrollBars");
            ScrollBar *sbh = new ScrollBar(Align::Horizontal);
                sbh->m_slider->m_slider_button_size = 50;
            scroll_bars->append(sbh, Fill::Horizontal);
            scroll_bars->append(new ScrollBar(Align::Vertical), Fill::Vertical);
        box->append(scroll_bars, Fill::Both);

    return box;
}

Widget* splitter(Application &app) {
    Box * box = new Box(Align::Vertical);
        Splitter *h = new Splitter(Align::Horizontal);
            h->left(new Button("Left"));
            h->right(new Button("Right"));
        box->append(h, Fill::Both);

        Splitter *v = new Splitter(Align::Vertical);
            v->top(new Button("Top"));
            v->bottom(new Button("Bottom"));
        box->append(v, Fill::Both);

    return box;
}

struct Hidden {
    int id = -1;
    Hidden(int id) : id{id} {}
};

Widget* treeView(Application &app) {
    ScrolledBox * box = new ScrolledBox(Align::Horizontal);
        {
            GroupBox *group = new GroupBox(Align::Vertical, "Mode::Unroll");
                TreeView<Hidden> *tv = new TreeView<Hidden>();
                    tv->hideColumnHeaders();
                    for (int i = 1; i < 4; i++) {
                        auto c = new Column<Hidden>(
                            "Column " + std::to_string(i), nullptr, HorizontalAlignment::Center,
                            [](TreeNode<Hidden> *lhs, TreeNode<Hidden> *rhs) {
                                return lhs->hidden->id > rhs->hidden->id;
                            }
                        );
                        c->setExpand(true);
                        tv->append(c);
                    }
                    Tree<Hidden> *model = new Tree<Hidden>();
                    for (int i = 1; i < 101; i++) {
                        model->append(nullptr, new TreeNode<Hidden>(
                            {
                                new TextCellRenderer("Row " + std::to_string(i)),
                                new Button("TreeView"),
                                new LineEdit("", "TreeView"),
                            },
                            new Hidden(i)
                        ));
                    }
                    tv->setModel(model);
                    tv->setTableMode(true);
                    tv->setMode(Mode::Unroll);
                group->append(tv, Fill::Both);
            box->append(group, Fill::Both);
        }
        {
            GroupBox *group = new GroupBox(Align::Vertical, "Mode::Scroll");
                TreeView<Hidden> *tv = new TreeView<Hidden>(Size(400, 400));
                    for (int i = 1; i < 4; i++) {
                        auto c = new Column<Hidden>(
                            "Column " + std::to_string(i), nullptr, HorizontalAlignment::Center,
                            [](TreeNode<Hidden> *lhs, TreeNode<Hidden> *rhs) {
                                return lhs->hidden->id > rhs->hidden->id;
                            }
                        );
                        tv->append(c);
                        if (i == 1) { c->setExpand(true); }
                    }
                    Tree<Hidden> *model = new Tree<Hidden>();
                    int count = 0;
                    float ic = 0.0f;
                    for (int i = 1; i < 6; i++) {
                        TreeNode<Hidden> *parent = nullptr;
                        for (int j = 1; j < 21; j++) {
                            if (j == 1) {
                                parent = model->append(parent, new TreeNode<Hidden>(
                                    {
                                        new ImageTextCellRenderer((new Image(app.icons["up_arrow"]))->setForeground(Color(0.7f, 0.7f, ic)), "Row " + std::to_string(count)),
                                        new ImageCellRenderer((new Image(app.icons["up_arrow"]))->flipVertically()->setForeground(Color(0.7f, ic, 0.7f))),
                                        new MultipleImagesCellRenderer({
                                            *(Image(app.icons["close"]).setForeground(Color(ic, 0.7f, 0.7f))),
                                            *(Image(app.icons["up_arrow"]).setForeground(Color(ic, 0.7f, 0.7f))),
                                            *(Image(app.icons["close_thin"]).setForeground(Color(ic, 0.7f, 0.7f))),
                                        }),
                                    },
                                    new Hidden(count)
                                ));
                            } else {
                                model->append(parent, new TreeNode<Hidden>(
                                    {
                                        new ImageTextCellRenderer((new Image(app.icons["up_arrow"]))->setForeground(Color(0.7f, 0.7f, ic)), "Row " + std::to_string(count)),
                                        new ImageCellRenderer((new Image(app.icons["up_arrow"]))->flipVertically()->setForeground(Color(0.7f, ic, 0.7f))),
                                        new MultipleImagesCellRenderer({
                                            *(Image(app.icons["close"]).setForeground(Color(ic, 0.7f, 0.7f))),
                                            *(Image(app.icons["up_arrow"]).setForeground(Color(ic, 0.7f, 0.7f))),
                                            *(Image(app.icons["close_thin"]).setForeground(Color(ic, 0.7f, 0.7f))),
                                        }),
                                    },
                                    new Hidden(count)
                                ));
                            }
                            count++;
                            ic += 0.01f;
                        }
                    }
                    tv->setModel(model);
                group->append(tv, Fill::Both);
            box->append(group, Fill::Both);
        }

    return box;
}

int main(int argc, char **argv) {
    Application *app = Application::get();
        delete app->mainWidget();
        app->setMainWidget(new Box(Align::Vertical));
        app->onReady = [&](Window *window) {
            if (argc > 1) {
                if (std::string(argv[1]) == std::string("quit")) {
                    window->quit();
                }
            }
        };
        app->resize(800, 700);
        app->center();
        app->setTitle("Widget Gallery");
        Box *h_box = new Box(Align::Horizontal);
            auto light = new Button("Light");
                light->onMouseClick.addEventListener([&](Widget *widget, MouseEvent event) {
                    app->dc->default_style = app->dc->default_light_style;
                });
            h_box->append(light);
            auto dark = new Button("Dark");
                dark->onMouseClick.addEventListener([&](Widget *widget, MouseEvent event) {
                    app->dc->default_style = app->dc->default_dark_style;
                });
            h_box->append(dark);
        app->append(h_box);
        NoteBook *notebook = new NoteBook();
            notebook->appendTab(basic(*app), "Basic", nullptr, false);
            notebook->appendTab(slidersAndScrollbars(*app), "Sliders and ScrollBars", nullptr, false);
            notebook->appendTab(splitter(*app), "Splitter", nullptr, false);
            notebook->appendTab(treeView(*app), "TreeView", nullptr, false);
        app->append(notebook, Fill::Both);
    app->run();

    return 0;
}
