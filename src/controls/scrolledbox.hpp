#ifndef SCROLLEDBOX_HPP
    #define SCROLLEDBOX_HPP

    #include "widget.hpp"
    #include "box.hpp"
    #include "scrollbar.hpp"
    #include "../renderer/drawing_context.hpp"

    class ScrolledBox : public Box {
        public:
            ScrollBar *m_vertical_scrollbar = nullptr;
            ScrollBar *m_horizontal_scrollbar = nullptr;

            ScrolledBox(Align align_policy) : Box(align_policy) {
            }

            ~ScrolledBox() {}

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
                Size<float> total_children_size;
                Align parent_layout = this->m_align_policy;
                for (Widget* child : this->children) {
                    Size<float> child_size = child->size_hint(dc);
                    Fill child_layout = child->fill_policy();
                    if (parent_layout == Align::Vertical) {
                        if (child_layout == Fill::Horizontal || child_layout == Fill::None) {
                            non_expandable_widgets += 1;
                        }
                    } else if (parent_layout == Align::Horizontal) {
                        if (child_layout == Fill::Vertical || child_layout == Fill::None) {
                            non_expandable_widgets += 1;
                        }
                    }
                    total_children_size.w += child_size.w;
                    total_children_size.h += child_size.h;
                }

                float content_x = rect.x;
                float content_y = rect.y;
                if (rect.w < total_children_size.w) {
                    this->add_scrollbar(Align::Horizontal);
                    content_x -= this->m_horizontal_scrollbar->m_slider->m_value * (total_children_size.w - rect.w);
                } else {
                    this->remove_scrollbar(Align::Horizontal);
                }
                if (rect.h < total_children_size.h) {
                    this->add_scrollbar(Align::Vertical);
                    content_y -= this->m_vertical_scrollbar->m_slider->m_value * (total_children_size.h - rect.h);
                } else {
                    this->remove_scrollbar(Align::Vertical);
                }

                int child_count = this->children.size() - non_expandable_widgets;
                if (!child_count) child_count = 1; // Protects from division by zero
                Point<float> pos = Point<float>(content_x, content_y);
                switch (parent_layout) {
                    case Align::Vertical: {
                        float available_height = rect.h;
                        float expandable_height = (available_height - total_children_size.h) / child_count;
                        if (expandable_height < 0) expandable_height = 0;
                        if (this->has_scrollbar(Align::Vertical)) rect.w -= m_vertical_scrollbar->size_hint(dc).w;
                        // TODO we could check if widget is visible and if not then dont draw it
                        for (Widget* child : this->children) {
                            Size<float> size;
                            Size<float> child_hint = child->size_hint(dc);
                            switch (child->fill_policy()) {
                                case Fill::Both: {
                                    size = Size<float> { 
                                        rect.w > child_hint.w ? rect.w : child_hint.w, 
                                        child_hint.h + expandable_height
                                    };
                                    break;
                                }
                                case Fill::Vertical: {
                                    size = Size<float> { 
                                        child_hint.w, 
                                        child_hint.h + expandable_height
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
                        if (m_vertical_scrollbar) {
                            Size<float> size = m_vertical_scrollbar->size_hint(dc);
                            m_vertical_scrollbar->m_slider->m_slider_button_size = rect.h * ((rect.h - size.h / 2) / total_children_size.h);
                            m_vertical_scrollbar->draw(dc, Rect<float>(rect.x + rect.w, rect.y, size.w, rect.h));
                        }
                        break;
                    }
                    case Align::Horizontal: {
                        float available_width = rect.w;
                        float expandable_width = (available_width - total_children_size.w) / child_count;
                        if (expandable_width < 0) expandable_width = 0;
                        for (Widget* child : this->children) {
                            Size<float> size;
                            Size<float> child_hint = child->size_hint(dc);
                            switch (child->fill_policy()) {
                                case Fill::Both: {
                                    size = Size<float> { 
                                        child_hint.w + expandable_width, 
                                        rect.h
                                    };
                                    break;
                                }
                                case Fill::Vertical:
                                    size = Size<float> { child_hint.w, rect.h > child_hint.h ? rect.h : child_hint.h };
                                    break;
                                case Fill::Horizontal: {
                                    size = Size<float> { 
                                        child_hint.w + expandable_width, 
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
                            if (m_horizontal_scrollbar) {
                                Size<float> size = m_horizontal_scrollbar->size_hint(dc);
                                m_horizontal_scrollbar->m_slider->m_slider_button_size = rect.w * ((rect.w - size.w / 2) / total_children_size.w);
                                m_horizontal_scrollbar->draw(dc, Rect<float>(rect.x, rect.y + rect.h, rect.w, size.h));
                            }
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
                    if (this->has_scrollbar(Align::Horizontal)) {
                        size.h += m_horizontal_scrollbar->size_hint(dc).h;
                    }
                } else {
                    for (Widget* child : this->children) {
                        Size<float> s = child->size_hint(dc);
                        size.h += s.h;
                        if (s.w > size.w) {
                            size.w = s.w;
                        }
                    }
                    if (this->has_scrollbar(Align::Vertical)) {
                        size.w += m_vertical_scrollbar->size_hint(dc).w;
                    }
                }

                return size;
            }

            void add_scrollbar(Align alignment) {
                if (alignment == Align::Horizontal) {
                    if (!this->m_horizontal_scrollbar) {
                        this->m_horizontal_scrollbar = new ScrollBar(alignment, "  ");
                        this->m_horizontal_scrollbar->m_slider->m_value = 0.0;
                    }
                    if (this->m_horizontal_scrollbar) {
                        this->m_horizontal_scrollbar->m_app = this->m_app;
                        for (Widget *child : this->m_horizontal_scrollbar->children) {
                            child->m_app = this->m_app;
                            child->attach_app(this->m_app);
                        }
                    }
                }
                else {
                    if (!this->m_vertical_scrollbar) {
                        this->m_vertical_scrollbar = new ScrollBar(alignment, "  ");
                        this->m_vertical_scrollbar->m_slider->m_value = 0.0;
                    }
                    if (this->m_vertical_scrollbar) {
                        this->m_vertical_scrollbar->m_app = this->m_app;
                        for (Widget *child : this->m_vertical_scrollbar->children) {
                            child->m_app = this->m_app;
                            child->attach_app(this->m_app);
                        }
                    }
                }
            }

            void remove_scrollbar(Align alignment) {
                if (alignment == Align::Horizontal) {
                    if (this->m_horizontal_scrollbar) {
                        delete this->m_horizontal_scrollbar;
                        this->m_horizontal_scrollbar = nullptr;
                    }
                } else {
                    if (this->m_vertical_scrollbar) {
                        delete this->m_vertical_scrollbar;
                        this->m_vertical_scrollbar = nullptr;
                    }
                }
            }

            bool has_scrollbar(Align alignment) {
                if (alignment == Align::Horizontal) {
                    if (m_horizontal_scrollbar) return true;
                    else return false;
                } else {
                    if (m_vertical_scrollbar) return true;
                    else return false;
                }
            }

            bool is_scrollable() {
                return true;
            }

        protected:
            const char *m_name = "ScrolledBox";
    };
#endif
