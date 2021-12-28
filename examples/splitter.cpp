#include "../src/util.hpp"
#include "../src/application.hpp"
#include "../src/controls/splitter.hpp"
#include "../src/controls/button.hpp"

struct SplitBox : Box {
    Button *h = new Button("Split Horizontally");
    Button *v = new Button("Split Vertically");
    SplitBox() : Box(Align::Vertical) {
        h->onMouseClick.addEventListener([&](Widget *button, MouseEvent event) {
            auto split = new Splitter(Align::Vertical);
            if (!this->parent) {
                Application::get()->setMainWidget(split);
            } else {
                if (this == ((Splitter*)this->parent)->m_first) {
                    ((Splitter*)this->parent)->left(split);
                } else if (this == ((Splitter*)this->parent)->m_second) {
                    ((Splitter*)this->parent)->right(split);
                } else {
                    fail("somethings wrong");
                }
            }
            split->left(this);
            auto other = new SplitBox();
            split->right(other);
        });
        v->onMouseClick.addEventListener([&](Widget *button, MouseEvent event) {
            auto split = new Splitter(Align::Horizontal);
            if (!this->parent) {
                Application::get()->setMainWidget(split);
            } else {
                if (this == ((Splitter*)this->parent)->m_first) {
                    ((Splitter*)this->parent)->left(split);
                } else if (this == ((Splitter*)this->parent)->m_second) {
                    ((Splitter*)this->parent)->right(split);
                } else {
                    fail("somethings wrong");
                }
            }
            split->left(this);
            auto other = new SplitBox();
            split->right(other);
        });

        append(h, Fill::Both);
        append(v, Fill::Both);
    }
};

int main(int argc, char **argv) { 
    Application *app = Application::get();
        delete app->mainWidget();
        app->setMainWidget(new SplitBox());
        app->onReady = [&](Window *window) {
            if (argc > 1) {
                if (std::string(argv[1]) == std::string("quit")) {
                    window->quit();
                }
            }
        };
        app->setTitle("Splitter");
    app->run();

    return 0; 
}