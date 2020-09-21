#include <iostream>
#include <vector>
#include <memory>

#include "util.hpp"
#include "app.hpp"
#include "controls/box.hpp"
#include "controls/button.hpp"

// void onMouseDown(Widget *self, MouseEvent event) {
//     // self->set_background(Color { 255, 0, 0, 255 });
// }

// void onMouseUp(Widget *self, MouseEvent event) {
//     // self->set_background(Color { 0, 255, 0, 255 });
// }

// void onMouseLeft(Widget *self, MouseEvent event) {
//     // self->set_background(Color { 50, 0, 0, 255 });
// }

// void onMouseEntered(Widget *self, MouseEvent event) {
//     // self->set_background(Color { 0, 0, 255, 255 });
// }

// void onMouseMotion(Widget *self, MouseEvent event) {
// }

int main() { 
    auto app = new Application();
    auto sizer = new Box(app, Align::Vertical);
        Box *inner = new Box(app, Align::Horizontal);
            Button *test_button = (new Button(app, "My first button !"))->set_background(Color {50, 0, 0, 255});
                // test_button->mouse_down_callback = onMouseDown;
                // test_button->mouse_up_callback = onMouseUp;
                // test_button->mouse_left_callback = onMouseLeft;
                // test_button->mouse_entered_callback = onMouseEntered;
                // test_button->mouse_motion_callback = onMouseMotion;
            inner->append(test_button, Fill::Both);
            inner->append((new Button(app, "2"))->set_background(Color {100, 0, 0, 255}), Fill::Both);
            inner->append((new Button(app, "3"))->set_background(Color {150, 0, 0, 255}), Fill::Both);
            inner->append((new Button(app, "4"))->set_background(Color {200, 0, 0, 255}), Fill::Both);
            inner->append((new Button(app, "5"))->set_background(Color {255, 0, 0, 255}), Fill::Both);
        sizer->append(inner, Fill::Vertical);
        sizer->append((new Button(app, "Alright, should be ready now."))->set_background(Color {0, 255, 0, 255}), Fill::Horizontal);
        sizer->append((new Button(app, "Centered text for everyone"))->set_background(Color {0, 0, 255, 255}), Fill::Both);
        sizer->append((new Button(app, "Test"))->set_background(Color {255, 0, 255, 255}), Fill::None);

    app->set_main_widget(sizer);
	// app->show();
    app->run();

    return 0; 
}
