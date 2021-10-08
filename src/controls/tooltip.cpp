#include "tooltip.hpp"

Tooltip::Tooltip(std::string text) : Label(text) {
    style.widget_background = Color("#ffffaa");
    style.border.type = STYLE_ALL;
}

Tooltip::~Tooltip() {

}

const char* Tooltip::name() {
    return "Tooltip";
}
