#include "util.hpp"
#include "custom_style.hpp"
#include "application.hpp"
#include "controls/box.hpp"
#include "controls/button.hpp"

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
    Application *app = new Application("Application", Size(600, 400));
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

        app->onResize = [&](Application *app) {
            update_rect_labels("Rect: ", csr, custom->rect);
            update_rect_labels("Content Rect: ", csir, custom->inner_rect);
        };
        app->onReady = [&](Application *app) {
            update_rect_labels("Rect: ", csr, custom->rect);
            update_rect_labels("Content Rect: ", csir, custom->inner_rect);
        };
    app->run();

    return 0;
}