#include <iostream>
#include <vector>
#include <memory>
#include <ctime>

#include "util.hpp"
#include "app.hpp"
#include "controls/widget.hpp"
#include "controls/box.hpp"
#include "controls/button.hpp"
#include "controls/image.hpp"
#include "controls/label.hpp"
#include "option.hpp"
// #include "controls/slider.hpp"
// #include "controls/scrollbar.hpp"
// #include "controls/scrolledbox.hpp"
// #include "controls/lineedit.hpp"

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
    app->setMainWidget(new Box(Align::Horizontal));
    app->mainWidget()->setBackground(Color(0.5, 0.2, 0.4, 0.8));
            Color colors[] = {
                Color(1, 0.19, 0.19), Color(1, 0.6), Color(1, 1),
                Color(0.19, 0.19, 1), Color(0, 0.5, 1), Color(0, 1, 1),
            };
            int widget_count = 1;
            int color_index = 0;
            Box *left = new Box(Align::Vertical);
            app->append(left, Fill::Both, 10);
            Box *middle = new Box(Align::Vertical);
                middle->append(new CustomWidget());
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
