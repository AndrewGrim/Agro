#include <iostream>
#include <vector>
#include <memory>

#include "../src/util.hpp"
#include "../src/app.hpp"
#include "../src/controls/box.hpp"
#include "../src/controls/button.hpp"

void onApplicationReady(Application *self) {
    for (Widget *child : self->main_widget->children) {
        println(child->rect);
        for (Widget *child : child->children) {
            pprint(4, child->rect);
            for (Widget *child : child->children) {
                pprint(8, child->rect);
            }
        }
    }
    // Root: Align::Vertical, Fill::Both
    // [0]: Align::Vertical, Fill::None
    assert(self->main_widget->children[0]->rect == Rect<float>(0, 0, 213, 124));
        assert(self->main_widget->children[0]->children[0]->rect == Rect<float>(0, 0, 181, 31));
        assert(self->main_widget->children[0]->children[1]->rect == Rect<float>(0, 31, 213, 31));
        assert(self->main_widget->children[0]->children[2]->rect == Rect<float>(0, 62, 195, 31));
        assert(self->main_widget->children[0]->children[3]->rect == Rect<float>(0, 93, 213, 31));

    // [1]: Align::Vertical, Fill::Horizontal
    assert(self->main_widget->children[1]->rect == Rect<float>(0, 124, 400, 124));
        assert(self->main_widget->children[1]->children[0]->rect == Rect<float>(0, 124, 213, 31));
        assert(self->main_widget->children[1]->children[1]->rect == Rect<float>(0, 155, 400, 31));
        assert(self->main_widget->children[1]->children[2]->rect == Rect<float>(0, 186, 227, 31));
        assert(self->main_widget->children[1]->children[3]->rect == Rect<float>(0, 217, 400, 31));

    // [2]: Align::Vertical, Fill::Vertical
    assert(self->main_widget->children[2]->rect == Rect<float>(0, 248, 227, 124));
        assert(self->main_widget->children[2]->children[0]->rect == Rect<float>(0, 248, 195, 31));
        assert(self->main_widget->children[2]->children[1]->rect == Rect<float>(0, 279, 227, 31));
        assert(self->main_widget->children[2]->children[2]->rect == Rect<float>(0, 310, 209, 31));
        assert(self->main_widget->children[2]->children[3]->rect == Rect<float>(0, 341, 227, 31));

    // [3]: Align::Vertical, Fill::Both
    assert(self->main_widget->children[3]->rect == Rect<float>(0, 372, 400, 124));
        assert(self->main_widget->children[3]->children[0]->rect == Rect<float>(0, 372, 177, 31));
        assert(self->main_widget->children[3]->children[1]->rect == Rect<float>(0, 403, 400, 31));
        assert(self->main_widget->children[3]->children[2]->rect == Rect<float>(0, 434, 191, 31));
        assert(self->main_widget->children[3]->children[3]->rect == Rect<float>(0, 465, 400, 31));
}

int main() { 
    Application *app = new Application();
        app->ready_callback = onApplicationReady;

        // [0]: Align::Vertical, Fill::None
        Box *av_fn = new Box(Align::Vertical);
            av_fn->append(new Button("P: Fill::None, C: Fill::None"), Fill::None);
            av_fn->append(new Button("P: Fill::None, C: Fill::Horizontal"), Fill::Horizontal);
            av_fn->append(new Button("P: Fill::None, C: Fill::Vertical"), Fill::Vertical);
            av_fn->append(new Button("P: Fill::None, C: Fill::Both"), Fill::Both);
        app->append(av_fn, Fill::None);

        // [1]: Align::Vertical, Fill::Horizontal
        Box *av_fh = new Box(Align::Vertical);
            av_fh->append(new Button("P: Fill::Horizontal, C: Fill::None"), Fill::None);
            av_fh->append(new Button("P: Fill::Horizontal, C: Fill::Horizontal"), Fill::Horizontal);
            av_fh->append(new Button("P: Fill::Horizontal, C: Fill::Vertical"), Fill::Vertical);
            av_fh->append(new Button("P: Fill::Horizontal, C: Fill::Both"), Fill::Both);
        app->append(av_fh, Fill::Horizontal);

        // [2]: Align::Vertical, Fill::Vertical
        Box *av_fv = new Box(Align::Vertical);
            av_fv->append(new Button("P: Fill::Vertical, C: Fill::None"), Fill::None);
            av_fv->append(new Button("P: Fill::Vertical, C: Fill::Horizontal"), Fill::Horizontal);
            av_fv->append(new Button("P: Fill::Vertical, C: Fill::Vertical"), Fill::Vertical);
            av_fv->append(new Button("P: Fill::Vertical, C: Fill::Both"), Fill::Both);
        app->append(av_fv, Fill::Vertical);

        // [3]: Align::Vertical, Fill::Both
        Box *av_fb = new Box(Align::Vertical);
            av_fb->append(new Button("P: Fill::Both, C: Fill::None"), Fill::None);
            av_fb->append(new Button("P: Fill::Both, C: Fill::Horizontal"), Fill::Horizontal);
            av_fb->append(new Button("P: Fill::Both, C: Fill::Vertical"), Fill::Vertical);
            av_fb->append(new Button("P: Fill::Both, C: Fill::Both"), Fill::Both);
        app->append(av_fb, Fill::Both);

    app->run();

    return 0; 
}
