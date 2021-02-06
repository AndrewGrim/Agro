#include "treeview.hpp"

TestTree::TestTree(Size min_size) : Scrollable(min_size) {

}

TestTree::~TestTree() {

}

const char* TestTree::name() {
    return "TestTree";
}

void TestTree::draw(DrawingContext *dc, Rect rect) {
    this->rect = rect;
    Rect old_clip = dc->clip();
    dc->setClip(rect);
    Size virtual_size = Size(1000, 270 * 28);
    dc->fillRect(rect, Color(0.6, 1.0, 0.2));
    Point pos = automaticallyAddOrRemoveScrollBars(dc, rect, virtual_size);
    dc->fillRect(rect, Color(0.6, 0.0, 0.2));
    for (int i = 0; i < 270; i++) {
        if (pos.y + 28 > rect.y) {
            dc->fillTextAligned(
                dc->default_font,
                std::to_string(i) + ". The quick brown fox jumps over the lazy dog!",
                HorizontalAlignment::Left,
                VerticalAlignment::Center,
                Rect(pos.x, pos.y, virtual_size.w, 28),
                10,
                Color()
            );
            if (pos.y + 28 > rect.y + rect.h) {
                break;
            }
        }
        pos.y += 28;
    }
    drawScrollBars(dc, rect, virtual_size);
    dc->setClip(old_clip);
}