#include "../src/util.hpp"
#include "../src/application.hpp"
#include "../src/controls/label.hpp"

int main(int argc, char **argv) {
    Application *app = Application::get();
        app->mainWindow()->onReady = [&](Window *window) {
            if (argc > 1) {
                if (std::string(argv[1]) == std::string("quit")) {
                    window->quit();
                }
            }
        };
        app->mainWindow()->resize(600, 600);
        app->mainWindow()->center();
        app->mainWindow()->setTitle("Label");
        app->mainWindow()->append(new Label("Space: ' ', Tab: '\t'."));
        VerticalAlignment v_align[3] = {
            VerticalAlignment::Top,
            VerticalAlignment::Center,
            VerticalAlignment::Bottom
        };
        HorizontalAlignment h_align[3] = {
            HorizontalAlignment::Left,
            HorizontalAlignment::Center,
            HorizontalAlignment::Right
        };
        Color colors[3] = { Color("#ff5555"), Color("#55ff55"), Color("#5555ff") };
        for (int i = 0; i < 3; i++) {
            Box *box = new Box(Align::Horizontal);
                box->style.window_background = colors[i];
                for (int j = 0; j < 3; j++) {
                    Label *label = new Label("This text spans\nmultiple\nlines!");
                        label->setVerticalAlignment(v_align[i]);
                        label->setHorizontalAlignment(h_align[j]);
                    box->append(label, Fill::Both);
                }
            app->mainWindow()->append(box, Fill::Both);
        }
    app->run();

    return 0;
}
