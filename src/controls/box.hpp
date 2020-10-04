#ifndef BOX_HPP
    #define BOX_HPP

    #include "widget.hpp"
    #include "../renderer/drawing_context.hpp"

    class Box : public Widget {
        public:
            Box(Align align_policy) {
                this->m_align_policy = align_policy;
            }

            ~Box() {}

            const char* name() {
                return this->m_name;
            }

            void draw(DrawingContext *dc, Rect<float> rect) {
                this->rect = rect;
                dc->fillRect(rect, this->bg);

                layout_children(dc, rect);
            }

            void layout_children(DrawingContext *dc, Rect<float> rect) {
                int non_expandable_widgets = 0;
                int reserved_x = 0;
                int reserved_y = 0;
                Align parent_layout = this->m_align_policy;
                for (Widget* child : this->children) {
                    Fill child_layout = child->fill_policy();
                    if (parent_layout == Align::Vertical) {
                        if (child_layout == Fill::Horizontal || child_layout == Fill::None) {
                            non_expandable_widgets += 1;
                            reserved_y += child->size_hint(dc).h;
                        }
                    } else if (parent_layout == Align::Horizontal) {
                        if (child_layout == Fill::Vertical || child_layout == Fill::None) {
                            non_expandable_widgets += 1;
                            reserved_x += child->size_hint(dc).w;
                        }
                    }
                }

                int child_count = this->children.size() - non_expandable_widgets;
                if (!child_count) child_count = 1; // Protects from division by zero
                rect.w -= reserved_x;
                rect.h -= reserved_y;
                Point<float> pos = Point<float>(rect.x, rect.y);
                switch (parent_layout) {
                    case Align::Vertical: {
                        float available_height = rect.h;
                        float expandable_height = available_height / child_count;
                        for (Widget* child : this->children) {
                            Size<float> size;
                            Size<float> child_hint = child->size_hint(dc);
                            switch (child->fill_policy()) {
                                case Fill::Both: {
                                    float height = expandable_height;
                                    if (expandable_height < child_hint.h) {
                                        height = child_hint.h;
                                        available_height -= child_hint.h;
                                        expandable_height = available_height / --child_count;
                                    }
                                    size = Size<float> { 
                                        rect.w > child_hint.w ? rect.w : child_hint.w, 
                                        height
                                    };
                                    break;
                                }
                                case Fill::Vertical: {
                                    float height = expandable_height;
                                    if (expandable_height < child_hint.h) {
                                        height = child_hint.h;
                                        available_height -= child_hint.h;
                                        expandable_height = available_height / --child_count;
                                    }
                                    size = Size<float> { 
                                        child_hint.w, 
                                        height
                                    };
                                    break;
                                }
                                case Fill::Horizontal:
                                    size = Size<float> { rect.w > child_hint.w ? rect.w : child_hint.w, child_hint.h };
                                    break;
                                case Fill::None:
                                default:
                                    size = child_hint;
                            }
                            child->draw(dc, Rect<float>(pos.x, pos.y, size.w, size.h));
                            pos.y += size.h;
                        }
                        break;
                    }
                    case Align::Horizontal: {
                        float available_width = rect.w;
                        float expandable_width = available_width / child_count;
                        for (Widget* child : this->children) {
                            Size<float> size;
                            Size<float> child_hint = child->size_hint(dc);
                            switch (child->fill_policy()) {
                                case Fill::Both: {
                                    float width = expandable_width;
                                    if (expandable_width < child_hint.w) {
                                        width = child_hint.w;
                                        available_width -= child_hint.w;
                                        expandable_width = available_width / --child_count;
                                    }
                                    size = Size<float> { 
                                        width, 
                                        rect.h
                                    };
                                    break;
                                }
                                case Fill::Vertical:
                                    size = Size<float> { child_hint.w, rect.h > child_hint.h ? rect.h : child_hint.h };
                                    break;
                                case Fill::Horizontal: {
                                    float width = expandable_width;
                                    if (expandable_width < child_hint.w) {
                                        width = child_hint.w;
                                        available_width -= child_hint.w;
                                        expandable_width = available_width / --child_count;
                                    }
                                    size = Size<float> { 
                                        width, 
                                        child_hint.h
                                    };
                                    break;
                                }
                                case Fill::None:
                                default:
                                    size = child_hint;
                            }
                            child->draw(dc, Rect<float>(pos.x, pos.y, size.w, size.h));
                            pos.x += size.w;
                        }
                        break;
                    }   
                }
            }

            Size<float> size_hint(DrawingContext *dc) {
                Size<float> size = Size<float> { 0, 0 };
                if (this->m_align_policy == Align::Horizontal) {
                    for (Widget* child : this->children) {
                        Size<float> s = child->size_hint(dc);
                        size.w += s.w;
                        if (s.h > size.h) {
                            size.h = s.h;
                        }
                    }
                } else {
                    for (Widget* child : this->children) {
                        Size<float> s = child->size_hint(dc);
                        size.h += s.h;
                        if (s.w > size.w) {
                            size.w = s.w;
                        }
                    }
                }

                return size;
            }

            Color background() {
                return this->bg;
            }

            Box* set_background(Color background) {
                this->bg = background;
                this->update();

                return this;
            }

            bool is_layout() {
                return true;
            }

        protected:
            const char *m_name = "Box";
            Color fg = Color();
            Color bg = Color(0.50f, 0.50f, 0.50f);
            Align m_align_policy;
    };
#endif