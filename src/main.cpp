#include <iostream>
#include <vector>
#include <memory>
#include <ctime>

#include "util.hpp"
#include "app.hpp"
#include "option.hpp"
#include "controls/widget.hpp"
#include "controls/box.hpp"
#include "controls/button.hpp"
#include "controls/image.hpp"
#include "controls/label.hpp"
#include "controls/slider.hpp"
#include "controls/scrollbar.hpp"
#include "controls/scrolledbox.hpp"
#include "controls/lineedit.hpp"
#include "controls/notebook.hpp"

#include "custom_widget.hpp"

int main() { 
    Application *app = new Application("Application", Size(500, 500));
    Font *font = new Font("fonts/DejaVu/DejaVuSansMono.ttf", 18, Font::Type::Mono);
    app->onReady = [](Application *app) {
        println("READY");
    };
    app->onQuit = [&](Application *app) -> bool {
        println("QUIT");
        delete font;
        return true;
        // return false; // to not quit 
    };
    // for (int i = 0; i < 1000000; i++) {
    //     app->append(new Button(std::to_string(i)), Fill::Both);
    // }
    int map_key = app->bind(
        SDLK_q,
        Mod::None, 
        [&]{
            println("Q");
            app->quit();
        }
    );
    // app->unbind(map_key);
    app->bind(
        SDLK_q,
        Mod::Ctrl,
        [&]{
            println("CTRL+Q");
            app->quit();
        }
    );
    app->bind(
        SDLK_q,
        Mod::Ctrl|Mod::Shift|Mod::Alt,
        [&]{
            println("CTRL+Shift+Alt+Q");
            app->quit();
        }
    );
    app->bind(
        SDLK_q,
        Mod::Ctrl|Mod::Shift,
        [&]{
            println("CTRL+Shift+Q");
            app->quit();
        }
    );
    ((ScrolledBox*)app->mainWidget())->setAlignPolicy(Align::Horizontal);
    app->mainWidget()->setBackground(Color(0.5, 0.2, 0.4, 0.8));
        Color colors[] = {
            Color(1, 0.19, 0.19), Color(1, 0.6), Color(1, 1),
            Color(0.19, 0.19, 1), Color(0, 0.5, 1), Color(0, 1, 1),
        };
        int widget_count = 1;
        int color_index = 0;
        Box *left = new Box(Align::Vertical);
            NoteBook *nb = new NoteBook();
                Box *_b = new Box(Align::Vertical);
                    // _b->setBackground(Color(0, 0, 0, 0));
                    _b->setBackground(Color(1, 0, 1, 1));
                    LineEdit *ln = new LineEdit("Fill::None");
                        // ln->setMinLength(80);
                        ln->bind(SDLK_a, Mod::Ctrl, []{
                            println("Ctrl+A");
                        });
                    _b->append(ln, Fill::None);
                    LineEdit *lb = new LineEdit("Fill::Both Fill::Both Fill::Both Fill::Both Fill::Both Fill::Both");
                        lb->setPlaceholderText("Nothing to see here!");
                        // lb->bind(SDLK_SPACE, Mod::None, [&]{
                        //     lb->clear();
                        // });
                    _b->append(lb, Fill::Both);
                nb->appendTab(_b, "Tab 1");
            left->append(nb, Fill::Both);
            {
                ScrolledBox *sb = new ScrolledBox(Align::Vertical, Size(200, 200));
                    sb->setBackground(Color(0.8, 0.5, 0.5));
                    for (int i = 0; i < 100; i++) {
                        Label *_l = new Label(std::string("This is some long ass text ") + std::to_string(i));
                        sb->append(_l);
                    }
                left->append(sb, Fill::Both);
            }
            {
                ScrolledBox *sb = new ScrolledBox(Align::Vertical, Size(200, 200));
                    sb->setBackground(Color(0.5, 0.5, 0.8));
                    ScrolledBox *inner_sb = new ScrolledBox(Align::Vertical, Size(150, 150));
                        inner_sb->setBackground(Color(0.8, 0.8, 0.5));
                        for (int i = 0; i < 100; i++) {
                            Label *_l2 = new Label(std::string("This is some long ass text ") + std::to_string(i));
                            inner_sb->append(_l2);
                        }
                    sb->append(inner_sb, Fill::Both);
                    Box *inner_b = new Box(Align::Vertical);
                        inner_b->setBackground(Color(1.0, 0.8, 0.4));
                        for (int i = 0; i < 100; i++) {
                            Label *_l = new Label(std::string("This is some long ass text ") + std::to_string(i));
                            inner_b->append(_l);
                        }
                    sb->append(inner_b, Fill::Both);
                left->append(sb, Fill::Both);
            }
        app->append(left, Fill::Both, 10);
        Box *middle = new Box(Align::Vertical);
            middle->append(new CustomWidget());
            Label *l = new Label("0.000000");
                l->setHorizontalAlignment(HorizontalAlignment::Center);
            middle->append(l, Fill::Horizontal);
            Slider *s = new Slider(Align::Horizontal);
                s->onValueChanged = [=]() {
                    l->setText(std::to_string(s->m_value));
                };
            middle->append(s, Fill::Horizontal);
            Box *b = new Box(Align::Horizontal);
                b->setBackground(Color(0.6, 1.0, 0.6));
                {
                    Label *l = new Label("0.000000");
                        l->setHorizontalAlignment(HorizontalAlignment::Center);
                    b->append(l, Fill::Both);
                    Slider *s = new Slider(Align::Vertical);
                        s->setBackground(Color(0.6, 0.0, 0.6));
                        s->setForeground(Color(1, 0, 1));
                        s->onValueChanged = [=]() {
                            l->setText(std::to_string(s->m_value));
                        };
                    b->append(s, Fill::Vertical);
                }
            middle->append(b, Fill::Both);
            ScrollBar *sbv = new ScrollBar(Align::Vertical);
            middle->append(sbv, Fill::Vertical);
            ScrollBar *sbh = new ScrollBar(Align::Horizontal);
            middle->append(sbh, Fill::Horizontal);
        app->append(middle, Fill::Both);
        Box *right = new Box(Align::Vertical);
            Button *add = new Button("Add Button");
                add->onMouseDown = [&](MouseEvent event) {
                    if (widget_count % 2 == 0) {
                        Image *i = new Image("Teostra.png");
                            i->setBackground(Color(1, 1, 1, 0.8));
                            i->onMouseClick = [=](MouseEvent event) {
                                if (i->parent == left) {
                                    i->setExpand(true);
                                    // i->setMaintainAspectRatio(false);
                                    middle->append(i, Fill::Both);
                                } else {
                                    i->setExpand(false);
                                    left->append(i, Fill::Both);
                                }
                            };
                        left->append(i, Fill::Both);
                    } else if (widget_count % 3 == 0) {
                        Label *l = new Label("Label " + std::to_string(widget_count));
                            l->setBackground(Color(0, 0, 0, 0.3));
                            l->onMouseClick = [=](MouseEvent event) {
                                if (l->parent == left) {
                                    middle->append(l, Fill::Both);
                                } else {
                                    left->append(l, Fill::Both);
                                }
                            };
                        left->append(l, Fill::Both);
                    } else {
                        Button *b = new Button(std::to_string(widget_count));
                            b->setBackground(colors[color_index]);
                            b->setImage(new Image("notes.png"));
                            b->onMouseClick = [=](MouseEvent event) {
                                time_t t = std::time(nullptr);
                                b->setText(std::asctime(std::localtime(&t)));
                                if (b->parent == left) {
                                    middle->append(b, Fill::Both);
                                    b->setFont(font);
                                } else {
                                    left->append(b, Fill::Both);
                                    b->setFont(nullptr);
                                }
                            };
                        left->append(b, Fill::Both);
                    }
                    widget_count++;
                    color_index++;
                    if (color_index == 6) {
                        color_index = 0;
                    }
                };
            right->append(add, Fill::Both);
            Button *remove = new Button("Remove");
                remove->onMouseClick = [&](MouseEvent event) {
                    if (left->children.size() > 0) {
                        left->remove(left->children.size() - 1);
                    }
                };
            right->append(remove, Fill::Both);
        app->append(right, Fill::Both);
    app->run();

    return 0; 
}
