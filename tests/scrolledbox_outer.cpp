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
    // assert(((ScrolledBox*)self->mainWidget())->hasScrollbar(Align::Vertical));
    // assert(((ScrolledBox*)self->mainWidget())->hasScrollbar(Align::Horizontal));

    // assert(self->mainWidget()->children[0]->children[0]->rect == Rect(0, 0, 300, 300));
    // assert(self->mainWidget()->children[0]->children[1]->rect == Rect(300, 0, 300, 300));
    // assert(self->mainWidget()->children[1]->children[0]->rect == Rect(0, 300, 300, 300));
    // assert(self->mainWidget()->children[1]->children[1]->rect == Rect(300, 300, 300, 300));
}

int main(int argc, char **argv) { 
    Application *app = new Application("ScrolledBox Outer Test", Size(500, 500));
        app->onReady = onApplicationReady;

        Box *top = (new Box(Align::Horizontal))->setBackground(Color());
            ScrolledBox *av_fn = (ScrolledBox*)(new ScrolledBox(Align::Vertical, Size(300, 300)))->setBackground(Color(0.0));
                av_fn->append((new Button("P: Fill::None, C: Fill::None"))->setBackground(Color(0.7, 0.2, 0.2)), Fill::None);
                av_fn->append((new Button("P: Fill::None, C: Fill::Horizontal"))->setBackground(Color(0.7, 0.2, 0.2)), Fill::Horizontal);
                av_fn->append((new Button("P: Fill::None, C: Fill::Vertical"))->setBackground(Color(0.7, 0.2, 0.2)), Fill::Vertical);
                av_fn->append((new Button("P: Fill::None, C: Fill::Both"))->setBackground(Color(0.7, 0.2, 0.2)), Fill::Both);
            top->append(av_fn, Fill::None);

            ScrolledBox *av_fb = (ScrolledBox*)(new ScrolledBox(Align::Vertical, Size(300, 300)))->setBackground(Color(0.0));
                av_fb->append((new Button("P: Fill::Both, C: Fill::None"))->setBackground(Color(0.7, 0.7, 0.2)), Fill::None);
                av_fb->append((new Button("P: Fill::Both, C: Fill::Horizontal"))->setBackground(Color(0.7, 0.7, 0.2)), Fill::Horizontal);
                av_fb->append((new Button("P: Fill::Both, C: Fill::Vertical"))->setBackground(Color(0.7, 0.7, 0.2)), Fill::Vertical);
                av_fb->append((new Button("P: Fill::Both, C: Fill::Both"))->setBackground(Color(0.7, 0.7, 0.2)), Fill::Both);
            top->append(av_fb, Fill::Both);
        app->append(top, Fill::Both);

        Box *bottom = (new Box(Align::Horizontal))->setBackground(Color());
            ScrolledBox *av_fh = (ScrolledBox*)(new ScrolledBox(Align::Vertical, Size(300, 300)))->setBackground(Color(0.0));
                av_fh->append((new Button("P: Fill::Horizontal, C: Fill::None"))->setBackground(Color(0.2, 0.7, 0.2)), Fill::None);
                av_fh->append((new Button("P: Fill::Horizontal, C: Fill::Horizontal"))->setBackground(Color(0.2, 0.7, 0.2)), Fill::Horizontal);
                av_fh->append((new Button("P: Fill::Horizontal, C: Fill::Vertical"))->setBackground(Color(0.2, 0.7, 0.2)), Fill::Vertical);
                av_fh->append((new Button("P: Fill::Horizontal, C: Fill::Both"))->setBackground(Color(0.2, 0.7, 0.2)), Fill::Both);
            bottom->append(av_fh, Fill::Horizontal);

            ScrolledBox *av_fv = (ScrolledBox*)(new ScrolledBox(Align::Vertical, Size(300, 300)))->setBackground(Color(0.0));
                av_fv->append((new Button("P: Fill::Vertical, C: Fill::None"))->setBackground(Color(0.2, 0.2, 0.7)), Fill::None);
                av_fv->append((new Button("P: Fill::Vertical, C: Fill::Horizontal"))->setBackground(Color(0.2, 0.2, 0.7)), Fill::Horizontal);
                av_fv->append((new Button("P: Fill::Vertical, C: Fill::Vertical"))->setBackground(Color(0.2, 0.2, 0.7)), Fill::Vertical);
                av_fv->append((new Button("P: Fill::Vertical, C: Fill::Both"))->setBackground(Color(0.2, 0.2, 0.7)), Fill::Both);
            bottom->append(av_fv, Fill::Vertical);
        app->append(bottom, Fill::Both);

        

    #ifdef TEST
        #include "headless.hpp"
    #else
        app->run();
    #endif

    return 0; 
}
