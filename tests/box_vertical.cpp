#include <iostream>
#include <vector>
#include <memory>

#include "../src/util.hpp"
#include "../src/app.hpp"
#include "../src/controls/box.hpp"
#include "../src/controls/button.hpp"
// #include "../src/controls/scrolledbox.hpp"

void onApplicationReady(Application *self) {
    #ifndef TEST
        for (Widget *child : self->mainWidget()->children) {
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
    // assert(self->mainWidget()->children[0]->rect == Rect(0, 0, 213, 124));
    //     assert(self->mainWidget()->children[0]->children[0]->rect == Rect(0, 0, 181, 31));
    //     assert(self->mainWidget()->children[0]->children[1]->rect == Rect(0, 31, 213, 31));
    //     assert(self->mainWidget()->children[0]->children[2]->rect == Rect(0, 62, 195, 31));
    //     assert(self->mainWidget()->children[0]->children[3]->rect == Rect(0, 93, 213, 31));

    // // [1]: Align::Vertical, Fill::Horizontal
    // assert(self->mainWidget()->children[1]->rect == Rect(0, 124, 400, 124));
    //     assert(self->mainWidget()->children[1]->children[0]->rect == Rect(0, 124, 213, 31));
    //     assert(self->mainWidget()->children[1]->children[1]->rect == Rect(0, 155, 400, 31));
    //     assert(self->mainWidget()->children[1]->children[2]->rect == Rect(0, 186, 227, 31));
    //     assert(self->mainWidget()->children[1]->children[3]->rect == Rect(0, 217, 400, 31));

    // // [2]: Align::Vertical, Fill::Vertical
    // assert(self->mainWidget()->children[2]->rect == Rect(0, 248, 227, 176));
    //     assert(self->mainWidget()->children[2]->children[0]->rect == Rect(0, 248, 195, 31));
    //     assert(self->mainWidget()->children[2]->children[1]->rect == Rect(0, 279, 227, 31));
    //     assert(self->mainWidget()->children[2]->children[2]->rect == Rect(0, 310, 209, 57));
    //     assert(self->mainWidget()->children[2]->children[3]->rect == Rect(0, 367, 227, 57));

    // // [3]: Align::Vertical, Fill::Both
    // assert(self->mainWidget()->children[3]->rect == Rect(0, 424, 400, 176));
    //     assert(self->mainWidget()->children[3]->children[0]->rect == Rect(0, 424, 177, 31));
    //     assert(self->mainWidget()->children[3]->children[1]->rect == Rect(0, 455, 400, 31));
    //     assert(self->mainWidget()->children[3]->children[2]->rect == Rect(0, 486, 191, 57));
    //     assert(self->mainWidget()->children[3]->children[3]->rect == Rect(0, 543, 400, 57));
}

int main(int argc, char **argv) { 
    Application *app = new Application("Box Vertical Test", Size(400, 600));
        app->onReady = onApplicationReady;

        // [0]: Align::Vertical, Fill::None
        Box *av_fn = (new Box(Align::Vertical))->setBackground(Color(0.0));
            av_fn->append((new Button("P: Fill::None, C: Fill::None"))->setBackground(Color(0.7, 0.2, 0.2)), Fill::None);
            av_fn->append((new Button("P: Fill::None, C: Fill::Horizontal"))->setBackground(Color(0.7, 0.2, 0.2)), Fill::Horizontal);
            av_fn->append((new Button("P: Fill::None, C: Fill::Vertical"))->setBackground(Color(0.7, 0.2, 0.2)), Fill::Vertical);
            av_fn->append((new Button("P: Fill::None, C: Fill::Both"))->setBackground(Color(0.7, 0.2, 0.2)), Fill::Both);
        app->append(av_fn, Fill::None);

        // [1]: Align::Vertical, Fill::Horizontal
        Box *av_fh = (new Box(Align::Vertical))->setBackground(Color(0.0));
            av_fh->append((new Button("P: Fill::Horizontal, C: Fill::None"))->setBackground(Color(0.2, 0.7, 0.2)), Fill::None);
            av_fh->append((new Button("P: Fill::Horizontal, C: Fill::Horizontal"))->setBackground(Color(0.2, 0.7, 0.2)), Fill::Horizontal);
            av_fh->append((new Button("P: Fill::Horizontal, C: Fill::Vertical"))->setBackground(Color(0.2, 0.7, 0.2)), Fill::Vertical);
            av_fh->append((new Button("P: Fill::Horizontal, C: Fill::Both"))->setBackground(Color(0.2, 0.7, 0.2)), Fill::Both);
        app->append(av_fh, Fill::Horizontal);

        // [2]: Align::Vertical, Fill::Vertical
        Box *av_fv = (new Box(Align::Vertical))->setBackground(Color(0.0));
            av_fv->append((new Button("P: Fill::Vertical, C: Fill::None"))->setBackground(Color(0.2, 0.2, 0.7)), Fill::None);
            av_fv->append((new Button("P: Fill::Vertical, C: Fill::Horizontal"))->setBackground(Color(0.2, 0.2, 0.7)), Fill::Horizontal);
            av_fv->append((new Button("P: Fill::Vertical, C: Fill::Vertical"))->setBackground(Color(0.2, 0.2, 0.7)), Fill::Vertical);
            av_fv->append((new Button("P: Fill::Vertical, C: Fill::Both"))->setBackground(Color(0.2, 0.2, 0.7)), Fill::Both);
        app->append(av_fv, Fill::Vertical);

        // [3]: Align::Vertical, Fill::Both
        Box *av_fb = (new Box(Align::Vertical))->setBackground(Color(0.0));
            av_fb->append((new Button("P: Fill::Both, C: Fill::None"))->setBackground(Color(0.7, 0.7, 0.2)), Fill::None);
            av_fb->append((new Button("P: Fill::Both, C: Fill::Horizontal"))->setBackground(Color(0.7, 0.7, 0.2)), Fill::Horizontal);
            av_fb->append((new Button("P: Fill::Both, C: Fill::Vertical"))->setBackground(Color(0.7, 0.7, 0.2)), Fill::Vertical);
            av_fb->append((new Button("P: Fill::Both, C: Fill::Both"))->setBackground(Color(0.7, 0.7, 0.2)), Fill::Both);
        app->append(av_fb, Fill::Both);

    #ifdef TEST
        #include "headless.hpp"
    #else
        app->run();
    #endif

    return 0; 
}
