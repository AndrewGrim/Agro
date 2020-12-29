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
    // assert(((ScrolledBox*)self->mainWidget())->hasScrollBar(Align::Vertical));
    // assert(((ScrolledBox*)self->mainWidget())->hasScrollBar(Align::Horizontal));

    // assert(((ScrolledBox*)self->mainWidget()->children[0])->hasScrollBar(Align::Vertical));
    // assert(((ScrolledBox*)self->mainWidget()->children[0])->hasScrollBar(Align::Horizontal));
    // assert(((ScrolledBox*)self->mainWidget()->children[1])->hasScrollBar(Align::Vertical));
    // assert(((ScrolledBox*)self->mainWidget()->children[1])->hasScrollBar(Align::Horizontal));
    
    // assert(self->mainWidget()->children[0]->rect == Rect(0, 0, 800, 400));
    // assert(self->mainWidget()->children[1]->rect == Rect(0, 400, 800, 400));
}

int main() { 
    Application *app = new Application("ScrolledBox Both Test", Size(500, 500));
        app->onReady = onApplicationReady;

        ScrolledBox *top = new ScrolledBox(Align::Vertical, Size(800, 400));
            for (char i = 'a'; i <= 'z'; i++) {
                top->append((new Button(std::string(100, i)))->setBackground(Color(0.6, 0.0, 0.6)), Fill::Both);
            }
        app->append(top, Fill::Both);
        ScrolledBox *bottom = new ScrolledBox(Align::Vertical, Size(800, 400));
            for (char i = 'A'; i < 'Z'; i++) {
                bottom->append((new Button(std::string(100, i)))->setBackground(Color(0.0, 0.6, 0.6)), Fill::Both);
            }
        app->append(bottom, Fill::Both);

        

    #ifdef TEST
        #include "headless.hpp"
    #else
        app->run();
    #endif

    return 0; 
}
