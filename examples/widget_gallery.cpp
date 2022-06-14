#include "../src/util.hpp"
#include "../src/application.hpp"
#include "../src/controls/box.hpp"
#include "../src/controls/group_box.hpp"
#include "../src/controls/note_book.hpp"
#include "../src/controls/label.hpp"
#include "../src/controls/tooltip.hpp"
#include "../src/controls/button.hpp"
#include "../src/controls/text_edit.hpp"
#include "../src/controls/spin_box.hpp"
#include "../src/controls/drop_down.hpp"
#include "../src/controls/splitter.hpp"
#include "../src/controls/tree_view.hpp"
#include "../src/controls/color_picker.hpp"
#include "../src/controls/check_button.hpp"
#include "../src/controls/radio_button.hpp"
#include "../src/controls/progress_bar.hpp"
#include "../src/controls/style_editor.hpp"
#include "resources.hpp"

u32 timerCallback(u32 interval, void *data) {
    Application *app = (Application*)data;
    ProgressBar *bar = (ProgressBar*)(app->mainWindow()->mainWidget()->children[0]->children[0]->children[3]->children[0]);
    f64 value = bar->m_value;
    if (value < 1.0) {
        bar->m_value += 0.01;
    } else {
        bar->m_value = 0.0;
    }
    app->mainWindow()->update();
    app->mainWindow()->pulse();

    return 50;
}

String loadFileToString(const char *path) {
    FILE *file = fopen(path, "rb");
    char byte;
    u64 index = 0;
    u64 size = 0;
    while (fgetc(file) != EOF) {
        size++;
    }
    fseek(file, 0, 0);
    String s = String(size);
    while ((byte = fgetc(file)) != EOF) {
        s.data()[index] = (u8)byte;
        index++;
    }
    s.data()[index] = '\0';
    return s;
}

Widget* basic1(Application &app) {
    Box *box = new Box(Align::Vertical);
        Box *labels_and_buttons = new Box(Align::Horizontal);
            GroupBox *labels = new GroupBox(Align::Vertical, "Labels");
                Label *lr = new Label("Right");
                    lr->setHorizontalAlignment(HorizontalAlignment::Right);
                Label *lc = new Label("This text\nspans\nmultiple lines,\nand is center aligned.\nÓreiða 😁 の ®");
                    lc->setHorizontalAlignment(HorizontalAlignment::Center);
                    GroupBox *l_menu = new GroupBox(Align::Vertical, "");
                        l_menu->append(new TextEdit("", "Context Menu", TextEdit::Mode::SingleLine, Size(100, 100)), Fill::Horizontal);
                    lc->context_menu = l_menu;
                labels->append(new Label("Left"), Fill::Both);
                labels->append(lr, Fill::Both);
                labels->append(lc, Fill::Both);
            labels_and_buttons->append(labels, Fill::Both);

            GroupBox *buttons = new GroupBox(Align::Vertical, "Buttons");
                buttons->append(new Button("Button"));
                buttons->append(new Button(new Image(notes_png, notes_png_length)));
                Button *image_and_text = new Button(new Image(notes_png, notes_png_length));
                    image_and_text->setText("Button");
                    image_and_text->tooltip = new Tooltip("Tooltip");
                    GroupBox *b_menu = new GroupBox(Align::Vertical, "");
                        Button *mb = new Button("Menu Button");
                            mb->onMouseClick.addEventListener([](Widget *button, MouseEvent event) {
                                println("context menu event");
                            });
                            mb->tooltip = (new Image(lena_png, lena_png_length))->setMinSize(Size(56, 56));
                        b_menu->append(mb, Fill::Horizontal);
                        b_menu->append(new CheckButton("Unchecked"), Fill::Horizontal);
                    image_and_text->context_menu = b_menu;
                buttons->append(image_and_text, Fill::Both);
            labels_and_buttons->append(buttons, Fill::Both);
        box->append(labels_and_buttons, Fill::Horizontal);

        Box *check_and_radio = new Box(Align::Horizontal);
            GroupBox *checks = new GroupBox(Align::Vertical, "CheckButtons");
                checks->append(new CheckButton("Unchecked", false));
                checks->append(new CheckButton("Checked", true));
                checks->append(new CheckButton("Checked, Expandable", true), Fill::Both);
            check_and_radio->append(checks, Fill::Both);

            GroupBox *radios = new GroupBox(Align::Vertical, "RadioButtons");
                std::shared_ptr<RadioGroup> radio_group = std::make_shared<RadioGroup>();
                radios->append(new RadioButton(radio_group, "Radio 1"));
                radio_group->buttons[0]->m_is_checked = true;
                radios->append(new RadioButton(radio_group, "Radio 2"));
                radios->append(new RadioButton(radio_group, "Radio 3, Expandable"), Fill::Both);
            check_and_radio->append(radios, Fill::Both);
        box->append(check_and_radio, Fill::Horizontal);

        GroupBox *text_edits = new GroupBox(Align::Vertical, "TextEdits");
            text_edits->append(new TextEdit("", "Placeholder text", TextEdit::Mode::SingleLine, Size(200, 100)));
            text_edits->append(new TextEdit("Default text", "", TextEdit::Mode::SingleLine, Size(300, 100)));
            text_edits->append(new TextEdit(String((const char*)utf8_test_txt, utf8_test_txt_length), "", TextEdit::Mode::MultiLine), Fill::Both);
        box->append(text_edits, Fill::Both);

        GroupBox *progress = new GroupBox(Align::Vertical, "ProgressBars");
            progress->append(new ProgressBar(200));
            progress->append(new ProgressBar(300));
            ((ProgressBar*)progress->children[1])->m_value = 0.50;
            progress->append(new ProgressBar(), Fill::Horizontal);
            ((ProgressBar*)progress->children[2])->m_value = 1.0;
        box->append(progress, Fill::Horizontal);

    return box;
}

