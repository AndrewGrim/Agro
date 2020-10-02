#include <iostream>
#include <vector>
#include <memory>

#include "util.hpp"
#include "app.hpp"
#include "controls/box.hpp"
#include "controls/button.hpp"

// void onMouseDown(Widget *self, MouseEvent event) {
// }

// void onMouseUp(Widget *self, MouseEvent event) {
// }

// void onMouseLeft(Widget *self, MouseEvent event) {
// }

// void onMouseEntered(Widget *self, MouseEvent event) {
// }

// void onMouseMotion(Widget *self, MouseEvent event) {
// }

int main() { 
    auto app = new Application();
        Box *inner = new Box(Align::Horizontal);
            Button *test_button = new Button("My first button !");
                // test_button->mouse_down_callback = onMouseDown;
                // test_button->mouse_up_callback = onMouseUp;
                // test_button->mouse_left_callback = onMouseLeft;
                // test_button->mouse_entered_callback = onMouseEntered;
                // test_button->mouse_motion_callback = onMouseMotion;
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
