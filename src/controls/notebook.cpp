#include "notebook.hpp"

NoteBook::NoteBook() {

}

NoteBook::~NoteBook() {

}

void NoteBook::draw(DrawingContext *dc, Rect rect) {

}

const char* NoteBook::name() {

}

Size NoteBook::sizeHint(DrawingContext *dc) {

}

NoteBook* NoteBook::appendTab(Widget *root, std::string text, Image *icon = nullptr) {

}

NoteBook* NoteBook::insertTab(Widget *root, std::string text, Image *icon = nullptr) {

}

NoteBook* NoteBook::removeTab(int index) {

}

NoteBook* NoteBook::setRootWidget(Widget *root) {

}

NoteBook* NoteBook::setTabText(std::string text) {

}

NoteBook* NoteBook::setTabIcon(Image *icon) {

}

int NoteBook::currentTab() {

}

NoteBook* NoteBook::setCurrentTab(int index) {

}
