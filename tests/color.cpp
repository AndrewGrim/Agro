#include <cassert>

#include "../src/application.hpp"
#include "../src/common/color.hpp"

int main(int argc, char **argv) {
    assert(Color("") == Color::fromInt(0, 0, 0, 255));
    assert(Color("!") == Color::fromInt(0, 0, 0, 255));
    assert(Color("f") == Color::fromInt(240, 0, 0, 255));
    assert(Color("ff") == Color::fromInt(255, 0, 0, 255));
    assert(Color("ff9") == Color::fromInt(255, 144, 0, 255));
    assert(Color("ff9b") == Color::fromInt(255, 155, 0, 255));
    assert(Color("ff9b0") == Color::fromInt(255, 155, 0, 255));
    assert(Color("ff9b00") == Color::fromInt(255, 155, 0, 255));
    assert(Color("ff9b00f") == Color::fromInt(255, 155, 0, 240));
    assert(Color("ff9b00ff") == Color::fromInt(255, 155, 0, 255));
    assert(Color("ff9b00ff123uisndof") == Color::fromInt(255, 155, 0, 255));
    assert(Color("#") == Color::fromInt(0, 0, 0, 255));
    assert(Color("#!") == Color::fromInt(0, 0, 0, 255));
    assert(Color("#f") == Color::fromInt(240, 0, 0, 255));
    assert(Color("#ff") == Color::fromInt(255, 0, 0, 255));
    assert(Color("#ff9") == Color::fromInt(255, 144, 0, 255));
    assert(Color("#ff9b") == Color::fromInt(255, 155, 0, 255));
    assert(Color("#ff9b0") == Color::fromInt(255, 155, 0, 255));
    assert(Color("#ff9b00") == Color::fromInt(255, 155, 0, 255));
    assert(Color("#ff9b00f") == Color::fromInt(255, 155, 0, 240));
    assert(Color("#ff9b00ff") == Color::fromInt(255, 155, 0, 255));
    assert(Color("#ff9b00ff123uisndof") == Color::fromInt(255, 155, 0, 255));

    return 0;
}