#include <iostream>
#include <string>
#include <vector>
#include <memory>

#include "../src/util.hpp"
#include "../src/application.hpp"
#include "../src/controls/box.hpp"
#include "../src/controls/button.hpp"

int main(int argc, char **argv) { 
    Application *app = Application::get();
        app->onReady = [&](Window *window) {
            if (argc > 1) {
                if (std::string(argv[1]) == std::string("quit")) {
                    window->quit();
                }
            }
        };
        app->setTitle("Box");
        app->resize(400, 600);
        Button *align_button = new Button("Switch Box Alignment");
        app->append(align_button, Fill::None);

        // [0]: Align::Vertical, Fill::None
        Box *av_fn = (new Box(Align::Vertical))->setBackground(COLOR_BLACK);
            av_fn->append((new Button("P: Fill::None, C: Fill::None"))->setBackground(Color(0.7, 0.2, 0.2)), Fill::None);
            av_fn->append((new Button("P: Fill::None, C: Fill::Horizontal"))->setBackground(Color(0.7, 0.2, 0.2)), Fill::Horizontal);
            av_fn->append((new Button("P: Fill::None, C: Fill::Vertical"))->setBackground(Color(0.7, 0.2, 0.2)), Fill::Vertical);
            av_fn->append((new Button("P: Fill::None, C: Fill::Both"))->setBackground(Color(0.7, 0.2, 0.2)), Fill::Both);
        app->append(av_fn, Fill::None);

        // [1]: Align::Vertical, Fill::Horizontal
        Box *av_fh = (new Box(Align::Vertical))->setBackground(COLOR_BLACK);
            av_fh->append((new Button("P: Fill::Horizontal, C: Fill::None"))->setBackground(Color(0.2, 0.7, 0.2)), Fill::None);
            av_fh->append((new Button("P: Fill::Horizontal, C: Fill::Horizontal"))->setBackground(Color(0.2, 0.7, 0.2)), Fill::Horizontal);
            av_fh->append((new Button("P: Fill::Horizontal, C: Fill::Vertical"))->setBackground(Color(0.2, 0.7, 0.2)), Fill::Vertical);
            av_fh->append((new Button("P: Fill::Horizontal, C: Fill::Both"))->setBackground(Color(0.2, 0.7, 0.2)), Fill::Both);
        app->append(av_fh, Fill::Horizontal);

        // [2]: Align::Vertical, Fill::Vertical
        Box *av_fv = (new Box(Align::Vertical))->setBackground(COLOR_BLACK);
            av_fv->append((new Button("P: Fill::Vertical, C: Fill::None"))->setBackground(Color(0.2, 0.2, 0.7)), Fill::None);
            av_fv->append((new Button("P: Fill::Vertical, C: Fill::Horizontal"))->setBackground(Color(0.2, 0.2, 0.7)), Fill::Horizontal);
            av_fv->append((new Button("P: Fill::Vertical, C: Fill::Vertical"))->setBackground(Color(0.2, 0.2, 0.7)), Fill::Vertical);
            av_fv->append((new Button("P: Fill::Vertical, C: Fill::Both"))->setBackground(Color(0.2, 0.2, 0.7)), Fill::Both);
        app->append(av_fv, Fill::Vertical);

        // [3]: Align::Vertical, Fill::Both
        Box *av_fb = (new Box(Align::Vertical))->setBackground(COLOR_BLACK);
            av_fb->append((new Button("P: Fill::Both, C: Fill::None"))->setBackground(Color(0.7, 0.7, 0.2)), Fill::None);
            av_fb->append((new Button("P: Fill::Both, C: Fill::Horizontal"))->setBackground(Color(0.7, 0.7, 0.2)), Fill::Horizontal);
            av_fb->append((new Button("P: Fill::Both, C: Fill::Vertical"))->setBackground(Color(0.7, 0.7, 0.2)), Fill::Vertical);
            av_fb->append((new Button("P: Fill::Both, C: Fill::Both"))->setBackground(Color(0.7, 0.7, 0.2)), Fill::Both);
        app->append(av_fb, Fill::Both);

        Align a = Align::Vertical;
        align_button->onMouseClick.addEventListener([&](Widget *button, MouseEvent event) {
            if (a == Align::Vertical) {
                a = Align::Horizontal;
            } else {
                a = Align::Vertical;
            }
            av_fn->setAlignPolicy(a);
            av_fh->setAlignPolicy(a);
            av_fv->setAlignPolicy(a);
            av_fb->setAlignPolicy(a);
        });
    app->run();

    return 0; 
}
