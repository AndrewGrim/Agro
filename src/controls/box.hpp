#ifndef BOX_HPP
    #define BOX_HPP

    #include "widget.hpp"
    #include "../renderer/drawing_context.hpp"

    class Box : public Widget {
        public:
            Box(Application *app, Align align_policy) {
                this->m_id = app->next_id();
                this->m_align_policy = align_policy;
            }

            ~Box() {}

            const char* name() {
                return this->m_name;
            }

            void draw(DrawingContext *dc, Rect<float> rect) {
                this->dc = dc;
                this->rect = rect;
                this->dc->fillRect(this->rect, Color(bg.red, bg.green, bg.blue, bg.alpha));

                layout_children(dc, rect);
            }

            void layout_children(DrawingContext *dc, Rect<float> rect) {
                int non_expandable_widgets = 0;
                int reserved_x = 0;
                int reserved_y = 0;
                Align parent_layout = this->m_align_policy;
                for (Widget* child : this->children) {
                    Fill child_layout = child->fill_policy();
                    if (child_layout == Fill::Horizontal && parent_layout == Align::Vertical) {
                        non_expandable_widgets += 1;
                        reserved_y += child->size_hint().h;
                    } else if (child_layout == Fill::Vertical && parent_layout == Align::Horizontal) {
                        non_expandable_widgets += 1;
                        reserved_x += child->size_hint().w;
                    } else if (child_layout == Fill::None) {
                        non_expandable_widgets += 1;
                        if (parent_layout == Align::Horizontal) reserved_x += child->size_hint().w;
                        else if (parent_layout == Align::Vertical) reserved_y += child->size_hint().h;
                    }
                }

                int child_count = this->children.size() - non_expandable_widgets;
                rect.w -= reserved_x;
                rect.h -= reserved_y;
                Point<float> pos = Point<float>(rect.x, rect.y);
                switch (this->m_align_policy) {
                    case Align::Vertical:
                        for (Widget* child : this->children) {
                            Size<float> size;
                            Size<float> child_hint = child->size_hint();
                            float expandable_height = rect.h / child_count;
                            switch (child->fill_policy()) {
                                case Fill::Both:
                                    size = Size<float> { 
                                        rect.w, 
                                        expandable_height > child_hint.h ? expandable_height : child_hint.h
                                    };
                                    break;
                                case Fill::Vertical:
                                    size = Size<float> { 
                                        child_hint.w, 
                                        expandable_height > child_hint.h ? expandable_height : child_hint.h
                                    };
                                    break;
                                case Fill::Horizontal:
                                    size = Size<float> { rect.w, child->size_hint().h };
                                    break;
                                case Fill::None:
                                default:
                                    size = child->size_hint();
                            }
                            child->draw(dc, Rect<float>(pos.x, pos.y, size.w, size.h));
                            pos.y += size.h;
                        }
                        break;
                    case Align::Horizontal:
                        for (Widget* child : this->children) {
                            Size<float> size;
                            Size<float> child_hint = child->size_hint();
                            float expandable_width = rect.w / child_count;
                            switch (child->fill_policy()) {
                                case Fill::Both:
                                    size = Size<float> { 
                                        expandable_width > child_hint.w ? expandable_width : child_hint.w, 
                                        rect.h
                                    };
                                    break;
                                case Fill::Vertical:
                                    size = Size<float> { child->size_hint().w, rect.h };
                                    break;
                                case Fill::Horizontal:
                                    size = Size<float> { 
                                        expandable_width > child_hint.w ? expandable_width : child_hint.w, 
                                        rect.h
                                    };
                                    break;
                                case Fill::None:
                                default:
                                    size = child->size_hint();
                            }
                            child->draw(dc, Rect<float>(pos.x, pos.y, size.w, size.h));
                            pos.x += size.w;
                        }
                        break;
                }
            }

            Size<float> size_hint() {
                Size<float> size = Size<float> { 0, 0 };
                for (Widget* child : this->children) {
                    Size<float> s = child->size_hint();
                    size.w += s.w;
                    if (s.h > size.h) {
                        size.h = s.h;
                    }
                }

                return size;
            }

            Color background() {
                return this->bg;
            }

            Box* set_background(Color background) {
                this->bg = background;
                if (this->dc) this->update();

                return this;
            }

            virtual void update() {
                // this->draw(this->dc, this->rect);
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