Widget* basic2(Application &app) {
    Box *box = new Box(Align::Vertical);
        Box *images_and_color = new Box(Align::Horizontal);
            GroupBox *images = new GroupBox(Align::Horizontal, "Images");
                auto lena = std::make_shared<Texture>(lena_png, lena_png_length);
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

        GroupBox *spin_boxes = new GroupBox(Align::Vertical, "SpinBoxes");
            spin_boxes->append(new SpinBox(0, 150));
            spin_boxes->append(new SpinBox(-100, 250));
            spin_boxes->append(new SpinBox(100, 150), Fill::Horizontal);
        box->append(spin_boxes, Fill::Both);

        Box *dropdowns_and_list = new Box(Align::Horizontal);
            GroupBox *dropdown_boxes = new GroupBox(Align::Vertical, "DropDowns");
                DropDown *dropdown_cellrenderers = new DropDown();
                    dropdown_cellrenderers->appendItem(new TextCellRenderer("Testing 0"));
                    dropdown_cellrenderers->appendItem(new TextCellRenderer("Testing 1"));
                    dropdown_cellrenderers->appendItem(new TextCellRenderer("Testing 2"));
                    dropdown_cellrenderers->appendItem(new TextCellRenderer("Testing 3"));
                    dropdown_cellrenderers->appendItem(new TextCellRenderer("Testing 4"));
                    dropdown_cellrenderers->appendItem(new TextCellRenderer("Testing 5"));
                    dropdown_cellrenderers->appendItem(new TextCellRenderer("Testing 6"));
                    dropdown_cellrenderers->appendItem(new TextCellRenderer("Testing 7"));
                    dropdown_cellrenderers->appendItem(new TextCellRenderer("Testing 8"));
                    dropdown_cellrenderers->appendItem(new TextCellRenderer("Testing 9"));
                    dropdown_cellrenderers->appendItem(new TextCellRenderer("Testing 10"));
                    dropdown_cellrenderers->onItemSelected.addEventListener([](Widget *widget, CellRenderer *item, i32 index) {
                        println(widget);
                        println(item);
                        println(((TextCellRenderer*)item)->text.data());
                        println(index);
                    });
                    dropdown_cellrenderers->setCurrent(0);
                dropdown_boxes->append(dropdown_cellrenderers);
                DropDown *dropdown_widgets = new DropDown();
                    dropdown_widgets->appendItem(new ImageCellRenderer((new Image(lena))->setMinSize(Size(56, 56))));
                    dropdown_widgets->appendItem(new ImageCellRenderer((new Image(lena))->setMinSize(Size(56, 56))));
                    dropdown_widgets->appendItem(new ImageCellRenderer((new Image(lena))->setMinSize(Size(56, 56))));
                    dropdown_widgets->setCurrent(2);
                dropdown_boxes->append(dropdown_widgets, Fill::Vertical);
                DropDown *dropdown_any_drawable = new DropDown();
                    dropdown_any_drawable->appendItem(new TextCellRenderer("TextCellRender"));
                    dropdown_any_drawable->appendItem(new ImageTextCellRenderer((new Image(lena))->setMinSize(Size(24, 24)), "ImageTextCellRenderer"));
                    dropdown_any_drawable->appendItem(new TextCellRenderer("TextCellRender"));
                    dropdown_any_drawable->appendItem(new ImageTextCellRenderer((new Image(lena))->setMinSize(Size(24, 24)), "ImageTextCellRenderer"));
                    dropdown_any_drawable->appendItem(new TextCellRenderer("TextCellRender"));
                    dropdown_any_drawable->appendItem(new ImageTextCellRenderer((new Image(lena))->setMinSize(Size(24, 24)), "ImageTextCellRenderer"));
                    dropdown_any_drawable->appendItem(new TextCellRenderer("TextCellRender"));
                    dropdown_any_drawable->appendItem(new ImageTextCellRenderer((new Image(lena))->setMinSize(Size(24, 24)), "ImageTextCellRenderer"));
                    dropdown_any_drawable->appendItem(new TextCellRenderer("TextCellRender"));
                    dropdown_any_drawable->appendItem(new ImageTextCellRenderer((new Image(lena))->setMinSize(Size(24, 24)), "ImageTextCellRenderer"));
                    dropdown_any_drawable->setCurrent(5);
            dropdown_boxes->append(dropdown_any_drawable, Fill::Horizontal);
        dropdowns_and_list->append(dropdown_boxes, Fill::Both);
            GroupBox *lists = new GroupBox(Align::Vertical, "Lists");
            List *list = new List();
                for (i32 i = 0; i < 50; i++) {
                    list->appendItem(new TextCellRenderer("TextCellRender"));
                }
            lists->append(list, Fill::Both);
        dropdowns_and_list->append(lists, Fill::Both);
    box->append(dropdowns_and_list, Fill::Both);

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
    Box *box = new Box(Align::Vertical);
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
    i32 id = -1;
    Hidden(i32 id) : id{id} {}
};

