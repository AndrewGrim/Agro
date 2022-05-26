#include "drawable.hpp"
#include "application.hpp"

Drawable::Drawable() {}

Drawable::~Drawable() {}

void Drawable::setStyle(Style style) {
    _style = style;
    layout(LAYOUT_STYLE);
}

Style& Drawable::style() {
    return _style;
}

bool Drawable::isWidget() {
    return false;
}

void Drawable::update() {
    Application::get()->currentWindow()->update();
}

void Drawable::layout(LayoutEvent event) {
    if (handleLayoutEvent(event)) { return; }
    Widget *_parent = this->parent;
    while (_parent) {
        // Early break to avoid having to go to the top of the widget tree.
        if (_parent->handleLayoutEvent(LAYOUT_CHILD)) { break; }
        _parent = _parent->parent;
    }
    update();
}

bool Drawable::handleLayoutEvent(LayoutEvent event) {
    // For the default implementation we want to fully redo the layout
    // in case any layout event comes in.
    // For other widget you might want to handle them at a more granular level.
    // Such as TextEdit only recalculating its text size when the font changes.
    if (event) {
        if (event & LAYOUT_SCALE) {
            if (_style.font.get()) { setFont(std::shared_ptr<Font>(_style.font.get()->reload(Application::get()->currentWindow()->dc->default_style.font->pixel_size))); }
        }
        // Since we already know the layout needs to be redone
        // we return true to avoid having to traverse the entire widget graph to the top.
        if (m_size_changed) { return true; }
        m_size_changed = true;
    }
    return false;
}

void Drawable::setMarginType(i32 type) {
    style().margin.type = type;
    layout(LAYOUT_MARGIN);
}

i32 Drawable::marginType() {
    return style().margin.type != STYLE_DEFAULT ? style().margin.type : Application::get()->currentWindow()->dc->default_style.margin.type;
}

void Drawable::setMarginTop(i32 top) {
    style().margin.top = top;
    layout(LAYOUT_MARGIN);
}

i32 Drawable::marginTop() {
    return style().margin.top != STYLE_DEFAULT ? style().margin.top : Application::get()->currentWindow()->dc->default_style.margin.top;
}

void Drawable::setMarginBottom(i32 bottom) {
    style().margin.bottom = bottom;
    layout(LAYOUT_MARGIN);
}

i32 Drawable::marginBottom() {
    return style().margin.bottom != STYLE_DEFAULT ? style().margin.bottom : Application::get()->currentWindow()->dc->default_style.margin.bottom;
}

void Drawable::setMarginLeft(i32 left) {
    style().margin.left = left;
    layout(LAYOUT_MARGIN);
}

i32 Drawable::marginLeft() {
    return style().margin.left != STYLE_DEFAULT ? style().margin.left : Application::get()->currentWindow()->dc->default_style.margin.left;
}

void Drawable::setMarginRight(i32 right) {
    style().margin.right = right;
    layout(LAYOUT_MARGIN);
}

i32 Drawable::marginRight() {
    return style().margin.right != STYLE_DEFAULT ? style().margin.right : Application::get()->currentWindow()->dc->default_style.margin.right;
}

void Drawable::setBorderType(i32 type) {
    style().border.type = type;
    layout(LAYOUT_BORDER);
}

i32 Drawable::borderType() {
    return style().border.type != STYLE_DEFAULT ? style().border.type : Application::get()->currentWindow()->dc->default_style.border.type;
}

void Drawable::setBorderTop(i32 top) {
    style().border.top = top;
    layout(LAYOUT_BORDER);
}

i32 Drawable::borderTop() {
    return style().border.top != STYLE_DEFAULT ? style().border.top : Application::get()->currentWindow()->dc->default_style.border.top;
}

void Drawable::setBorderBottom(i32 bottom) {
    style().border.bottom = bottom;
    layout(LAYOUT_BORDER);
}

i32 Drawable::borderBottom() {
    return style().border.bottom != STYLE_DEFAULT ? style().border.bottom : Application::get()->currentWindow()->dc->default_style.border.bottom;
}

void Drawable::setBorderLeft(i32 left) {
    style().border.left = left;
    layout(LAYOUT_BORDER);
}

i32 Drawable::borderLeft() {
    return style().border.left != STYLE_DEFAULT ? style().border.left : Application::get()->currentWindow()->dc->default_style.border.left;
}

void Drawable::setBorderRight(i32 right) {
    style().border.right = right;
    layout(LAYOUT_BORDER);
}

