#include <iostream>
#include <vector>
#include <memory>

#include "../src/util.hpp"
#include "../src/application.hpp"
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
    // [0]: Align::Horizontal, Fill::None
    // assert(self->mainWidget()->children[0]->rect == Rect(0, 0, 766, 31));
    //     assert(self->mainWidget()->children[0]->children[0]->rect == Rect(0, 0, 181, 31));
    //     assert(self->mainWidget()->children[0]->children[1]->rect == Rect(181, 0, 213, 31));
    //     assert(self->mainWidget()->children[0]->children[2]->rect == Rect(394, 0, 195, 31));
    //     assert(self->mainWidget()->children[0]->children[3]->rect == Rect(589, 0, 177, 31));

    // // [1]: Align::Horizontal, Fill::Horizontal
    // assert(self->mainWidget()->children[1]->rect == Rect(0, 31, 1000, 31));
    //     assert(self->mainWidget()->children[1]->children[0]->rect == Rect(0, 31, 213, 31));
    //     assert(self->mainWidget()->children[1]->children[1]->rect == Rect(213, 31, 298, 31));
    //     assert(self->mainWidget()->children[1]->children[2]->rect == Rect(511, 31, 227, 31));
    //     assert(self->mainWidget()->children[1]->children[3]->rect == Rect(738, 31, 262, 31));

    // // [2]: Align::Horizontal, Fill::Vertical
    // assert(self->mainWidget()->children[2]->rect == Rect(0, 62, 822, 169));
    //     assert(self->mainWidget()->children[2]->children[0]->rect == Rect(0, 62, 195, 31));
    //     assert(self->mainWidget()->children[2]->children[1]->rect == Rect(195, 62, 227, 31));
    //     assert(self->mainWidget()->children[2]->children[2]->rect == Rect(422, 62, 209, 169));
    //     assert(self->mainWidget()->children[2]->children[3]->rect == Rect(631, 62, 191, 169));

    // // [3]: Align::Horizontal, Fill::Both
    // assert(self->mainWidget()->children[3]->rect == Rect(0, 231, 1000, 169));
    //     assert(self->mainWidget()->children[3]->children[0]->rect == Rect(0, 231, 177, 31));
    //     assert(self->mainWidget()->children[3]->children[1]->rect == Rect(177, 231, 334, 31));
    //     assert(self->mainWidget()->children[3]->children[2]->rect == Rect(511, 231, 191, 169));
    //     assert(self->mainWidget()->children[3]->children[3]->rect == Rect(702, 231, 298, 169));
}

int main(int argc, char **argv) { 
    Application *app = new Application("Box Horizontal Test", Size(1000, 400));
        app->onReady = onApplicationReady;

        // [0]: Align::Horizontal, Fill::None
        Box *av_fn = (new Box(Align::Horizontal))->setBackground(Color(0.0));
            av_fn->append((new Button("P: Fill::None, C: Fill::None"))->setBackground(Color(0.7, 0.2, 0.2)), Fill::None);
            av_fn->append((new Button("P: Fill::None, C: Fill::Horizontal"))->setBackground(Color(0.7, 0.2, 0.2)), Fill::Horizontal);
            av_fn->append((new Button("P: Fill::None, C: Fill::Vertical"))->setBackground(Color(0.7, 0.2, 0.2)), Fill::Vertical);
            av_fn->append((new Button("P: Fill::None, C: Fill::Both"))->setBackground(Color(0.7, 0.2, 0.2)), Fill::Both);
        app->append(av_fn, Fill::None);

        // [1]: Align::Horizontal, Fill::Horizontal
        Box *av_fh = (new Box(Align::Horizontal))->setBackground(Color(0.0));
            av_fh->append((new Button("P: Fill::Horizontal, C: Fill::None"))->setBackground(Color(0.2, 0.7, 0.2)), Fill::None);
            av_fh->append((new Button("P: Fill::Horizontal, C: Fill::Horizontal"))->setBackground(Color(0.2, 0.7, 0.2)), Fill::Horizontal);
            av_fh->append((new Button("P: Fill::Horizontal, C: Fill::Vertical"))->setBackground(Color(0.2, 0.7, 0.2)), Fill::Vertical);
            av_fh->append((new Button("P: Fill::Horizontal, C: Fill::Both"))->setBackground(Color(0.2, 0.7, 0.2)), Fill::Both);
        app->append(av_fh, Fill::Horizontal);

        // [2]: Align::Horizontal, Fill::Vertical
        Box *av_fv = (new Box(Align::Horizontal))->setBackground(Color(0.0));
            av_fv->append((new Button("P: Fill::Vertical, C: Fill::None"))->setBackground(Color(0.2, 0.2, 0.7)), Fill::None);
            av_fv->append((new Button("P: Fill::Vertical, C: Fill::Horizontal"))->setBackground(Color(0.2, 0.2, 0.7)), Fill::Horizontal);
            av_fv->append((new Button("P: Fill::Vertical, C: Fill::Vertical"))->setBackground(Color(0.2, 0.2, 0.7)), Fill::Vertical);
            av_fv->append((new Button("P: Fill::Vertical, C: Fill::Both"))->setBackground(Color(0.2, 0.2, 0.7)), Fill::Both);
        app->append(av_fv, Fill::Vertical);

        // [3]: Align::Horizontal, Fill::Both
        Box *av_fb = (new Box(Align::Horizontal))->setBackground(Color(0.0));
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
