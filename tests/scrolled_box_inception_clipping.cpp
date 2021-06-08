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
        app->onReady = [&](Window *window) {
            if (argc > 1) {
                if (std::string(argv[1]) == std::string("quit")) {
                    window->quit();
                }
            }
        };
        app->setTitle("Scrolledbox Inception Clipping Test");
        app->resize(1000, 400);
        app->append(new Button("Top"), Fill::Horizontal);
        Box *h_box = new Box(Align::Horizontal);
        {
            h_box->append(new Button("Left"), Fill::Vertical);
            ScrolledBox *left = new ScrolledBox(Align::Horizontal);
            {
                ScrolledBox *inner_top = new ScrolledBox(Align::Horizontal, Size(200, 200));
                    {
                        for (int i = 0; i < 50; i++) {
                            inner_top->append(new Label(std::to_string(i)));
                        }
                    }
                left->append(inner_top, Fill::Both);
                for (int i = 0; i < 50; i++) {
                    left->append(new Label(std::to_string(i)));
                }
                ScrolledBox *inner = new ScrolledBox(Align::Vertical, Size(200, 200));
                    {
                        for (int i = 0; i < 50; i++) {
                            inner->append(new Label(std::to_string(i)));
                        }
                    }
                left->append(inner, Fill::Both);
                for (int i = 0; i < 50; i++) {
                    left->append(new Label(std::to_string(i)));
                }
            }
            h_box->append(left, Fill::Both);

            ScrolledBox *right = new ScrolledBox(Align::Vertical);
            {
                ScrolledBox *inner_top = new ScrolledBox(Align::Vertical, Size(200, 200));
                    {
                        for (int i = 0; i < 50; i++) {
                            inner_top->append(new Label(std::to_string(i)));
                        }
                    }
                right->append(inner_top, Fill::Both);
                for (int i = 0; i < 50; i++) {
                    right->append(new Label(std::to_string(i)));
                }
                ScrolledBox *inner = new ScrolledBox(Align::Horizontal, Size(200, 200));
                    {
                        for (int i = 0; i < 50; i++) {
                            inner->append(new Label(std::to_string(i)));
                        }
                    }
                right->append(inner, Fill::Both);
                for (int i = 0; i < 50; i++) {
                    right->append(new Label(std::to_string(i)));
                }
            }
            h_box->append(right, Fill::Both);
            h_box->append(new Button("Right"), Fill::Vertical);
        }
        app->append(h_box, Fill::Both);
        app->append(new Button("Bottom"), Fill::Horizontal);
    app->run();

    return 0;
}