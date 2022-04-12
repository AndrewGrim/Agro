#include <iostream>
#include <vector>
#include <memory>

#include "../src/util.hpp"
#include "../src/application.hpp"
#include "../src/controls/box.hpp"
#include "../src/controls/button.hpp"
#include "../src/controls/scrolled_box.hpp"

int main(int argc, char **argv) {
    Application *app = Application::get();
        app->mainWindow()->onReady = [&](Window *window) {
            if (argc > 1) {
                if (std::string(argv[1]) == std::string("quit")) {
                    window->quit();
                }
            }
        };
        app->mainWindow()->setTitle("ScrolledBox Outer Test");
        app->mainWindow()->resize(500, 500);
        Box *top = new Box(Align::Horizontal);
            ScrolledBox *av_fn = new ScrolledBox(Align::Vertical, Size(300, 300));
                av_fn->style.window_background = COLOR_BLACK;
                av_fn->append(new Button("P: Fill::None, C: Fill::None"), Fill::None);
                av_fn->append(new Button("P: Fill::None, C: Fill::Horizontal"), Fill::Horizontal);
                av_fn->append(new Button("P: Fill::None, C: Fill::Vertical"), Fill::Vertical);
                av_fn->append(new Button("P: Fill::None, C: Fill::Both"), Fill::Both);
                for (auto child : av_fn->children) { child->style.widget_background = Color("#ff5555"); }
            top->append(av_fn, Fill::None);

            ScrolledBox *av_fb = new ScrolledBox(Align::Vertical, Size(300, 300));
                av_fb->style.window_background = COLOR_BLACK;
                av_fb->append(new Button("P: Fill::Both, C: Fill::None"), Fill::None);
                av_fb->append(new Button("P: Fill::Both, C: Fill::Horizontal"), Fill::Horizontal);
                av_fb->append(new Button("P: Fill::Both, C: Fill::Vertical"), Fill::Vertical);
                av_fb->append(new Button("P: Fill::Both, C: Fill::Both"), Fill::Both);
                for (auto child : av_fb->children) { child->style.widget_background = Color("#ffaa55"); }
            top->append(av_fb, Fill::Both);
        app->mainWindow()->append(top, Fill::Both);

        Box *bottom = new Box(Align::Horizontal);
            ScrolledBox *av_fh = new ScrolledBox(Align::Vertical, Size(300, 300));
                av_fh->style.window_background = COLOR_BLACK;
                av_fh->append(new Button("P: Fill::Horizontal, C: Fill::None"), Fill::None);
                av_fh->append(new Button("P: Fill::Horizontal, C: Fill::Horizontal"), Fill::Horizontal);
                av_fh->append(new Button("P: Fill::Horizontal, C: Fill::Vertical"), Fill::Vertical);
                av_fh->append(new Button("P: Fill::Horizontal, C: Fill::Both"), Fill::Both);
                for (auto child : av_fh->children) { child->style.widget_background = Color("#55ff55"); }
            bottom->append(av_fh, Fill::Horizontal);

            ScrolledBox *av_fv = new ScrolledBox(Align::Vertical, Size(300, 300));
                av_fv->style.window_background = COLOR_BLACK;
                av_fv->append(new Button("P: Fill::Vertical, C: Fill::None"), Fill::None);
                av_fv->append(new Button("P: Fill::Vertical, C: Fill::Horizontal"), Fill::Horizontal);
                av_fv->append(new Button("P: Fill::Vertical, C: Fill::Vertical"), Fill::Vertical);
                av_fv->append(new Button("P: Fill::Vertical, C: Fill::Both"), Fill::Both);
                for (auto child : av_fv->children) { child->style.widget_background = Color("#5555ff"); }
            bottom->append(av_fv, Fill::Vertical);
        app->mainWindow()->append(bottom, Fill::Both);
    app->run();

    return 0;
}
