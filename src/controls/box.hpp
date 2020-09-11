#ifndef BOX_HPP
    #define BOX_HPP

    #include "widget.hpp"

    class Box : public Widget {
        public:
            Box(Align align_policy) {
                this->m_align_policy = align_policy;
            }

            ~Box() {}

            const char* name() {
                return this->m_name;
            }

            void draw(SDL_Renderer* ren, Rect rect) {
                this->rect = rect;
                SDL_SetRenderDrawColor(ren, this->bg.red, this->bg.green, this->bg.blue, this->bg.alpha);
                SDL_RenderFillRect(ren,  rect.to_SDL_Rect());

                layout_children(ren, rect);
            }

            void layout_children(SDL_Renderer *ren, Rect rect) {
                int non_expandable_widgets = 0;
                int reserved_x = 0;
                int reserved_y = 0;
                Align parent_layout = this->m_align_policy;
                for (Widget* child : this->children) {
                    Fill child_layout = child->fill_policy();
                    if (child_layout == Fill::Horizontal && parent_layout == Align::Vertical) {
                        non_expandable_widgets += 1;
                        reserved_y += child->size_hint().height;
                    } else if (child_layout == Fill::Vertical && parent_layout == Align::Horizontal) {
                        non_expandable_widgets += 1;
                        reserved_x += child->size_hint().width;
                    } else if (child_layout == Fill::None) {
                        non_expandable_widgets += 1;
                        if (parent_layout == Align::Horizontal) reserved_x += child->size_hint().width;
                        else if (parent_layout == Align::Vertical) reserved_y += child->size_hint().height;
                    }
                }

                int child_count = this->children.size() - non_expandable_widgets;
                rect.w -= reserved_x;
                rect.h -= reserved_y;
                Point pos = Point { rect.x, rect.y };
                switch (this->m_align_policy) {
                    case Align::Vertical:
                        for (Widget* child : this->children) {
                            Size size;
                            switch (child->fill_policy()) {
                                case Fill::Both:
                                    size = Size { rect.w, rect.h / child_count };
                                    break;
                                case Fill::Vertical:
                                    size = Size { child->size_hint().width, rect.h / child_count };
                                    break;
                                case Fill::Horizontal:
                                    size = Size { rect.w, child->size_hint().height };
                                    break;
                                case Fill::None:
                                default:
                                    size = child->size_hint();
                            }
                            child->draw(ren, Rect { pos.x, pos.y, size.width, size.height });
                            pos.y += size.height;
                        }
                        break;
                    case Align::Horizontal:
                        for (Widget* child : this->children) {
                            Size size;
                            switch (child->fill_policy()) {
                                case Fill::Both:
                                    size = Size { rect.w / child_count, rect.h };
                                    break;
                                case Fill::Vertical:
                                    size = Size { child->size_hint().width, rect.h };
                                    break;
                                case Fill::Horizontal:
                                    size = Size { rect.w / child_count, child->size_hint().height };
                                    break;
                                case Fill::None:
                                default:
                                    size = child->size_hint();
                            }
                            child->draw(ren, Rect { pos.x, pos.y, size.width, size.height });
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

            bool is_layout() {
                return true;
            }

        private:
            const char *m_name = "Box";
            Color fg = {0, 0, 0, 255};
            Color bg = {220, 220, 220, 255};
            Align m_align_policy;
    };
#endif