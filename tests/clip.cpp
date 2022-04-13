#include <cassert>

#include "../src/application.hpp"
#include "../src/common/rect.hpp"
#include "../src/util.hpp"

void lesser_x_or_y() {
    auto r = Rect(20, 20, 30, 30);
    assert(r.clipTo(Rect(40, 40, 20, 20)) == Rect(40, 40, 10, 10));
}

void greater_x_or_y() {
    auto r = Rect(50, 50, 30, 30);
    assert(r.clipTo(Rect(40, 40, 20, 20)) == Rect(50, 50, 10, 10));
}

void lesser_w_or_h() {
    auto r = Rect(40, 40, 10, 10);
    assert(r.clipTo(Rect(40, 40, 20, 20)) == Rect(40, 40, 10, 10));
}

void greater_w_or_h() {
    auto r = Rect(40, 40, 30, 30);
    assert(r.clipTo(Rect(40, 40, 20, 20)) == Rect(40, 40, 20, 20));
}

void lesser_no_overlap() {
    auto r = Rect(0, 0, 40, 40);
    assert(r.clipTo(Rect(40, 40, 20, 20)) == Rect(40, 40, 0, 0));
}

void greater_no_overlap() {
    auto r = Rect(70, 70, 40, 40);
    assert(r.clipTo(Rect(40, 40, 20, 20)) == Rect(70, 70, 0, 0));
}

void within() {
    auto r = Rect(2, 850, 150, 24);
    assert(r.clipTo(Rect(0, 540, 2000, 500)) == Rect(2, 850, 150, 24));
}

int main(int argc, char **argv) {
    lesser_x_or_y();
    greater_x_or_y();
    lesser_w_or_h();
    greater_w_or_h();
    lesser_no_overlap();
    greater_no_overlap();
    within();

    return 0;
}