i32 Drawable::borderRight() {
    return style().border.right != STYLE_DEFAULT ? style().border.right : Application::get()->currentWindow()->dc->default_style.border.right;
}

void Drawable::setBorderColorTop(Color top) {
    style().border_color.top = top;
    update();
}

Color Drawable::borderColorTop() {
    return style().border_color.top ? style().border_color.top : Application::get()->currentWindow()->dc->default_style.border_color.top;
}

void Drawable::setBorderColorBottom(Color bottom) {
    style().border_color.bottom = bottom;
    update();
}

Color Drawable::borderColorBottom() {
    return style().border_color.bottom ? style().border_color.bottom : Application::get()->currentWindow()->dc->default_style.border_color.bottom;
}

void Drawable::setBorderColorLeft(Color left) {
    style().border_color.left = left;
    update();
}

Color Drawable::borderColorLeft() {
    return style().border_color.left ? style().border_color.left : Application::get()->currentWindow()->dc->default_style.border_color.left;
}

void Drawable::setBorderColorRight(Color right) {
    style().border_color.right = right;
    update();
}

Color Drawable::borderColorRight() {
    return style().border_color.right ? style().border_color.right : Application::get()->currentWindow()->dc->default_style.border_color.right;
}

void Drawable::setPaddingType(i32 type) {
    style().padding.type = type;
    layout(LAYOUT_PADDING);
}

i32 Drawable::paddingType() {
    return style().padding.type != STYLE_DEFAULT ? style().padding.type : Application::get()->currentWindow()->dc->default_style.padding.type;
}

void Drawable::setPaddingTop(i32 top) {
    style().padding.top = top;
    layout(LAYOUT_PADDING);
}

i32 Drawable::paddingTop() {
    return style().padding.top != STYLE_DEFAULT ? style().padding.top : Application::get()->currentWindow()->dc->default_style.padding.top;
}

void Drawable::setPaddingBottom(i32 bottom) {
    style().padding.bottom = bottom;
    layout(LAYOUT_PADDING);
}

i32 Drawable::paddingBottom() {
    return style().padding.bottom != STYLE_DEFAULT ? style().padding.bottom : Application::get()->currentWindow()->dc->default_style.padding.bottom;
}

void Drawable::setPaddingLeft(i32 left) {
    style().padding.left = left;
    layout(LAYOUT_PADDING);
}

i32 Drawable::paddingLeft() {
    return style().padding.left != STYLE_DEFAULT ? style().padding.left : Application::get()->currentWindow()->dc->default_style.padding.left;
}

void Drawable::setPaddingRight(i32 right) {
    style().padding.right = right;
    layout(LAYOUT_PADDING);
}

i32 Drawable::paddingRight() {
    return style().padding.right != STYLE_DEFAULT ? style().padding.right : Application::get()->currentWindow()->dc->default_style.padding.right;
}

void Drawable::setMargin(Style::Margin margin) {
    style().margin = margin;
    layout(LAYOUT_MARGIN);
}

Style::Margin Drawable::margin() {
    return style().margin.type == STYLE_DEFAULT &&
           style().margin.top == STYLE_DEFAULT &&
           style().margin.bottom == STYLE_DEFAULT &&
           style().margin.left == STYLE_DEFAULT &&
           style().margin.right == STYLE_DEFAULT
    ? Application::get()->currentWindow()->dc->default_style.margin : style().margin;
}

void Drawable::setBorder(Style::Border border) {
    style().border = border;
    layout(LAYOUT_BORDER);
}

Style::Border Drawable::border() {
    return style().border.type == STYLE_DEFAULT &&
           style().border.top == STYLE_DEFAULT &&
           style().border.bottom == STYLE_DEFAULT &&
           style().border.left == STYLE_DEFAULT &&
           style().border.right == STYLE_DEFAULT
    ? Application::get()->currentWindow()->dc->default_style.border : style().border;
}

void Drawable::setPadding(Style::Padding padding) {
    style().padding = padding;
    layout(LAYOUT_PADDING);
}

Style::Padding Drawable::padding() {
    return style().padding.type == STYLE_DEFAULT &&
           style().padding.top == STYLE_DEFAULT &&
           style().padding.bottom == STYLE_DEFAULT &&
           style().padding.left == STYLE_DEFAULT &&
           style().padding.right == STYLE_DEFAULT
    ? Application::get()->currentWindow()->dc->default_style.padding : style().padding;
}

