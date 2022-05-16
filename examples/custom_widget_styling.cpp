#include "../src/util.hpp"
#include "../src/application.hpp"
#include "../src/controls/box.hpp"
#include "../src/controls/button.hpp"

class CustomStyle : public Widget {
    public:
        CustomStyle() {
            setMarginType(STYLE_ALL);
            setBorderType(STYLE_ALL);
            setPaddingType(STYLE_ALL);

            setMarginTop(10);
            setMarginBottom(20);
            setMarginLeft(30);
            setMarginRight(40);

            setBorderTop(10);
            setBorderBottom(20);
            setBorderLeft(30);
            setBorderRight(40);

            // // Changes the color of the border to resemble the
            // // windows logo.
            // setBorderColorTop(Color("#dc3e00"));
            // setBorderColorBottom(Color("#6aa413"));
            // setBorderColorLeft(Color("#009fb8"));
            // setBorderColorRight(Color("#d7a120"));

            setPaddingTop(10);
            setPaddingBottom(20);
            setPaddingLeft(30);
            setPaddingRight(40);
        }

        ~CustomStyle() {

        }

        const char* name() override {
            return "CustomStyle";
        }

        void draw(DrawingContext &dc, Rect rect, i32 state) override {
            this->rect = rect;

            dc.fillRect(rect, Color(1.0f, 0.0f, 0.0f, 0.5f));
            dc.margin(rect, style());
            dc.drawBorder(rect, style(), state);
            dc.fillRect(rect, Color(0.0f, 1.0f, 0.0f, 0.5f));
            dc.padding(rect, style());
            this->inner_rect = rect;

            dc.fillRect(rect, Color(0.0f, 0.0f, 1.0f, 0.5f));
        }

        Size sizeHint(DrawingContext &dc) override {
            Size s = Size(96, 96);
            dc.sizeHintMargin(s, style());
            dc.sizeHintBorder(s, style());
            dc.sizeHintPadding(s, style());
            return s;
        }
};

void update_rect_labels(std::string text, Button *button, Rect rect) {
    button->setText(
        text
        + std::to_string((i32)rect.x) + ", "
        + std::to_string((i32)rect.y) + ", "
        + std::to_string((i32)rect.w) + ", "
        + std::to_string((i32)rect.h)
    );
}

int main(int argc, char **argv) {
    Application *app = Application::get();
        app->mainWindow()->setTitle("Custom Widget Styling");
        app->mainWindow()->resize(600, 400);
        // LEFT
        app->mainWindow()->setMainWidget(new Box(Align::Horizontal));
        CustomStyle *custom = new CustomStyle();
        app->mainWindow()->append(custom, Fill::Both);

        // RIOHT
        Box *right = new Box(Align::Vertical);

            Button *csr = new Button("Rect: ");
            right->append(csr, Fill::Horizontal);
                Button *margin = new Button(
                        "Margin: "
                        + std::to_string(custom->style().margin.top) + ", "
                        + std::to_string(custom->style().margin.bottom) + ", "
                        + std::to_string(custom->style().margin.left) + ", "
                        + std::to_string(custom->style().margin.right)
                    );
                right->append(margin, Fill::Horizontal);
                Button *border = new Button(
                        "Border: "
                        + std::to_string(custom->style().border.top) + ", "
                        + std::to_string(custom->style().border.bottom) + ", "
                        + std::to_string(custom->style().border.left) + ", "
                        + std::to_string(custom->style().border.right)
                    );
                right->append(border, Fill::Horizontal);
                Button *padding = new Button(
                        "Padding: "
                        + std::to_string(custom->style().padding.top) + ", "
                        + std::to_string(custom->style().padding.bottom) + ", "
                        + std::to_string(custom->style().padding.left) + ", "
                        + std::to_string(custom->style().padding.right)
                    );
                right->append(padding, Fill::Horizontal);
            Button *csir = new Button("Content Rect: ");
            right->append(csir, Fill::Horizontal);
        app->mainWindow()->append(right);

        app->mainWindow()->onResize = [&](Window *window) {
            update_rect_labels("Rect: ", csr, custom->rect);
            update_rect_labels("Content Rect: ", csir, custom->inner_rect);
        };
        app->mainWindow()->onReady = [&](Window *window) {
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
