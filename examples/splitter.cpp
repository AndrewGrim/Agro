#include "../src/util.hpp"
#include "../src/application.hpp"
#include "../src/controls/splitter.hpp"
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
        app->setTitle("Splitter");
        Splitter *h = new Splitter(Align::Horizontal);
            h->left(new Button("Left"));
            Box *box = new Box(Align::Vertical);
                box->append(new Button("Inner box top"), Fill::Both);
                box->append(new Button("Inner box bottom"), Fill::Both);
            h->right(box);
        app->append(h, Fill::Both);

        Splitter *v = new Splitter(Align::Vertical);
            v->top(new Button("Top"));
            v->bottom(new Button("Bottom"));
        app->append(v, Fill::Both);
    app->run();

    return 0; 
}