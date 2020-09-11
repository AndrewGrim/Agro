#include <iostream>
#include <vector>
#include <memory>

#include "util.hpp"
#include "app.hpp"
#include "controls/box.hpp"
#include "controls/button.hpp"

void onMouseDown(Widget *self, MouseEvent event) {
    self->set_background(Color { 255, 255, 255, 255 });
}

void onMouseUp(Widget *self, MouseEvent event) {
    self->set_background(Color { 50, 0, 0, 255 });
}

int main() { 
    auto app = Application();
    auto sizer = new Box(Align::Vertical);
        Box *inner = new Box(Align::Horizontal);
            Button *test_button = (new Button())->set_background(Color {50, 0, 0, 255});
                test_button->mouse_down_callback = &onMouseDown;
                test_button->mouse_up_callback = &onMouseUp;
            inner->append(test_button, Fill::Both);
            inner->append((new Button())->set_background(Color {100, 0, 0, 255}), Fill::Both);
            inner->append((new Button())->set_background(Color {150, 0, 0, 255}), Fill::Both);
            inner->append((new Button())->set_background(Color {200, 0, 0, 255}), Fill::Both);
            inner->append((new Button())->set_background(Color {255, 0, 0, 255}), Fill::Both);
        sizer->append(inner, Fill::Vertical);
        sizer->append((new Button())->set_background(Color {0, 255, 0, 255}), Fill::Horizontal);
        sizer->append((new Button())->set_background(Color {0, 0, 255, 255}), Fill::Both);
        sizer->append((new Button())->set_background(Color {255, 0, 255, 255}), Fill::None);

    app.set_main_widget(sizer);
	app.show();
    app.run();

    return 0; 
}
