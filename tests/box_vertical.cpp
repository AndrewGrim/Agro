#include <iostream>
#include <vector>
#include <memory>

#include "../src/util.hpp"
#include "../src/app.hpp"
#include "../src/controls/box.hpp"
#include "../src/controls/button.hpp"
#include "../src/controls/scrolledbox.hpp"

void onApplicationReady(Application *self) {
    #ifndef TEST
        for (Widget *child : self->main_widget->children) {
            println(child->rect);
            for (Widget *child : child->children) {
                pprint(4, child->rect);
                for (Widget *child : child->children) {
                    pprint(8, child->rect);
                }
            }
        }
    #endif

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
    assert(self->main_widget->children[2]->rect == Rect<float>(0, 248, 227, 176));
        assert(self->main_widget->children[2]->children[0]->rect == Rect<float>(0, 248, 195, 31));
        assert(self->main_widget->children[2]->children[1]->rect == Rect<float>(0, 279, 227, 31));
        assert(self->main_widget->children[2]->children[2]->rect == Rect<float>(0, 310, 209, 57));
        assert(self->main_widget->children[2]->children[3]->rect == Rect<float>(0, 367, 227, 57));

    // [3]: Align::Vertical, Fill::Both
    assert(self->main_widget->children[3]->rect == Rect<float>(0, 424, 400, 176));
        assert(self->main_widget->children[3]->children[0]->rect == Rect<float>(0, 424, 177, 31));
        assert(self->main_widget->children[3]->children[1]->rect == Rect<float>(0, 455, 400, 31));
        assert(self->main_widget->children[3]->children[2]->rect == Rect<float>(0, 486, 191, 57));
        assert(self->main_widget->children[3]->children[3]->rect == Rect<float>(0, 543, 400, 57));
}

int main() { 
    Application *app = new Application("Box Vertical Test", Size<int>(400, 600));
        app->ready_callback = onApplicationReady;

        // [0]: Align::Vertical, Fill::None
        Box *av_fn = (new Box(Align::Vertical))->set_background(Color(0.0));
            av_fn->append((new Button("P: Fill::None, C: Fill::None"))->set_background(Color(0.7, 0.2, 0.2)), Fill::None);
            av_fn->append((new Button("P: Fill::None, C: Fill::Horizontal"))->set_background(Color(0.7, 0.2, 0.2)), Fill::Horizontal);
            av_fn->append((new Button("P: Fill::None, C: Fill::Vertical"))->set_background(Color(0.7, 0.2, 0.2)), Fill::Vertical);
            av_fn->append((new Button("P: Fill::None, C: Fill::Both"))->set_background(Color(0.7, 0.2, 0.2)), Fill::Both);
        app->append(av_fn, Fill::None);

        // [1]: Align::Vertical, Fill::Horizontal
        Box *av_fh = (new Box(Align::Vertical))->set_background(Color(0.0));
            av_fh->append((new Button("P: Fill::Horizontal, C: Fill::None"))->set_background(Color(0.2, 0.7, 0.2)), Fill::None);
            av_fh->append((new Button("P: Fill::Horizontal, C: Fill::Horizontal"))->set_background(Color(0.2, 0.7, 0.2)), Fill::Horizontal);
            av_fh->append((new Button("P: Fill::Horizontal, C: Fill::Vertical"))->set_background(Color(0.2, 0.7, 0.2)), Fill::Vertical);
            av_fh->append((new Button("P: Fill::Horizontal, C: Fill::Both"))->set_background(Color(0.2, 0.7, 0.2)), Fill::Both);
        app->append(av_fh, Fill::Horizontal);

        // [2]: Align::Vertical, Fill::Vertical
        Box *av_fv = (new Box(Align::Vertical))->set_background(Color(0.0));
            av_fv->append((new Button("P: Fill::Vertical, C: Fill::None"))->set_background(Color(0.2, 0.2, 0.7)), Fill::None);
            av_fv->append((new Button("P: Fill::Vertical, C: Fill::Horizontal"))->set_background(Color(0.2, 0.2, 0.7)), Fill::Horizontal);
            av_fv->append((new Button("P: Fill::Vertical, C: Fill::Vertical"))->set_background(Color(0.2, 0.2, 0.7)), Fill::Vertical);
            av_fv->append((new Button("P: Fill::Vertical, C: Fill::Both"))->set_background(Color(0.2, 0.2, 0.7)), Fill::Both);
        app->append(av_fv, Fill::Vertical);

        // [3]: Align::Vertical, Fill::Both
        Box *av_fb = (new Box(Align::Vertical))->set_background(Color(0.0));
            av_fb->append((new Button("P: Fill::Both, C: Fill::None"))->set_background(Color(0.7, 0.7, 0.2)), Fill::None);
            av_fb->append((new Button("P: Fill::Both, C: Fill::Horizontal"))->set_background(Color(0.7, 0.7, 0.2)), Fill::Horizontal);
            av_fb->append((new Button("P: Fill::Both, C: Fill::Vertical"))->set_background(Color(0.7, 0.7, 0.2)), Fill::Vertical);
            av_fb->append((new Button("P: Fill::Both, C: Fill::Both"))->set_background(Color(0.7, 0.7, 0.2)), Fill::Both);
        app->append(av_fb, Fill::Both);

    #ifdef TEST
        #include "headless.hpp"
    #else
        app->run();
    #endif

    return 0; 
}
