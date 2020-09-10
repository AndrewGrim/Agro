#ifndef BOX_HPP
    #define BOX_HPP

    #include "widget.hpp"

    class Box : public Widget {
        public:
            GuiLayout layout_direction;

            Box(GuiLayout layout_direction) {
                this->layout_direction = layout_direction;
                this->expand = expand;
            }

            ~Box() {}

            const char* name() {
                return this->m_name;
            }

            void draw(SDL_Renderer* ren, Rect rect) {
                SDL_SetRenderDrawColor(ren, this->bg.red, this->bg.green, this->bg.blue, this->bg.alpha);
                SDL_RenderFillRect(ren,  rect.to_SDL_Rect());
                printf("x: %d, y: %d, w: %d, h: %d\n", rect.x, rect.y, rect.w, rect.h);

                layout_children(ren, rect);
            }

            void layout_children(SDL_Renderer *ren, Rect rect) {
                int non_expandable_widgets = 0;
                int reserved_x = 0;
                int reserved_y = 0;
                GuiLayout parent_layout = this->layout_direction;
                for (Widget* child : this->children) {
                    GuiLayout child_layout = child->get_expand();
                    if (child_layout == GUI_LAYOUT_EXPAND_HORIZONTAL && parent_layout == GUI_LAYOUT_VERTICAL) {
                        non_expandable_widgets += 1;
                        reserved_y += child->size_hint().height;
                    } else if (child_layout == GUI_LAYOUT_EXPAND_VERTICAL && parent_layout == GUI_LAYOUT_HORIZONTAL) {
                        non_expandable_widgets += 1;
                        reserved_x += child->size_hint().width;
                    } else if (child_layout == GUI_LAYOUT_EXPAND_NONE) {
                        non_expandable_widgets += 1;
                        if (parent_layout == GUI_LAYOUT_HORIZONTAL) reserved_x += child->size_hint().width;
                        else if (parent_layout == GUI_LAYOUT_VERTICAL) reserved_y += child->size_hint().height;
                    }
                }

                int child_count = this->children.size() - non_expandable_widgets;
                rect.w -= reserved_x;
                rect.h -= reserved_y;
                Point pos = Point { rect.x, rect.y };
                switch (this->layout_direction) {
                    case GUI_LAYOUT_VERTICAL:
                        for (Widget* child : this->children) {
                            Size size;
                            switch (child->get_expand()) {
                                case GUI_LAYOUT_EXPAND_BOTH:
                                    size = Size { rect.w, rect.h / child_count };
                                    break;
                                case GUI_LAYOUT_EXPAND_VERTICAL:
                                    size = Size { child->size_hint().width, rect.h / child_count };
                                    break;
                                case GUI_LAYOUT_EXPAND_HORIZONTAL:
                                    size = Size { rect.w, child->size_hint().height };
                                    break;
                                case GUI_LAYOUT_EXPAND_NONE:
                                default:
                                    size = child->size_hint();
                            }
                            child->draw(ren, Rect { pos.x, pos.y, size.width, size.height });
                            printf("x: %d, y: %d, w: %d, h: %d\n", pos.x, pos.y, size.width, size.height);
                            pos.y += size.height;
                        }
                        break;
                    case GUI_LAYOUT_HORIZONTAL:
                        for (Widget* child : this->children) {
                            Size size;
                            switch (child->get_expand()) {
                                case GUI_LAYOUT_EXPAND_BOTH:
                                    size = Size { rect.w / child_count, rect.h };
                                    break;
                                case GUI_LAYOUT_EXPAND_VERTICAL:
                                    size = Size { child->size_hint().width, rect.h };
                                    break;
                                case GUI_LAYOUT_EXPAND_HORIZONTAL:
                                    size = Size { rect.w / child_count, child->size_hint().height };
                                    break;
                                case GUI_LAYOUT_EXPAND_NONE:
                                default:
                                    size = child->size_hint();
                            }
                            child->draw(ren, Rect { pos.x, pos.y, size.width, size.height });
                            printf("x: %d, y: %d, w: %d, h: %d\n", pos.x, pos.y, size.width, size.height);
                            pos.x += size.width;
                        }
                        break;
                }
            }

            Size size_hint() {
                Size size = Size { 0, 0 };
                for (Widget* child : this->children) {
                    Size s = child->size_hint();
                    size.width += s.width;
                    if (s.height > size.height) {
                        size.height = s.height;
                    }
                }

                return size;
            }

            Color background() {
                return this->bg;
            }

            Box* set_background(Color background) {
                this->bg = background;

                return this;
            }

        private:
            const char *m_name = "Box";
            Color fg = {0, 0, 0, 255};
            Color bg = {220, 220, 220, 255};
    };
#endif