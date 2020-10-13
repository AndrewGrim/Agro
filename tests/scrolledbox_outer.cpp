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
    assert(((ScrolledBox*)self->main_widget)->has_scrollbar(Align::Vertical));
    assert(((ScrolledBox*)self->main_widget)->has_scrollbar(Align::Horizontal));

    assert(self->main_widget->children[0]->children[0]->rect == Rect<float>(0, 0, 250, 250));
    assert(self->main_widget->children[0]->children[1]->rect == Rect<float>(250, 0, 250, 250));
    assert(self->main_widget->children[1]->children[0]->rect == Rect<float>(0, 250, 250, 250));
    assert(self->main_widget->children[1]->children[1]->rect == Rect<float>(250, 250, 250, 250));
}

int main() { 
    Application *app = new Application("ScrolledBox Outer Test", Size<int>(400, 400));
        app->ready_callback = onApplicationReady;

        Box *top = (new Box(Align::Horizontal))->set_background(Color());
            ScrolledBox *av_fn = (new ScrolledBox(Align::Vertical, Size<float>(250, 250)))->set_background(Color(0.0));
                av_fn->append((new Button("P: Fill::None, C: Fill::None"))->set_background(Color(0.7, 0.2, 0.2)), Fill::None);
                av_fn->append((new Button("P: Fill::None, C: Fill::Horizontal"))->set_background(Color(0.7, 0.2, 0.2)), Fill::Horizontal);
                av_fn->append((new Button("P: Fill::None, C: Fill::Vertical"))->set_background(Color(0.7, 0.2, 0.2)), Fill::Vertical);
                av_fn->append((new Button("P: Fill::None, C: Fill::Both"))->set_background(Color(0.7, 0.2, 0.2)), Fill::Both);
            top->append(av_fn, Fill::None);

            ScrolledBox *av_fb = (new ScrolledBox(Align::Vertical, Size<float>(250, 250)))->set_background(Color(0.0));
                av_fb->append((new Button("P: Fill::Both, C: Fill::None"))->set_background(Color(0.7, 0.7, 0.2)), Fill::None);
                av_fb->append((new Button("P: Fill::Both, C: Fill::Horizontal"))->set_background(Color(0.7, 0.7, 0.2)), Fill::Horizontal);
                av_fb->append((new Button("P: Fill::Both, C: Fill::Vertical"))->set_background(Color(0.7, 0.7, 0.2)), Fill::Vertical);
                av_fb->append((new Button("P: Fill::Both, C: Fill::Both"))->set_background(Color(0.7, 0.7, 0.2)), Fill::Both);
            top->append(av_fb, Fill::Both);
        app->append(top, Fill::Both);

        Box *bottom = (new Box(Align::Horizontal))->set_background(Color());
            ScrolledBox *av_fh = (new ScrolledBox(Align::Vertical, Size<float>(250, 250)))->set_background(Color(0.0));
                av_fh->append((new Button("P: Fill::Horizontal, C: Fill::None"))->set_background(Color(0.2, 0.7, 0.2)), Fill::None);
                av_fh->append((new Button("P: Fill::Horizontal, C: Fill::Horizontal"))->set_background(Color(0.2, 0.7, 0.2)), Fill::Horizontal);
                av_fh->append((new Button("P: Fill::Horizontal, C: Fill::Vertical"))->set_background(Color(0.2, 0.7, 0.2)), Fill::Vertical);
                av_fh->append((new Button("P: Fill::Horizontal, C: Fill::Both"))->set_background(Color(0.2, 0.7, 0.2)), Fill::Both);
            bottom->append(av_fh, Fill::Horizontal);

            ScrolledBox *av_fv = (new ScrolledBox(Align::Vertical, Size<float>(250, 250)))->set_background(Color(0.0));
                av_fv->append((new Button("P: Fill::Vertical, C: Fill::None"))->set_background(Color(0.2, 0.2, 0.7)), Fill::None);
                av_fv->append((new Button("P: Fill::Vertical, C: Fill::Horizontal"))->set_background(Color(0.2, 0.2, 0.7)), Fill::Horizontal);
                av_fv->append((new Button("P: Fill::Vertical, C: Fill::Vertical"))->set_background(Color(0.2, 0.2, 0.7)), Fill::Vertical);
                av_fv->append((new Button("P: Fill::Vertical, C: Fill::Both"))->set_background(Color(0.2, 0.2, 0.7)), Fill::Both);
            bottom->append(av_fv, Fill::Vertical);
        app->append(bottom, Fill::Both);

        

    #ifdef TEST
        app->dc->loadFont("fonts/FreeSans.ttf", 14);
        app->main_widget->m_app = (void*)app;
        for (Widget *child : app->main_widget->children) {
            child->m_app = (void*)app;
            child->attach_app((void*)app);
        }
        app->show();
        if (app->ready_callback) {
            app->ready_callback(app);
        }
    #else
        app->run();
    #endif

    return 0; 
}