void Drawable::setFont(std::shared_ptr<Font> font) {
    style().font = font;
    layout(LAYOUT_FONT);
}

std::shared_ptr<Font> Drawable::font() {
    return style().font ? style().font : Application::get()->currentWindow()->dc->default_style.font;
}

void Drawable::setWindowBackgroundColor(Color window_background_color) {
    style().window_background_color = window_background_color;
    update();
}

Color Drawable::windowBackgroundColor() {
    return style().window_background_color ? style().window_background_color : Application::get()->currentWindow()->dc->default_style.window_background_color;
}

void Drawable::setWidgetBackgroundColor(Color widget_background_color) {
    style().widget_background_color = widget_background_color;
    update();
}

Color Drawable::widgetBackgroundColor() {
    return style().widget_background_color ? style().widget_background_color : Application::get()->currentWindow()->dc->default_style.widget_background_color;
}

void Drawable::setAccentWidgetBackgroundColor(Color accent_widget_background_color) {
    style().accent_widget_background_color = accent_widget_background_color;
    update();
}

Color Drawable::accentWidgetBackgroundColor() {
    return style().accent_widget_background_color ? style().accent_widget_background_color : Application::get()->currentWindow()->dc->default_style.accent_widget_background_color;
}

void Drawable::setTextForegroundColor(Color text_foreground_color) {
    style().text_foreground_color = text_foreground_color;
    update();
}

Color Drawable::textForegroundColor() {
    return style().text_foreground_color ? style().text_foreground_color : Application::get()->currentWindow()->dc->default_style.text_foreground_color;
}

void Drawable::setTextBackgroundColor(Color text_background_color) {
    style().text_background_color = text_background_color;
    update();
}

Color Drawable::textBackgroundColor() {
    return style().text_background_color ? style().text_background_color : Application::get()->currentWindow()->dc->default_style.text_background_color;
}

void Drawable::setTextSelectedColor(Color text_selected_color) {
    style().text_selected_color = text_selected_color;
    update();
}

Color Drawable::textSelectedColor() {
    return style().text_selected_color ? style().text_selected_color : Application::get()->currentWindow()->dc->default_style.text_selected_color;
}

void Drawable::setTextDisabledColor(Color text_disabled_color) {
    style().text_disabled_color = text_disabled_color;
    update();
}

Color Drawable::textDisabledColor() {
    return style().text_disabled_color ? style().text_disabled_color : Application::get()->currentWindow()->dc->default_style.text_disabled_color;
}

void Drawable::setHoveredBackgroundColor(Color hovered_background_color) {
    style().hovered_background_color = hovered_background_color;
    update();
}

Color Drawable::hoveredBackgroundColor() {
    return style().hovered_background_color ? style().hovered_background_color : Application::get()->currentWindow()->dc->default_style.hovered_background_color;
}

void Drawable::setPressedBackgroundColor(Color pressed_background_color) {
    style().pressed_background_color = pressed_background_color;
    update();
}

Color Drawable::pressedBackgroundColor() {
    return style().pressed_background_color ? style().pressed_background_color : Application::get()->currentWindow()->dc->default_style.pressed_background_color;
}

void Drawable::setAccentHoveredBackgroundColor(Color accent_hovered_background_color) {
    style().accent_hovered_background_color = accent_hovered_background_color;
    update();
}

Color Drawable::accentHoveredBackgroundColor() {
    return style().accent_hovered_background_color ? style().accent_hovered_background_color : Application::get()->currentWindow()->dc->default_style.accent_hovered_background_color;
}

void Drawable::setAccentPressedBackgroundColor(Color accent_pressed_background_color) {
    style().accent_pressed_background_color = accent_pressed_background_color;
    update();
}

Color Drawable::accentPressedBackgroundColor() {
    return style().accent_pressed_background_color ? style().accent_pressed_background_color : Application::get()->currentWindow()->dc->default_style.accent_pressed_background_color;
}

void Drawable::setIconForegroundColor(Color icon_foreground_color) {
    style().icon_foreground_color = icon_foreground_color;
    update();
}

Color Drawable::iconForegroundColor() {
    return style().icon_foreground_color ? style().icon_foreground_color : Application::get()->currentWindow()->dc->default_style.icon_foreground_color;
}

void Drawable::setBorderBackgroundColor(Color border_background_color) {
    style().border_background_color = border_background_color;
    update();
}

Color Drawable::borderBackgroundColor() {
    return style().border_background_color ? style().border_background_color : Application::get()->currentWindow()->dc->default_style.border_background_color;
}
