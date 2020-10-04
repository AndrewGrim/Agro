#include <iostream>
#include <vector>
#include <memory>

#include "util.hpp"
#include "app.hpp"
#include "controls/box.hpp"
#include "controls/button.hpp"
#include "controls/slider.hpp"

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

void onValueChanged(Slider *self) {
    println(self->m_value);
}

int main() { 
    Application *app = new Application();
        app->ready_callback = onApplicationReady;
        Box *top_sizer = new Box(Align::Horizontal);
            Button *test_button = new Button("T Really long button so we can test scrolling!");
                test_button->mouse_down_callback = onMouseDown;
                test_button->mouse_up_callback = onMouseUp;
                test_button->mouse_left_callback = onMouseLeft;
                test_button->mouse_entered_callback = onMouseEntered;
                test_button->mouse_motion_callback = onMouseMotion;
            Slider *test_slider = new Slider(Align::Vertical, "Slider");
                test_slider->value_changed_callback = onValueChanged;
            top_sizer->append(test_slider, Fill::Vertical);
            top_sizer->append(test_button, Fill::Both);
            top_sizer->append(new Button("T 2"), Fill::Both);
            top_sizer->append(new Button("T 3"), Fill::Both);
            top_sizer->append(new Button("T 4"), Fill::Both);
            top_sizer->append(new Button("T 5"), Fill::Both);
        app->append(top_sizer, Fill::Both);
        Box *bottom_sizer = new Box(Align::Vertical);
            bottom_sizer->append(new Button("B 1"), Fill::Both);
            bottom_sizer->append(new Button("B 2"), Fill::Both);
            bottom_sizer->append(new Button("B 3"), Fill::Both);
            bottom_sizer->append(new Button("B 4"), Fill::Both);
            bottom_sizer->append(new Button("B 5"), Fill::Both);
        app->append(bottom_sizer, Fill::Both);
    app->run();

    return 0; 
}
