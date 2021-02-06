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
    Size children_size = Size();
    for (Widget *child : children) {
        Size s = child->sizeHint(dc);
        children_size.w += s.w;
        if (s.h > children_size.h) {
            children_size.h = s.h;
        }
    }
    Size virtual_size = Size(children_size.w, 270 * 28);
    Point pos = automaticallyAddOrRemoveScrollBars(dc, rect, virtual_size);
    dc->fillRect(rect, Color(0.6, 0.0, 0.2));
    Rect local = rect;
    float local_pos_x = pos.x;
    dc->setClip(local);
    for (Widget *child : children) {
        Size s = child->sizeHint(dc);
        child->draw(dc, Rect(local_pos_x, local.y, s.w, children_size.h));
        local_pos_x += s.w;
    }
    local.y += children_size.h;
    local.h -= children_size.h;
    dc->setClip(Rect(rect.x, local.y, rect.w, local.h));
    // TODO so at this point we need to figure out drawing the rows themselves
    // which shouldnt be that difficult but for that we need to have a usable tree
    // then we just iterate over the tree and draw visible nodes
    for (int i = 0; i < 270; i++) {
        if (pos.y + 28 > local.y) {
            dc->fillTextAligned(
                dc->default_font,
                std::to_string(i) + ". The quick brown fox jumps over the lazy dog!",
                HorizontalAlignment::Left,
                VerticalAlignment::Center,
                Rect(pos.x, pos.y, virtual_size.w, 28),
                10,
                Color()
            );
            if (pos.y + 28 > local.y + local.h) {
                break;
            }
        }
        pos.y += 28;
    }
    dc->setClip(old_clip);
    drawScrollBars(dc, rect, virtual_size);
}