Widget* treeView(Application &app) {
    ScrolledBox *box = new ScrolledBox(Align::Horizontal);
        {
            GroupBox *group = new GroupBox(Align::Vertical, "Mode::Unroll");
                TreeView<Hidden> *tv = new TreeView<Hidden>();
                    tv->hideColumnHeaders();
                    for (i32 i = 1; i < 4; i++) {
                        auto c = new Column<Hidden>(
                            "Column " + toString(i), nullptr, HorizontalAlignment::Center,
                            [](TreeNode<Hidden> *lhs, TreeNode<Hidden> *rhs) {
                                return lhs->hidden->id > rhs->hidden->id;
                            }
                        );
                        c->setExpand(true);
                        tv->append(c);
                    }
                    Tree<Hidden> *model = new Tree<Hidden>();
                    for (i32 i = 1; i < 101; i++) {
                        model->append(nullptr, new TreeNode<Hidden>(
                            {
                                new TextCellRenderer("Row " + toString(i)),
                                new Button("TreeView"),
                                new TextEdit("", "TreeView"),
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
                    for (i32 i = 1; i < 4; i++) {
                        auto c = new Column<Hidden>(
                            String("Column ") + toString(i), nullptr, HorizontalAlignment::Center,
                            [](TreeNode<Hidden> *lhs, TreeNode<Hidden> *rhs) {
                                return lhs->hidden->id > rhs->hidden->id;
                            }
                        );
                        tv->append(c);
                        if (i == 1) { c->setExpand(true); }
                    }
                    Tree<Hidden> *model = new Tree<Hidden>();
                    i32 count = 0;
                    f32 ic = 0.0f;
                    for (i32 i = 1; i < 6; i++) {
                        TreeNode<Hidden> *parent = nullptr;
                        for (i32 j = 1; j < 21; j++) {
                            auto n = model->append(parent, new TreeNode<Hidden>(
                                {
                                    new ImageTextCellRenderer((new Image(app.icons["up_arrow"]))->setForeground(Color(0.7f, 0.7f, ic)), "Row " + toString(count)),
                                    new ImageCellRenderer((new Image(app.icons["up_arrow"]))->flipVertically()->setForeground(Color(0.7f, ic, 0.7f))),
                                    new MultipleImagesCellRenderer({
                                        *(Image(app.icons["close"]).setForeground(Color(ic, 0.7f, 0.7f))),
                                        *(Image(app.icons["up_arrow"]).setForeground(Color(ic, 0.7f, 0.7f))),
                                        *(Image(app.icons["close_thin"]).setForeground(Color(ic, 0.7f, 0.7f))),
                                    }),
                                },
                                new Hidden(count)
                            ));
                            if (j == 1) {
                                parent = n;
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
        delete app->mainWindow()->mainWidget();
        app->mainWindow()->setMainWidget(new Box(Align::Vertical));
        app->mainWindow()->onReady = [&](Window *window) {
            if (argc > 1) {
                if (String(argv[1]) == "quit") {
                    window->quit();
                }
            }
        };
        app->mainWindow()->resize(800, 800);
        app->mainWindow()->center();
        app->mainWindow()->setTitle("Widget Gallery");
        NoteBook *notebook = new NoteBook();
            notebook->appendTab(basic1(*app), "Basic 1", nullptr, false);
            notebook->appendTab(basic2(*app), "Basic 2", nullptr, false);
            notebook->appendTab(slidersAndScrollbars(*app), "Sliders and ScrollBars", nullptr, false);
            notebook->appendTab(splitter(*app), "Splitter", nullptr, false);
            notebook->appendTab(treeView(*app), "TreeView", nullptr, false);
            // notebook->setCurrentTab(1);
        app->mainWindow()->append(notebook, Fill::Both);
        // app->addTimer(
        //     100,
        //     [=](u32 interval) -> u32 {
        //         ProgressBar *bar = (ProgressBar*)(app->mainWindow()->mainWidget()->children[0]->children[0]->children[3]->children[0]);
        //         f64 value = bar->m_value;
        //         if (value < 1.0) {
        //             bar->m_value += 0.01;
        //         } else {
        //             bar->m_value = 0.0;
        //         }
        //         app->mainWindow()->update();
        //         app->mainWindow()->pulse();

        //         return 50;
        //     }
        // );
        // StyleEditor::asWindow("Style Editor", Size(600, 600))->run();
    app->run();

    return 0;
}
