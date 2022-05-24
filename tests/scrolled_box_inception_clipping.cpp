#include "../src/util.hpp"
#include "../src/application.hpp"
#include "../src/controls/box.hpp"
#include "../src/controls/button.hpp"
#include "../src/controls/label.hpp"
#include "../src/controls/scrolled_box.hpp"

// This test is used for checking that each ScrolledBox (SB) clips correctly.
// Both horizontal and vertical ScrolledBoxes are tested.
// Main things to look for are the four possible clips for ScrolledBox:
// When the SB is within another SB:
//  1 - Inner SB is at least partially outside the outer SB's rect (Align::Vertical).
//  2 - Inner SB is at least partially outside the outer SB's rect (Align::Horizontal).
//  3 - Inner SB is fully within the outer SB's rect (Either Align::).
// When the SB's parent is not an SB:
//  4 - Regular old clip.
int main(int argc, char **argv) {
    Application *app = Application::get();
        app->mainWindow()->onReady = [&](Window *window) {
            if (argc > 1) {
                if (String(argv[1]) == "quit") {
                    window->quit();
                }
            }
        };
        app->mainWindow()->setTitle("Scrolledbox Inception Clipping Test");
        app->mainWindow()->resize(1000, 400);
        app->mainWindow()->append(new Button("Top"), Fill::Horizontal);
        Box *h_box = new Box(Align::Horizontal);
        {
            h_box->append(new Button("Left"), Fill::Vertical);
            ScrolledBox *left = new ScrolledBox(Align::Horizontal);
            left->setWindowBackgroundColor(Color("#ff5555"));
            {
                ScrolledBox *inner_top = new ScrolledBox(Align::Horizontal, Size(200, 200));
                inner_top->setWindowBackgroundColor(Color("#ffaa55"));
                    {
                        for (i32 i = 0; i < 50; i++) {
                            inner_top->append(new Label(toString(i)));
                        }
                    }
                left->append(inner_top, Fill::Both);
                for (i32 i = 0; i < 50; i++) {
                    left->append(new Label(toString(i)));
                }
                ScrolledBox *inner = new ScrolledBox(Align::Vertical, Size(200, 200));
                inner->setWindowBackgroundColor(Color("#55ff55"));
                    {
                        for (i32 i = 0; i < 50; i++) {
                            inner->append(new Label(toString(i)));
                        }
                    }
                left->append(inner, Fill::Both);
                for (i32 i = 0; i < 50; i++) {
                    left->append(new Label(toString(i)));
                }
            }
            h_box->append(left, Fill::Both);

            ScrolledBox *right = new ScrolledBox(Align::Vertical);
            right->setWindowBackgroundColor(Color("#5555ff"));
            {
                ScrolledBox *inner_top = new ScrolledBox(Align::Vertical, Size(200, 200));
                inner_top->setWindowBackgroundColor(Color("#55ffaa"));
                    {
                        for (i32 i = 0; i < 50; i++) {
                            inner_top->append(new Label(toString(i)));
                        }
                    }
                right->append(inner_top, Fill::Both);
                for (i32 i = 0; i < 50; i++) {
                    right->append(new Label(toString(i)));
                }
                ScrolledBox *inner = new ScrolledBox(Align::Horizontal, Size(200, 200));
                inner->setWindowBackgroundColor(Color("#55aaff"));
                    {
                        for (i32 i = 0; i < 50; i++) {
                            inner->append(new Label(toString(i)));
                        }
                    }
                right->append(inner, Fill::Both);
                for (i32 i = 0; i < 50; i++) {
                    right->append(new Label(toString(i)));
                }
            }
            h_box->append(right, Fill::Both);
            h_box->append(new Button("Right"), Fill::Vertical);
        }
        app->mainWindow()->append(h_box, Fill::Both);
        app->mainWindow()->append(new Button("Bottom"), Fill::Horizontal);
    app->run();

    return 0;
}
