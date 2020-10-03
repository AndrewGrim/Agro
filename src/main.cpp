#include <iostream>
#include <vector>
#include <memory>

#include "util.hpp"
#include "app.hpp"
#include "controls/box.hpp"
#include "controls/button.hpp"

void onMouseDown(Widget *self, MouseEvent event) {
}

void onMouseUp(Widget *self, MouseEvent event) {
}

void onMouseLeft(Widget *self, MouseEvent event) {
}

void onMouseEntered(Widget *self, MouseEvent event) {
}

void onMouseMotion(Widget *self, MouseEvent event) {
}

void onApplicationReady(Application *self) {
    for (Widget *child : self->main_widget->children) {
        println(child->rect);
        for (Widget *child : child->children) {
            pprint(4, child->rect);
        }
    }
    // Root: Align::Vertical, Fill::Both
    // [0]: Align::Horizontal, Fill::Vertical
    assert(self->main_widget->children[0]->rect == Rect<float>(0, 0, 234, 169));
        assert(self->main_widget->children[0]->children[0]->rect == Rect<float>(0, 0, 122, 169));
        assert(self->main_widget->children[0]->children[1]->rect == Rect<float>(122, 0, 28, 169));
        assert(self->main_widget->children[0]->children[2]->rect == Rect<float>(150, 0, 28, 169));
        assert(self->main_widget->children[0]->children[3]->rect == Rect<float>(178, 0, 28, 169));
        assert(self->main_widget->children[0]->children[4]->rect == Rect<float>(206, 0, 28, 169));
    // [1]: Fill::Horizontal
    assert(self->main_widget->children[1]->rect == Rect<float>(0, 169, 400, 31));
    // [2]: Fill::Both
    assert(self->main_widget->children[2]->rect == Rect<float>(0, 200, 400, 169));
    // [3]: Fill::None
    assert(self->main_widget->children[3]->rect == Rect<float>(0, 369, 50, 31));
}

int main() { 
    Application *app = new Application();
        app->ready_callback = onApplicationReady;
        Box *inner = new Box(Align::Horizontal);
            Button *test_button = new Button("My first button !");
                test_button->mouse_down_callback = onMouseDown;
                test_button->mouse_up_callback = onMouseUp;
                test_button->mouse_left_callback = onMouseLeft;
                test_button->mouse_entered_callback = onMouseEntered;
                test_button->mouse_motion_callback = onMouseMotion;
            inner->append(test_button, Fill::Both);
            inner->append(new Button("2"), Fill::Both);
            inner->append(new Button("3"), Fill::Both);
            inner->append(new Button("4"), Fill::Both);
            inner->append(new Button("5"), Fill::Both);
        app->append(inner, Fill::Vertical);
        app->append(new Button("Alright, should be ready now."), Fill::Horizontal);
        app->append(new Button("Centered text for everyone"), Fill::Both);
        app->append(new Button("Test"), Fill::None);
    app->run();

    return 0; 
}
