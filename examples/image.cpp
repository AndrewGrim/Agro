#include "../src/application.hpp"
#include "resources.hpp"

int main(int argc, char **argv) {
    Application *app = Application::get();
        app->onReady = [&](Window *window) {
            if (argc > 1) {
                if (std::string(argv[1]) == std::string("quit")) {
                    window->quit();
                }
            }
        };
        app->setTitle("New Title");
        app->resize(500, 500);

        Box *top = new Box(Align::Horizontal);
            Button *expand = new Button("Expand: false");
            top->append(expand);
            Button *aspect = new Button("Maintain Aspect Ration: true");
            top->append(aspect);
            Button *small = new Button("MinSize 24x24");
            top->append(small);
            Button *original = new Button("Original Size");
            top->append(original);
        app->append(top, Fill::Horizontal);
        Image *img = new Image(lena_png, lena_png_length);
        app->append(img, Fill::Both);

        expand->onMouseClick.addEventListener([&](Widget *button, MouseEvent event) {
            if (img->expand() == false) {
                img->setExpand(true);
                expand->setText("Expand: true");
            } else {
                img->setExpand(false);
                expand->setText("Expand: false");
            }
        });
        aspect->onMouseClick.addEventListener([&](Widget *button, MouseEvent event) {
            if (img->maintainAspectRation() == false) {
                img->setMaintainAspectRatio(true);
                aspect->setText("Maintain Aspect Ration: true");
            } else {
                img->setMaintainAspectRatio(false);
                aspect->setText("Maintain Aspect Ration: false");
            }
        });
        small->onMouseClick.addEventListener([&](Widget *button, MouseEvent event) {
            img->setMinSize(Size(24, 24));
        });
        original->onMouseClick.addEventListener([&](Widget *button, MouseEvent event) {
            img->setMinSize(img->originalSize());
        });
    app->run();

    return 0;
}
