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
    assert(!((ScrolledBox*)self->main_widget)->has_scrollbar(Align::Vertical));
    assert(!((ScrolledBox*)self->main_widget)->has_scrollbar(Align::Horizontal));

    assert(((ScrolledBox*)self->main_widget->children[0])->has_scrollbar(Align::Vertical));
    assert(((ScrolledBox*)self->main_widget->children[0])->has_scrollbar(Align::Horizontal));
    assert(((ScrolledBox*)self->main_widget->children[1])->has_scrollbar(Align::Vertical));
    assert(((ScrolledBox*)self->main_widget->children[1])->has_scrollbar(Align::Horizontal));
    
    assert(self->main_widget->children[0]->rect == Rect<float>(0, 0, 500, 250));
    assert(self->main_widget->children[1]->rect == Rect<float>(0, 250, 500, 250));
}

int main() { 
    Application *app = new Application("ScrolledBox Inner Test", Size<int>(500, 500));
        app->ready_callback = onApplicationReady;

        ScrolledBox *top = new ScrolledBox(Align::Vertical, Size<float>(200, 200));
            for (char i = 'a'; i <= 'z'; i++) {
                top->append((new Button(std::string(100, i)))->set_background(Color(0.6, 0.0, 0.6)), Fill::Both);
            }
        app->append(top, Fill::Both);
        ScrolledBox *bottom = new ScrolledBox(Align::Vertical, Size<float>(200, 200));
            for (char i = 'A'; i < 'Z'; i++) {
                bottom->append((new Button(std::string(100, i)))->set_background(Color(0.0, 0.6, 0.6)), Fill::Both);
            }
        app->append(bottom, Fill::Both);

        

    #ifdef TEST
        #include "headless.hpp"
    #else
        app->run();
    #endif

    return 0; 
}
