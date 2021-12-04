#include "../src/util.hpp"
#include "../src/application.hpp"
#include "../src/controls/box.hpp"
#include "../src/controls/button.hpp"

class CustomStyle : public Widget {
    public:
        CustomStyle() {
            style.margin.type = STYLE_ALL;
            style.border.type = STYLE_ALL;
            style.padding.type = STYLE_ALL;

            style.margin.top = 10;
            style.margin.bottom = 20;
            style.margin.left = 30;
            style.margin.right = 40;

            style.border.top = 10;
            style.border.bottom = 20;
            style.border.left = 30;
            style.border.right = 40;

            // // Changes the color of the border to resemble the
            // // windows logo.
            // style.border.color_top = Color("#dc3e00");
            // style.border.color_bottom = Color("#6aa413");
            // style.border.color_left = Color("#009fb8");
            // style.border.color_right = Color("#d7a120");

            style.padding.top = 10;
            style.padding.bottom = 20;
            style.padding.left = 30;
            style.padding.right = 40;
        }

        ~CustomStyle() {

        }

        const char* name() override {
            return "CustomStyle";
        }

        void draw(DrawingContext &dc, Rect rect, int state) {
            this->rect = rect;

            dc.fillRect(rect, Color(1.0f, 0.0f, 0.0f, 0.5f));
            dc.margin(rect, style);
            dc.drawBorder(rect, style, state);
            dc.fillRect(rect, Color(0.0f, 1.0f, 0.0f, 0.5f));
            dc.padding(rect, style);
            this->inner_rect = rect;

            dc.fillRect(rect, Color(0.0f, 0.0f, 1.0f, 0.5f));
        }

        Size sizeHint(DrawingContext &dc) {
            Size s = Size(96, 96);
            dc.sizeHintMargin(s, style);
            dc.sizeHintBorder(s, style);
            dc.sizeHintPadding(s, style);
            return s;
        }
};

void update_rect_labels(std::string text, Button *button, Rect rect) {
    button->setText(
        text
        + std::to_string((int)rect.x) + ", "
        + std::to_string((int)rect.y) + ", "
        + std::to_string((int)rect.w) + ", "
        + std::to_string((int)rect.h)
    );
}

int main(int argc, char **argv) {
    Application *app = Application::get();
        app->setTitle("Custom Widget Styling");
        app->resize(600, 400);
        // LEFT
        app->setMainWidget(new Box(Align::Horizontal));
        CustomStyle *custom = new CustomStyle();
        app->append(custom, Fill::Both);

        // RIOHT
        Box *right = new Box(Align::Vertical);

            Button *csr = new Button("Rect: ");
            right->append(csr, Fill::Horizontal);
                Button *margin = new Button(
                        "Margin: "
                        + std::to_string(custom->style.margin.top) + ", "
                        + std::to_string(custom->style.margin.bottom) + ", "
                        + std::to_string(custom->style.margin.left) + ", "
                        + std::to_string(custom->style.margin.right)
                    );
                right->append(margin, Fill::Horizontal);
                Button *border = new Button(
                        "Border: "
                        + std::to_string(custom->style.border.top) + ", "
                        + std::to_string(custom->style.border.bottom) + ", "
                        + std::to_string(custom->style.border.left) + ", "
                        + std::to_string(custom->style.border.right)
                    );
                right->append(border, Fill::Horizontal);
                Button *padding = new Button(
                        "Padding: "
                        + std::to_string(custom->style.padding.top) + ", "
                        + std::to_string(custom->style.padding.bottom) + ", "
                        + std::to_string(custom->style.padding.left) + ", "
                        + std::to_string(custom->style.padding.right)
                    );
                right->append(padding, Fill::Horizontal);
            Button *csir = new Button("Content Rect: ");
            right->append(csir, Fill::Horizontal);
        app->append(right);

        app->onResize = [&](Window *window) {
            update_rect_labels("Rect: ", csr, custom->rect);
            update_rect_labels("Content Rect: ", csir, custom->inner_rect);
        };
        app->onReady = [&](Window *window) {
            update_rect_labels("Rect: ", csr, custom->rect);
            update_rect_labels("Content Rect: ", csir, custom->inner_rect);
            if (argc > 1) {
                if (std::string(argv[1]) == std::string("quit")) {
                    window->quit();
                }
            }
        };
    app->run();

    return 0;
}
