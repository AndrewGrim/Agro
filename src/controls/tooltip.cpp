#include "tooltip.hpp"

Tooltip::Tooltip(std::string text) : Label(text) {
    style.widget_background = Color("#ffff88");
    style.text_foreground = COLOR_BLACK;
    style.border.type = STYLE_ALL;
    style.border.color_top = COLOR_BLACK;
    style.border.color_bottom = COLOR_BLACK;
    style.border.color_left = COLOR_BLACK;
    style.border.color_right = COLOR_BLACK;
}

Tooltip::~Tooltip() {

}

const char* Tooltip::name() {
    return "Tooltip";
}
