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
                Size<float> max_children_size;
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
                    if (child_size.w > max_children_size.w) max_children_size.w = child_size.w;
                    if (child_size.h > max_children_size.h) max_children_size.h = child_size.h;
                }

                float content_x = rect.x;
                float content_y = rect.y;
                if (this->m_align_policy == Align::Horizontal) {
                    if (rect.w < total_children_size.w) {
                        this->add_scrollbar(Align::Horizontal);
                    } else {
                        this->remove_scrollbar(Align::Horizontal);
                    }
                    if (rect.h < max_children_size.h) {
                        this->add_scrollbar(Align::Vertical);
                    } else {
                        this->remove_scrollbar(Align::Vertical);
                    }
                    if (this->has_scrollbar(Align::Vertical)) {
                        float vscroll = 0;
                        if (this->has_scrollbar(Align::Horizontal)) vscroll += this->m_horizontal_scrollbar->size_hint(dc).h;
                        content_y -= this->m_vertical_scrollbar->m_slider->m_value * ((max_children_size.h + vscroll) - rect.h);
                    }
                    if (this->has_scrollbar(Align::Horizontal)) {
                        float hscroll = 0;
                        if (this->has_scrollbar(Align::Vertical)) hscroll += this->m_vertical_scrollbar->size_hint(dc).w;
                        content_x -= this->m_horizontal_scrollbar->m_slider->m_value * ((total_children_size.w + hscroll) - rect.w);
                    }
                } else {
                    if (rect.h < total_children_size.h) {
                        this->add_scrollbar(Align::Vertical);
                    } else {
                        this->remove_scrollbar(Align::Vertical);
                    }
                    if (rect.w < max_children_size.w) {
                        this->add_scrollbar(Align::Horizontal);
                    } else {
                        this->remove_scrollbar(Align::Horizontal);
                    }
                    if (this->has_scrollbar(Align::Vertical)) {
                        float vscroll = 0;
                        if (this->has_scrollbar(Align::Horizontal)) vscroll += this->m_horizontal_scrollbar->size_hint(dc).h;
                        content_y -= this->m_vertical_scrollbar->m_slider->m_value * ((total_children_size.h + vscroll) - rect.h);
                    }
                    if (this->has_scrollbar(Align::Horizontal)) {
                        float hscroll = 0;
                        if (this->has_scrollbar(Align::Vertical)) hscroll += this->m_vertical_scrollbar->size_hint(dc).w;
                        content_x -= this->m_horizontal_scrollbar->m_slider->m_value * ((max_children_size.w + hscroll) - rect.w);
                    }
                }

                Point<float> pos = Point<float>(content_x, content_y);
                switch (parent_layout) {
                    case Align::Vertical: {
                        if (this->has_scrollbar(Align::Vertical)) rect.w -= m_vertical_scrollbar->size_hint(dc).w;
                        if (this->has_scrollbar(Align::Horizontal)) rect.h -= m_horizontal_scrollbar->size_hint(dc).h;
                        for (Widget* child : this->children) {
                            Size<float> size;
                            Size<float> child_hint = child->size_hint(dc);
                            switch (child->fill_policy()) {
                                case Fill::Both: {
                                    size = Size<float> { 
                                        rect.w > child_hint.w ? rect.w : child_hint.w,
                                        child_hint.h
                                    };
                                    break;
                                }
                                case Fill::Horizontal:
                                    size = Size<float> { rect.w > child_hint.w ? rect.w : child_hint.w, child_hint.h };
                                    break;
                                case Fill::None:
                                case Fill::Vertical:
                                default:
                                    size = child_hint;
                            }
                            Rect<float> widget_rect = Rect<float>(pos.x, pos.y, size.w, size.h);
                            if (pos.y < 0) {
                                if (pos.y + size.h < 0) {
                                    child->rect = widget_rect;
                                }
                                else goto DRAW_VERTICAL;
                            } else {
                                DRAW_VERTICAL:
                                    child->draw(dc, widget_rect);
                                    if (pos.y > rect.h) break;
                            }
                            pos.y += size.h;
                        }
                        if (m_vertical_scrollbar) {
                            Size<float> size = m_vertical_scrollbar->size_hint(dc);
                            m_vertical_scrollbar->m_slider->m_slider_button_size = rect.h * ((rect.h - size.h / 2) / total_children_size.h) < 20 ? 20 : rect.h * ((rect.h - size.h / 2) / total_children_size.h);
                            m_vertical_scrollbar->draw(dc, Rect<float>(rect.x + rect.w, rect.y, size.w, rect.h));
                        }
                        if (m_horizontal_scrollbar) {
                            Size<float> size = m_horizontal_scrollbar->size_hint(dc);
                            m_horizontal_scrollbar->m_slider->m_slider_button_size = rect.w * ((rect.w - size.w / 2) / max_children_size.w) < 20 ? 20 : rect.w * ((rect.w - size.w / 2) / max_children_size.w);
                            m_horizontal_scrollbar->draw(dc, Rect<float>(rect.x, rect.y + rect.h, rect.w, size.h));
                        }
                        break;
                    }
                    case Align::Horizontal: {
                        if (this->has_scrollbar(Align::Vertical)) rect.w -= m_vertical_scrollbar->size_hint(dc).w;
                        if (this->has_scrollbar(Align::Horizontal)) rect.h -= m_horizontal_scrollbar->size_hint(dc).h;
                        for (Widget* child : this->children) {
                            Size<float> size;
                            Size<float> child_hint = child->size_hint(dc);
                            switch (child->fill_policy()) {
                                case Fill::Both: {
                                    size = Size<float> { 
                                        child_hint.w, 
                                        rect.h
                                    };
                                    break;
                                }
                                case Fill::Vertical:
                                    size = Size<float> { child_hint.w, rect.h > child_hint.h ? rect.h : child_hint.h };
                                    break;
                                case Fill::None:
                                case Fill::Horizontal:
                                default:
                                    size = child_hint;
                            }
                            Rect<float> widget_rect = Rect<float>(pos.x, pos.y, size.w, size.h);
                            if (pos.x < 0) {
                                if (pos.x + size.w < 0) {
                                    child->rect = widget_rect;
                                }
                                else goto DRAW_HORIZONTAL;
                            } else {
                                DRAW_HORIZONTAL:
                                    child->draw(dc, widget_rect);
                                    if (pos.x > rect.w) break;
                            }
                            pos.x += size.w;
                        }
                        if (m_vertical_scrollbar) {
                            Size<float> size = m_vertical_scrollbar->size_hint(dc);
                            m_vertical_scrollbar->m_slider->m_slider_button_size = rect.h * ((rect.h - size.h / 2) / total_children_size.h) < 20 ? 20 : rect.h * ((rect.h - size.h / 2) / total_children_size.h);
                            m_vertical_scrollbar->draw(dc, Rect<float>(rect.x + rect.w, rect.y, size.w, rect.h));
                        }
                        if (m_horizontal_scrollbar) {
                            Size<float> size = m_horizontal_scrollbar->size_hint(dc);
                            m_horizontal_scrollbar->m_slider->m_slider_button_size = rect.w * ((rect.w - size.w / 2) / total_children_size.w) < 20 ? 20 : rect.w * ((rect.w - size.w / 2) / total_children_size.w);
                            m_horizontal_scrollbar->draw(dc, Rect<float>(rect.x, rect.y + rect.h, rect.w, size.h));
                        }
                        break;
                    }   
                }
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
