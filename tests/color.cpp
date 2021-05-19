#include "../src/application.hpp"
#include "../src/common/color.hpp"

int main(int argc, char **argv) {
    Color c = Color(1.0f, 0.7f, 0.3f, 0.05f);
        c = Color::fromInt(255, 150, 50, 17);
        c = Color::fromInt(0xff, 0x96, 0x32, 0x11);
        c = Color("#ff");
        c = Color("#ff96");
        c = Color("#ff9632");
        c = Color("#ff963211");
        c = Color("ff963211");

    return 0;
}