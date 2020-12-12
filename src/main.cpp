#include <iostream>
#include <vector>
#include <memory>
#include <ctime>

#include "util.hpp"
#include "app.hpp"
#include "controls/widget.hpp"
#include "controls/box.hpp"
#include "controls/button.hpp"
#include "option.hpp"
// #include "controls/slider.hpp"
// #include "controls/scrollbar.hpp"
// #include "controls/scrolledbox.hpp"
// #include "controls/lineedit.hpp"

int main() { 
    Application *app = new Application("Application", Size(500, 500));
    app->onReady = [](Application *app) {
        println("READY");
    };
    app->onQuit = [](Application *app) -> bool {
        println("QUIT");
        return true;
        // return false; // to not quit 
    };
    app->setMainWidget(new Box(Align::Horizontal));
    app->mainWidget()->setBackground(Color(0.5, 0.2, 0.4, 0.8));
            Color colors[] = {
                Color(1, 0.19, 0.19), Color(1, 0.6), Color(1, 1),
                Color(0.19, 0.19, 1), Color(0, 0.5, 1), Color(0, 1, 1),
            };
            int button_count = 1;
            int color_index = 0;
            Box *left = new Box(Align::Vertical);
            app->append(left, Fill::Both, 10);
            Box *middle = new Box(Align::Vertical);
            app->append(middle, Fill::Both, 10);
            Box *right = new Box(Align::Vertical);
                Button *add = new Button("Add Button");
                    add->onMouseDown = [&](MouseEvent event) {
                        Button *b = new Button(std::to_string(button_count));
                            b->setBackground(colors[color_index]);
                            b->onMouseClick = [=](MouseEvent event) {
                                time_t t = std::time(nullptr);
                                b->setText(std::asctime(std::localtime(&t)));
                                if (b->parent == left) {
                                    middle->append(b, Fill::Both);
                                } else {
                                    left->append(b, Fill::Both);
                                }
                            };
                        button_count++;
                        color_index++;
                        if (color_index == 6) {
                            color_index = 0;
                        }
                        left->append(b, Fill::Both);
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
