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
}

int main() { 
    Application *app = new Application();
        app->ready_callback = onApplicationReady;
        Box *sizer = new Box(Align::Horizontal);
            Button *test_button = new Button("My first button !");
                test_button->mouse_down_callback = onMouseDown;
                test_button->mouse_up_callback = onMouseUp;
                test_button->mouse_left_callback = onMouseLeft;
                test_button->mouse_entered_callback = onMouseEntered;
                test_button->mouse_motion_callback = onMouseMotion;
            sizer->append(test_button, Fill::Both);
            sizer->append(new Button("2"), Fill::Both);
            sizer->append(new Button("3"), Fill::Both);
            sizer->append(new Button("4"), Fill::Both);
            sizer->append(new Button("5"), Fill::Both);
        app->append(sizer, Fill::Vertical);
    app->run();

    return 0; 
}
