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
    auto sizer = new Box(app, Align::Vertical);
        Box *inner = new Box(app, Align::Horizontal);
            Button *test_button = new Button(app, "My first button !");
                // test_button->mouse_down_callback = onMouseDown;
                // test_button->mouse_up_callback = onMouseUp;
                // test_button->mouse_left_callback = onMouseLeft;
                // test_button->mouse_entered_callback = onMouseEntered;
                // test_button->mouse_motion_callback = onMouseMotion;
            inner->append(test_button, Fill::Both);
            inner->append(new Button(app, "2"), Fill::Both);
            inner->append(new Button(app, "3"), Fill::Both);
            inner->append(new Button(app, "4"), Fill::Both);
            inner->append(new Button(app, "5"), Fill::Both);
        sizer->append(inner, Fill::Vertical);
        sizer->append(new Button(app, "Alright, should be ready now."), Fill::Horizontal);
        sizer->append(new Button(app, "Centered text for everyone"), Fill::Both);
        sizer->append(new Button(app, "Test"), Fill::None);

    app->set_main_widget(sizer);
    app->run();

    return 0; 
}
