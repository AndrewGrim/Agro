#include <iostream>
#include <vector>
#include <memory>

#include "../src/util.hpp"
#include "../src/application.hpp"
#include "../src/controls/box.hpp"
#include "../src/controls/button.hpp"
#include "../src/controls/scrolledbox.hpp"

int main(int argc, char **argv) { 
    Application *app = Application::get();
        app->onReady = [&](Window *window) {
            if (argc > 1) {
                if (std::string(argv[1]) == std::string("quit")) {
                    window->quit();
                }
            }
        };
        app->setTitle("ScrolledBox Outer Test");
        app->resize(500, 500);
        Box *top = (new Box(Align::Horizontal))->setBackground(COLOR_BLACK);
            ScrolledBox *av_fn = (ScrolledBox*)(new ScrolledBox(Align::Vertical, Size(300, 300)))->setBackground(COLOR_BLACK);
                av_fn->append((new Button("P: Fill::None, C: Fill::None"))->setBackground(Color(0.7f, 0.2f, 0.2f)), Fill::None);
                av_fn->append((new Button("P: Fill::None, C: Fill::Horizontal"))->setBackground(Color(0.7f, 0.2f, 0.2f)), Fill::Horizontal);
                av_fn->append((new Button("P: Fill::None, C: Fill::Vertical"))->setBackground(Color(0.7f, 0.2f, 0.2f)), Fill::Vertical);
                av_fn->append((new Button("P: Fill::None, C: Fill::Both"))->setBackground(Color(0.7f, 0.2f, 0.2f)), Fill::Both);
            top->append(av_fn, Fill::None);

            ScrolledBox *av_fb = (ScrolledBox*)(new ScrolledBox(Align::Vertical, Size(300, 300)))->setBackground(COLOR_BLACK);
                av_fb->append((new Button("P: Fill::Both, C: Fill::None"))->setBackground(Color(0.7f, 0.7f, 0.2f)), Fill::None);
                av_fb->append((new Button("P: Fill::Both, C: Fill::Horizontal"))->setBackground(Color(0.7f, 0.7f, 0.2f)), Fill::Horizontal);
                av_fb->append((new Button("P: Fill::Both, C: Fill::Vertical"))->setBackground(Color(0.7f, 0.7f, 0.2f)), Fill::Vertical);
                av_fb->append((new Button("P: Fill::Both, C: Fill::Both"))->setBackground(Color(0.7f, 0.7f, 0.2f)), Fill::Both);
            top->append(av_fb, Fill::Both);
        app->append(top, Fill::Both);

        Box *bottom = (new Box(Align::Horizontal))->setBackground(COLOR_BLACK);
            ScrolledBox *av_fh = (ScrolledBox*)(new ScrolledBox(Align::Vertical, Size(300, 300)))->setBackground(COLOR_BLACK);
                av_fh->append((new Button("P: Fill::Horizontal, C: Fill::None"))->setBackground(Color(0.2f, 0.7f, 0.2f)), Fill::None);
                av_fh->append((new Button("P: Fill::Horizontal, C: Fill::Horizontal"))->setBackground(Color(0.2f, 0.7f, 0.2f)), Fill::Horizontal);
                av_fh->append((new Button("P: Fill::Horizontal, C: Fill::Vertical"))->setBackground(Color(0.2f, 0.7f, 0.2f)), Fill::Vertical);
                av_fh->append((new Button("P: Fill::Horizontal, C: Fill::Both"))->setBackground(Color(0.2f, 0.7f, 0.2f)), Fill::Both);
            bottom->append(av_fh, Fill::Horizontal);

            ScrolledBox *av_fv = (ScrolledBox*)(new ScrolledBox(Align::Vertical, Size(300, 300)))->setBackground(COLOR_BLACK);
                av_fv->append((new Button("P: Fill::Vertical, C: Fill::None"))->setBackground(Color(0.2f, 0.2f, 0.7f)), Fill::None);
                av_fv->append((new Button("P: Fill::Vertical, C: Fill::Horizontal"))->setBackground(Color(0.2f, 0.2f, 0.7f)), Fill::Horizontal);
                av_fv->append((new Button("P: Fill::Vertical, C: Fill::Vertical"))->setBackground(Color(0.2f, 0.2f, 0.7f)), Fill::Vertical);
                av_fv->append((new Button("P: Fill::Vertical, C: Fill::Both"))->setBackground(Color(0.2f, 0.2f, 0.7f)), Fill::Both);
            bottom->append(av_fv, Fill::Vertical);
        app->append(bottom, Fill::Both);
    app->run();

    return 0; 
}
