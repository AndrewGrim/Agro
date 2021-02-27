#include "style.hpp"

Style::Style() {

}

Style::Style(StyleOptions default_value) {
    border = default_value;
    padding = default_value;
    margin = default_value;
}

Style::~Style() {

}