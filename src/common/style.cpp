#include "style.hpp"

Style::Style() {

}

Style::Style(StyleOptions type, int size) {
    border = type;
    padding = type;
    margin = type;

    border_size = size;
    padding_size = size;
    margin_size = size;
}

Style::~Style() {

}