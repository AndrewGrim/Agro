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

            // TODO implement
            style.border.color_top = Color(0.2, 0.4, 0.7);

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

        void draw(DrawingContext *dc, Rect rect) {
            this->rect = rect;

            dc->fillRect(rect, Color(1, 0, 0, 0.5));
            dc->margin(rect, style);
            dc->drawBorder(rect, style);
            dc->fillRect(rect, Color(0, 1, 0, 0.5));
            dc->padding(rect, style);
            this->inner_rect = rect;

            dc->fillRect(rect, Color(0, 0, 1, 0.5));
        }

        Size sizeHint(DrawingContext *dc) {
            Size s = Size(96, 96);
            dc->sizeHintMargin(s, style);
            dc->sizeHintBorder(s, style);
            dc->sizeHintPadding(s, style);
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
        app->setMainWidget((new Box(Align::Horizontal))->setBackground(COLOR_WHITE));
        CustomStyle *custom = new CustomStyle();
        app->append(custom, Fill::Both);

        // RIOHT
        Box *right = new Box(Align::Vertical);
            right->setBackground(Color(0.8, 0.8, 0.8));

            Button *csr = new Button("Rect: ");
            right->append(csr, Fill::Horizontal);
                Button *margin = new Button(
                        "Margin: "
                        + std::to_string(custom->style.margin.top) + ", "
                        + std::to_string(custom->style.margin.bottom) + ", "
                        + std::to_string(custom->style.margin.left) + ", "
                        + std::to_string(custom->style.margin.right)
                    );
                    margin->setBackground(Color(1, 0, 0, 0.5));
                right->append(margin, Fill::Horizontal);
                Button *border = new Button(
                        "Border: "
                        + std::to_string(custom->style.border.top) + ", "
                        + std::to_string(custom->style.border.bottom) + ", "
                        + std::to_string(custom->style.border.left) + ", "
                        + std::to_string(custom->style.border.right)
                    );
                    border->setBackground(COLOR_BLACK);
                    border->setForeground(COLOR_WHITE);
                right->append(border, Fill::Horizontal);
                Button *padding = new Button(
                        "Padding: "
                        + std::to_string(custom->style.padding.top) + ", "
                        + std::to_string(custom->style.padding.bottom) + ", "
                        + std::to_string(custom->style.padding.left) + ", "
                        + std::to_string(custom->style.padding.right)
                    );
                    padding->setBackground(Color(0, 1, 0, 0.5));
                right->append(padding, Fill::Horizontal);
            Button *csir = new Button("Content Rect: ");
                csir->setBackground(Color(0, 0, 1, 0.5));
            right->append(csir, Fill::Horizontal);
        app->append(right);

        app->onResize = [&](Window *window) {
            update_rect_labels("Rect: ", csr, custom->rect);
            update_rect_labels("Content Rect: ", csir, custom->inner_rect);
        };
        app->onReady = [&](Window *window) {
            update_rect_labels("Rect: ", csr, custom->rect);
            update_rect_labels("Content Rect: ", csir, custom->inner_rect);
        };

    #ifdef TEST
        #include "../tests/headless.hpp"
    #else
        app->run();
    #endif

    return 0;
}