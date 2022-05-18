#include "tooltip.hpp"

Tooltip::Tooltip(String text) : Label(text) {
    setWidgetBackgroundColor(Color("#ffff88"));
    setTextForegroundColor(COLOR_BLACK);
    setBorderType(STYLE_ALL);
    setBorderColorTop(COLOR_BLACK);
    setBorderColorBottom(COLOR_BLACK);
    setBorderColorLeft(COLOR_BLACK);
    setBorderColorRight(COLOR_BLACK);
}

Tooltip::~Tooltip() {

}

const char* Tooltip::name() {
    return "Tooltip";
}
