#include <cassert>

#include "../src/application.hpp"
#include "../src/common/color.hpp"

int main(int argc, char **argv) {
    assert(Color(nullptr) == Color(0, 0, 0, 255));
    assert(Color("") == Color(0, 0, 0, 255));
    assert(Color("!") == Color(0, 0, 0, 255));
    assert(Color("f") == Color(240, 0, 0, 255));
    assert(Color("ff") == Color(255, 0, 0, 255));
    assert(Color("ff9") == Color(255, 144, 0, 255));
    assert(Color("ff9b") == Color(255, 155, 0, 255));
    assert(Color("ff9b0") == Color(255, 155, 0, 255));
    assert(Color("ff9b00") == Color(255, 155, 0, 255));
    assert(Color("ff9b00f") == Color(255, 155, 0, 240));
    assert(Color("ff9b00ff") == Color(255, 155, 0, 255));
    assert(Color("ff9b00ff123uisndof") == Color(255, 155, 0, 255));
    assert(Color("!#@({},.") == Color(0, 0, 0, 0));
    assert(Color("#") == Color(0, 0, 0, 255));
    assert(Color("#!") == Color(0, 0, 0, 255));
    assert(Color("#f") == Color(240, 0, 0, 255));
    assert(Color("#ff") == Color(255, 0, 0, 255));
    assert(Color("#ff9") == Color(255, 144, 0, 255));
    assert(Color("#ff9b") == Color(255, 155, 0, 255));
    assert(Color("#ff9b0") == Color(255, 155, 0, 255));
    assert(Color("#ff9b00") == Color(255, 155, 0, 255));
    assert(Color("#ff9b00f") == Color(255, 155, 0, 240));
    assert(Color("#ff9b00ff") == Color(255, 155, 0, 255));
    assert(Color("#ff9b00ff123uisndof") == Color(255, 155, 0, 255));
    assert(Color("#!#@({},.") == Color(0, 0, 0, 0));

    return 0;
}
