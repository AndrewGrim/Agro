#ifndef TREEVIEW_HPP
    #define TREEVIEW_HPP

    #include <algorithm>

    #include "widget.hpp"
    #include "scrollable.hpp"
    #include "spacer.hpp"
    #include "../application.hpp"
    #include "cell_renderer.hpp"

    /// Meant to represent a single row within the TreeView Widget.
    /// Note that a TreeNode can have children and so its not exactly
    /// equivalent to a single row.
    template <typename T> class TreeNode {
        public:
            std::vector<Drawable*> columns;

            /// This is a pointer to backend data.
            /// This can be used to display one value but perform
            /// the sorting process on something completely different.
            T *hidden = nullptr;

            TreeNode<T> *parent = nullptr;
            int parent_index = -1;
            std::vector<TreeNode<T>*> children;
            bool is_collapsed = false;
            int max_cell_height = 0;
            int depth = 0;

            /// Internal data that stores the vertical position and height of the TreeNode.
            BinarySearchData bs_data;

            TreeNode(std::vector<Drawable*> columns, T *hidden) {
                this->columns = columns;
                this->hidden = hidden;
            }

            ~TreeNode() {
                for (TreeNode<T> *child : children) {
                    delete child;
                }
                for (Drawable *drawable : columns) {
                    delete drawable;
                }
                delete hidden;
            }
    };

    enum class Traversal {
        /// Continue going down as normal by traversing all nodes.
        Continue,

        /// Ends the traversal of the current node and its children early and goes to the next one on the same level.
        Next,

        /// Ends the traversal of the entire tree immediately.
        /// Note: When manually descending (NOT using forEachNode) it is your
        /// responsibility to check `early_exit` for Traversal::Break
        /// after calling descend().
        Break
    };

    /// The model for TreeNodes.
    /// Contains utility methods for traversing the model
    /// adding nodes and emptying the model.
    template <typename T> class Tree {
        public:
            std::vector<TreeNode<T>*> roots;

            Tree() {

            }

            ~Tree() {
                for (TreeNode<T> *root : roots) {
                    delete root;
                }
            }

            TreeNode<T>* append(TreeNode<T> *parent_node, TreeNode<T> *node) {
                if (!parent_node) {
                    node->parent = nullptr;
                    node->depth = 1;
                    roots.push_back(node);
                    node->parent_index = roots.size() - 1;

                    return node;
                } else {
                    parent_node->children.push_back(node);
                    node->parent_index = parent_node->children.size() - 1;
                    node->parent = parent_node;
                    node->depth = node->parent->depth + 1;

                    return node;
                }
            }

            void clear() {
                for (TreeNode<T> *root : roots) {
                    delete root;
                }
                roots.clear();
            }

            TreeNode<T>* descend(Traversal &early_exit, TreeNode<T> *root, std::function<Traversal(TreeNode<T> *node)> fn = nullptr) {
                if (fn) {
                    early_exit = fn(root);
                    if (early_exit == Traversal::Break || early_exit == Traversal::Next) {
                        return root;
                    }
                }
                if (root->children.size()) {
                    TreeNode<T> *last = nullptr;
                    for (TreeNode<T> *child : root->children) {
                        last = descend(early_exit, child, fn);
                        if (early_exit == Traversal::Break) {
                            return last;
                        }
                    }
                    return last;
                } else {
                    return root;
                }
            }

            void forEachNode(std::vector<TreeNode<T>*> &roots, std::function<Traversal(TreeNode<T> *node)> fn) {
                Traversal early_exit = Traversal::Continue;
                for (TreeNode<T> *root : roots) {
                    descend(early_exit, root, fn);
                    if (early_exit == Traversal::Break) {
                        break;
                    }
                }
            }
            // TreeNode find(compare function);
            // Goes down the tree using row (skips non visible nodes)
            // TreeNode get(size_t row);
            // TreeNode ascend(TreeNode leaf, std::function<void(TreeNode node)> callback = nullptr);
    };

    template <typename T> class TreeView;

    enum class Sort {
        None,
        Ascending,
        Descending
    };

    template <typename T> class Column : public Box {
        public:
            std::function<bool(TreeNode<T> *lhs, TreeNode<T> *rhs)> sort_fn = nullptr;

            Column(
                std::string text, Image *image = nullptr,
                HorizontalAlignment alignment = HorizontalAlignment::Center,
                std::function<bool(TreeNode<T> *lhs, TreeNode<T> *rhs)> sort_function = nullptr) : Box(Align::Horizontal) {
                if (alignment == HorizontalAlignment::Right) {
                    this->append(new Spacer(), Fill::Both);
                }
                Button *b = new Button(text);
                    b->setImage(image);
                if (alignment == HorizontalAlignment::Left || alignment == HorizontalAlignment::Right) {
                    this->append(b, Fill::Vertical);
                } else {
                    this->append(b, Fill::Both);
                }
                this->sort_fn = sort_function;
                if (sort_fn) {
                    if (alignment == HorizontalAlignment::Left) {
                        this->append(new Spacer(), Fill::Both);
                    }
                    IconButton *sort_icon = new IconButton((new Image(Application::get()->icons["up_arrow"]))->setMinSize(Size(12, 12)));
                        sort_icon->hide();
                    this->append(sort_icon, Fill::Vertical);
                }
                this->onMouseDown.addEventListener([&](Widget *widget, MouseEvent event) {
                    if (event.x >= (rect.x + rect.w) - 5) {
                        m_dragging = true;
                        m_custom_width = m_custom_size ? m_custom_width : m_size.w;
                        m_custom_size = true;
                        Application::get()->setMouseCursor(Cursor::SizeWE);
                    }
                });
                this->onMouseClick.addEventListener([&](Widget *widget, MouseEvent event) {
                    if (!m_dragging && sort_fn) {
                        if (m_sort == Sort::None) {
                            children[children.size() - 1]->show();
                            m_sort = Sort::Ascending;
                        } else if (m_sort == Sort::Ascending) {
                            IconButton *sort_icon = (IconButton*)children[children.size() - 1];
                                sort_icon->image()->flipVertically();
                            m_sort = Sort::Descending;
                        } else {
                            IconButton *sort_icon = (IconButton*)children[children.size() - 1];
                                sort_icon->image()->flipVertically();
                            m_sort = Sort::Ascending;
                        }
                        sort(m_sort);
                    } else {
                        m_dragging = false;
                        Application::get()->setMouseCursor(Cursor::Default);
                    }
                });
                this->onMouseMotion.addEventListener([&](Widget *widget, MouseEvent event) {
                    if (m_dragging) {
                        // Ignore any right side movement if the mouse is to the left of the column's right most boundary.
                        if (!((event.x < (rect.x + rect.w)) && (event.xrel > 0))) {
                            setExpand(false);
                            setWidth(rect.w + event.xrel);
                        }
                    } else {
                        if (!isPressed()) {
                            if (event.x >= (rect.x + rect.w) - 5) {
                                Application::get()->setMouseCursor(Cursor::SizeWE);
                            } else {
                                Application::get()->setMouseCursor(Cursor::Default);
                            }
                        }
                    }
                });
                this->onMouseLeft.addEventListener([&](Widget *widget, MouseEvent event) {
                    if (m_dragging) {
                        m_dragging = false;
                    }
                    Application::get()->setMouseCursor(Cursor::Default);
                });
            }

            ~Column() {

            }

            virtual const char* name() override {
                return "Column";
            }

            virtual void draw(DrawingContext &dc, Rect rect, int state) override {
                this->rect = rect;
                Color color;
                if (m_dragging) {
                    color = dc.widgetBackground(style);
                } else if (state & STATE_PRESSED && state & STATE_HOVERED) {
                    color = dc.pressedBackground(style);
                } else if (state & STATE_HOVERED) {
                    color = dc.hoveredBackground(style);
                } else {
                    color = dc.widgetBackground(style);
                }
                dc.drawBorder(rect, style, state);

                dc.fillRect(rect, color);
                layoutChildren(dc, rect);
            }

            virtual bool isLayout() override {
                return false;
            }

            void sort(Sort sort) {
                if (sort_fn) {
                    if (sort == Sort::None) {
                        IconButton *sort_icon = (IconButton*)children[children.size() - 1];
                            sort_icon->hide();
                        if (m_sort == Sort::Descending) {
                            sort_icon->image()->flipVertically();
                        }
                        m_sort = sort;
                    } else {
                        assert(m_model && "Model cannot be null when sorting! Only sort once youve set the model.");
                        if (isSorted() == Sort::Ascending) {
                            std::sort(m_model->roots.rbegin(), m_model->roots.rend(), sort_fn);
                        } else {
                            std::sort(m_model->roots.begin(), m_model->roots.end(), sort_fn);
                        }
                        ((TreeView<T>*)parent)->m_virtual_size_changed = true;
                        ((TreeView<T>*)parent)->sizeHint(*Application::get()->getCurrentWindow()->dc);
                        update();
                    }
                }
            }

            Sort isSorted() {
                return m_sort;
            }

            void setModel(Tree<T> *model) {
                m_model = model;
            }

            virtual Size sizeHint(DrawingContext &dc) override {
                unsigned int visible = 0;
                unsigned int horizontal_non_expandable = 0;
                if (m_size_changed) {
                    Size size = Size();
                    for (Widget *child : children) {
                        Size s = child->sizeHint(dc);
                        size.w += s.w;
                        if (s.h > size.h) {
                            size.h = s.h;
                        }
                        visible += child->proportion();
                        if (child->fillPolicy() == Fill::Vertical || child->fillPolicy() == Fill::None) {
                            horizontal_non_expandable++;
                        }
                    }
                    m_widgets_only = size;

                    dc.sizeHintBorder(size, style);

                    m_horizontal_non_expandable = horizontal_non_expandable;
                    m_visible_children = visible;
                    m_size = size;
                    m_size_changed = false;

                    if (m_custom_size) {
                        return Size(m_custom_width, m_size.h);
                    }
                    return size;
                } else {
                    if (m_custom_size) {
                        return Size(m_custom_width, m_size.h);
                    }
                    return m_size;
                }
            }

            int width() {
                if (m_custom_size) {
                    return m_custom_width;
                }
                return m_size.w;
            }

            void setWidth(int width) {
                if (width < m_min_width) {
                    return;
                }
                m_custom_size = true;
                m_custom_width = width;
                rect.w = width;
                layout();
            }

            void setColumnStyle(Style column, Style button) {
                setStyle(column);
                for (auto child : children) {
                    child->setStyle(button);
                }
            }

            Column* setTooltip(Widget *tooltip) {
                this->tooltip = tooltip;
                return this;
            }

            void setExpand(bool expand) {
                m_expand = expand;
                update();
            }

            bool expand() {
                return m_expand;
            }

        protected:
            Sort m_sort = Sort::None;
            Tree<T> *m_model = nullptr;
            bool m_dragging = false;
            bool m_custom_size = false;
            int m_custom_width = 0;
            int m_min_width = 16;
            bool m_expand = false;
    };

    /// Describes which of the grid lines get drawn.
    enum class GridLines {
        None,
        Horizontal,
        Vertical,
        Both,
    };

    /// Describes the scrolling mode of the TreeView.
    /// Scroll means that the TreeView uses its own ScrollBars.
    /// Unroll means the TreeView contents are factored into the
    /// physical size of the Widget and stretch the height of it.
    /// Unroll also always leaves the Column heading at the top of the Widget
    /// it does not scroll them with the contents.
    /// in this mode it is the responsibility of the parent Widget to take
    /// care of any scrolling operations.
    /// This mode can be desirable when you want to have multiple TreeViews
    /// within a Scrollable without everyone of the stealing the scroll events.
    enum class Mode {
        Scroll,
        Unroll,
    };

    template <typename T> class TreeView : public Scrollable {
        public:
            EventListener<TreeView<T>*, TreeNode<T>*> onNodeHovered = EventListener<TreeView<T>*, TreeNode<T>*>();
            EventListener<TreeView<T>*, TreeNode<T>*> onNodeSelected = EventListener<TreeView<T>*, TreeNode<T>*>();
            EventListener<TreeView<T>*, TreeNode<T>*> onNodeDeselected = EventListener<TreeView<T>*, TreeNode<T>*>();
            EventListener<TreeView<T>*, TreeNode<T>*> onNodeCollapsed = EventListener<TreeView<T>*, TreeNode<T>*>();
            EventListener<TreeView<T>*, TreeNode<T>*> onNodeExpanded = EventListener<TreeView<T>*, TreeNode<T>*>();

            TreeView(Size min_size = Size(100, 100)) : Scrollable(min_size) {
                this->onMouseMotion.addEventListener([&](Widget *widget, MouseEvent event) {
                    if (m_event_node) {
                        if (m_hovered != m_event_node) {
                            m_hovered = m_event_node;
                            onNodeHovered.notify(this, m_event_node);
                        }
                        int x = inner_rect.x;
                        if (m_horizontal_scrollbar->isVisible()) {
                            x -= m_horizontal_scrollbar->m_slider->m_value * ((m_virtual_size.w) - inner_rect.w);
                        }
                        if ((m_event_node->children.size() && !m_table) &&
                            (event.x >= x + (m_event_node->depth - 1) * m_indent && event.x <= x + m_event_node->depth * m_indent)) {
                            m_tree_collapser = m_event_node;
                        } else {
                            m_tree_collapser = nullptr;
                        }
                        update();
                    }
                });
                this->onMouseDown.addEventListener([&](Widget *widget, MouseEvent event) {
                    if (m_event_node) {
                        int x = inner_rect.x;
                        if (m_horizontal_scrollbar->isVisible()) {
                            x -= m_horizontal_scrollbar->m_slider->m_value * ((m_virtual_size.w) - inner_rect.w);
                        }
                        if (m_event_node->children.size() &&
                            (!m_table && (event.x >= x + (m_event_node->depth - 1) * m_indent && event.x <= x + m_event_node->depth * m_indent))) {
                            if (m_event_node->is_collapsed) {
                                expand(m_event_node);
                            } else {
                                collapse(m_event_node);
                            }
                        } else {
                            select(m_event_node);
                        }
                    }
                });
                this->onMouseLeft.addEventListener([&](Widget *widget, MouseEvent event) {
                    this->m_hovered = nullptr;
                });
                bind(SDLK_UP, Mod::None, [&]() {
                    if (m_focused) {
                        // TODO make sure to scroll the node into view
                        auto focusNextNode = [&](TreeNode<T> *node) -> TreeNode<T>* {
                            if (node->parent_index == 0 && node->depth != 1) {
                                return node->parent;
                            }
                            if (node->parent_index > 0 && node->depth != 1) {
                                return node->parent->children[node->parent_index - 1];
                            }
                            if (node->depth == 1 && node->parent_index > 0) {
                                Traversal early_exit = Traversal::Continue;
                                return m_model->descend(early_exit, m_model->roots[node->parent_index - 1], [&](TreeNode<T> *node) {
                                    if (node->is_collapsed) { return Traversal::Break; }
                                    else { return Traversal::Continue; }
                                });
                            }
                            return nullptr; // TODO change to maybe wrap around
                        };
                        TreeNode<T> *result = focusNextNode(m_focused);
                        if (result) {
                            select(result);
                        }
                    } else {
                        assert(m_model && m_model->roots.size() && "Trying to focus node when model doesnt exist or is empty!");
                        m_focused = m_model->roots[0];
                    }
                    update();
                });
                bind(SDLK_DOWN, Mod::None, [&]() {
                    if (m_focused) {
                        // TODO make sure to scroll the node into view
                        auto focusNextNode = [&](TreeNode<T> *node) -> TreeNode<T>* {
                            if (node->children.size() && !node->is_collapsed) {
                                return node->children[0];
                            }
                            while (node->parent) {
                                if ((int)node->parent->children.size() - 1 > node->parent_index) {
                                    return node->parent->children[node->parent_index + 1];
                                }
                                node = node->parent;
                            }
                            if (node->depth == 1 && (int)m_model->roots.size() - 1 > node->parent_index) {
                                return m_model->roots[node->parent_index + 1];
                            }
                            return nullptr; // TODO change to maybe wrap around
                        };
                        TreeNode<T> *result = focusNextNode(m_focused);
                        if (result) {
                            select(result);
                        }
                    } else {
                        assert(m_model && m_model->roots.size() && "Trying to focus node when model doesnt exist or is empty!");
                        m_focused = m_model->roots[0];
                    }
                    update();
                });
                bind(SDLK_LEFT, Mod::None, [&]() {
                    if (m_focused && m_focused->children.size()) {
                        collapse(m_focused);
                    }
                });
                bind(SDLK_RIGHT, Mod::None, [&]() {
                    if (m_focused && m_focused->children.size()) {
                        expand(m_focused);
                    }
                });
                m_column_style = Style();
                m_column_style.border.type = STYLE_BOTTOM | STYLE_RIGHT;
                m_column_style.border.bottom = 1;
                m_column_style.border.right = 1;
                m_column_style.margin.type = STYLE_NONE;

                m_column_button_style = Style();
                m_column_button_style.widget_background = COLOR_NONE;
                m_column_button_style.border.type = STYLE_NONE;
                m_column_button_style.margin.type = STYLE_NONE;
            }

            ~TreeView() {
                delete m_model;
                delete m_collapsed;
                delete m_expanded;
            }

            virtual const char* name() override {
                return "TreeView";
            }

            virtual void draw(DrawingContext &dc, Rect rect, int state) override {
                assert(m_model && "A TreeView needs a model to work!");
                this->rect = rect;

                dc.margin(rect, style);
                dc.drawBorder(rect, style, state);
                this->inner_rect = rect;
                dc.fillRect(rect, dc.textBackground(style));

                Size virtual_size = m_virtual_size;
                if (areColumnHeadersHidden()) {
                    virtual_size.h -= m_children_size.h;
                }
                Rect old_clip = dc.clip();
                Point pos = Point(rect.x, rect.y);
                if (m_mode == Mode::Scroll) {
                    pos = automaticallyAddOrRemoveScrollBars(dc, rect, virtual_size);
                } else {
                    m_vertical_scrollbar->hide();
                    m_horizontal_scrollbar->hide();
                }
                this->inner_rect = rect;
                Rect tv_clip = old_clip;

                int child_count = m_expandable_columns;
                if (child_count < 1) { child_count = 1; }
                int expandable_length = (rect.w - m_children_size.w) / child_count;
                int remainder = (rect.w - m_children_size.w) % child_count;
                if (expandable_length < 0) {
                    expandable_length = 0;
                    remainder = 0;
                }
                int local_pos_x = pos.x;
                int i = 0;
                m_current_header_width = 0;
                for (Widget *child : children) {
                    int child_expandable_length = expandable_length;
                    if (remainder) {
                        child_expandable_length++;
                        remainder--;
                    }
                    Size s = child->sizeHint(dc);
                    if (((Column<T>*)child)->expand()) {
                        s.w += child_expandable_length > 0 ? child_expandable_length : 0;
                        m_column_widths[i] = s.w;
                    }
                    m_current_header_width += s.w;
                    child->rect = Rect(local_pos_x, rect.y, s.w, m_children_size.h); // Always set rect because we need it for event handling.
                    if (!areColumnHeadersHidden()) {
                        dc.setClip(Rect(
                            local_pos_x,
                            rect.y,
                            local_pos_x + s.w > rect.x + rect.w ? (rect.x + rect.w) - local_pos_x : s.w,
                            m_children_size.h
                        ).clipTo(tv_clip).clipTo(rect));
                        child->draw(dc, Rect(local_pos_x, rect.y, s.w, m_children_size.h), child->state());
                    }
                    local_pos_x += s.w;
                    i++;
                }
                int column_header = 0;
                if (!areColumnHeadersHidden()) {
                    column_header = m_children_size.h;
                    pos.y += column_header;
                }
                Rect drawing_rect = rect;
                if (m_mode == Mode::Unroll) {
                    drawing_rect = Rect(0, 0, Application::get()->getCurrentWindow()->size.w, Application::get()->getCurrentWindow()->size.h);
                    column_header = 0;
                }

                bool collapsed = false;
                int collapsed_depth = -1;
                size_t y_scroll_offset = (m_vertical_scrollbar->isVisible() ? m_vertical_scrollbar->m_slider->m_value : 0.0) * ((virtual_size.h) - inner_rect.h);
                if (m_mode == Mode::Unroll && rect.y + m_children_size.h < 0) {
                    y_scroll_offset = (rect.y + m_children_size.h) * -1;
                }
                Option<TreeNode<T>*> result = binarySearch(m_model->roots, y_scroll_offset).value;
                if (result) {
                    pos.y += result.value->bs_data.position;
                    TreeNode<T> *node = result.value;

                    bool finished = false;
                    while (node) {
                        if (node->depth <= collapsed_depth) {
                            collapsed = false;
                            collapsed_depth = -1;
                        }
                        if (!collapsed) {
                            drawNode(dc, pos, node, rect, drawing_rect, tv_clip, column_header);
                            if (pos.y > drawing_rect.y + drawing_rect.h) { break; }
                            if (node->is_collapsed && !collapsed) {
                                collapsed = true;
                                collapsed_depth = node->depth;
                            }

                            m_model->forEachNode(node->children, [&](TreeNode<T> *node) -> Traversal {
                                if (node->depth <= collapsed_depth) {
                                    collapsed = false;
                                    collapsed_depth = -1;
                                }
                                if (!collapsed) {
                                    drawNode(dc, pos, node, rect, drawing_rect, tv_clip, column_header);
                                    if (pos.y > drawing_rect.y + drawing_rect.h) {
                                        finished = true;
                                        return Traversal::Break;
                                    }
                                }
                                if (node->is_collapsed && !collapsed) {
                                    collapsed = true;
                                    collapsed_depth = node->depth;
                                }
                                return Traversal::Continue;
                            });
                        }
                        if (finished) { break; }
                        node = findNextNode(node);
                    }
                }

                if (m_model->roots.size()) {
                    int local_column_header = !areColumnHeadersHidden() ? m_children_size.h : 0;
                    // Clip and draw column grid lines.
                    if (m_grid_lines == GridLines::Vertical || m_grid_lines == GridLines::Both) {
                        dc.setClip(Rect(rect.x, rect.y + local_column_header, rect.w, rect.h - local_column_header).clipTo(tv_clip));
                        for (int width : m_column_widths) {
                            dc.fillRect(Rect(pos.x + width - m_grid_line_width, rect.y + local_column_header, m_grid_line_width, virtual_size.h - local_column_header), dc.textDisabled(style));
                            pos.x += width;
                        }
                    }
                }

                dc.setClip(old_clip);
                if (m_mode == Mode::Scroll) {
                    drawScrollBars(dc, rect, virtual_size);
                }
                dc.drawKeyboardFocus(this->rect, style, state);
            }

            void setModel(Tree<T> *model) {
                delete m_model;
                m_model = model;
                for (Widget *widget : children) {
                    ((Column<T>*)widget)->setModel(model);
                    ((Column<T>*)widget)->setColumnStyle(m_column_style, m_column_button_style);
                }
                m_virtual_size_changed = true;
                m_auto_size_columns = true;
                m_hovered = nullptr;
                m_focused = nullptr;
                m_event_node = nullptr;
                m_tree_collapser = nullptr;
                if (m_last_sort) {
                    m_last_sort->sort(Sort::None);
                    m_last_sort = nullptr;
                }
                update();
            }

            void sort(size_t column_index, Sort sort_type) {
                ((Column<T>*)children[column_index])->sort(sort_type);
            }

            void clear() {
                if (m_model) {
                    m_model->clear();
                    m_virtual_size_changed = true;
                    m_hovered = nullptr;
                    m_focused = nullptr;
                    m_event_node = nullptr;
                    m_tree_collapser = nullptr;
                    if (m_last_sort) {
                        m_last_sort->sort(Sort::None);
                        m_last_sort = nullptr;
                    }
                    update();
                }
            }

            void setGridLines(GridLines grid_lines) {
                m_grid_lines = grid_lines;
                update();
            }

            GridLines gridLines() {
                return m_grid_lines;
            }

            /// Can return null.
            TreeNode<T>* hovered() {
                return m_hovered;
            }

            /// Can return null.
            TreeNode<T>* selected() {
                return m_focused;
            }

            uint8_t indent() {
                return m_indent;
            }

            void setIndent(uint8_t indent_width) {
                if (indent_width >= 12) {
                    m_indent = indent_width;
                    update();
                }
            }

            void select(TreeNode<T> *node) {
                if (m_focused != node) {
                    deselect();
                    m_focused = node;
                    onNodeSelected.notify(this, node);
                    update();
                } else {
                    deselect();
                }
            }

            void select(size_t index) {
                size_t i = 0;
                m_model->forEachNode(m_model->roots, [&](TreeNode<T> *node) -> Traversal {
                    if (i == index) {
                        select(node);
                        return Traversal::Break;
                    }
                    i++;
                    return Traversal::Continue;
                });
            }

            void deselect() {
                if (m_focused) {
                    onNodeDeselected.notify(this, m_focused);
                    m_focused = nullptr;
                    update();
                }
            }

            void collapse(TreeNode<T> *node) {
                if (node && node->children.size()) {
                    node->is_collapsed = true;
                    onNodeCollapsed.notify(this, node);
                    m_virtual_size_changed = true;
                    update();
                    if (m_mode == Mode::Unroll && parent) { parent->m_size_changed = true; }
                }
            }

            void collapseRecursively(TreeNode<T> *node) {
                collapseOrExpandRecursively(node, true);
            }

            void collapseAll() {
                collapseOrExpandAll(true);
            }

            void expand(TreeNode<T> *node) {
                if (node && node->children.size()) {
                    node->is_collapsed = false;
                    onNodeExpanded.notify(this, node);
                    m_virtual_size_changed = true;
                    update();
                    if (m_mode == Mode::Unroll && parent) { parent->m_size_changed = true; }
                }
            }

            void expandRecursively(TreeNode<T> *node) {
                collapseOrExpandRecursively(node, false);
            }

            void expandAll() {
                collapseOrExpandAll(false);
            }

            TreeView* append(Column<T> *column) {
                if (column->parent) {
                    column->parent->remove(column->parent_index);
                }
                column->parent = this;
                column->onMouseEntered.addEventListener([&](Widget *widget, MouseEvent event) {
                    this->m_hovered = nullptr;
                });
                column->onMouseClick.addEventListener([&](Widget *column, MouseEvent event) {
                    Column<T> *col = (Column<T>*)column;
                    if (col->sort_fn) {
                        if (this->m_last_sort && this->m_last_sort != col) {
                            this->m_last_sort->sort(Sort::None);
                        } else {
                            col->sort(col->isSorted());
                        }
                        this->m_last_sort = col;
                    }
                });
                children.push_back(column);
                column->parent_index = children.size() - 1;
                layout();

                return this;
            }

            virtual Size sizeHint(DrawingContext &dc) override {
                if (m_size_changed) { // TODO this will be slow for a large number of columns
                    Scrollable::sizeHint(dc);
                    m_virtual_size.w = 0;
                    m_column_widths.clear();
                    Size size = Size();
                    m_expandable_columns = 0;
                    for (Widget *child : children) {
                        Size s = child->sizeHint(dc);
                        m_column_widths.push_back(s.w);
                        size.w += s.w;
                        if (s.h > size.h) {
                            size.h = s.h;
                        }
                        if (((Column<T>*)child)->expand()) {
                            m_expandable_columns++;
                        }
                    }
                    m_children_size = size;
                    m_virtual_size.w = size.w;
                    m_size_changed = false;
                }
                if (m_virtual_size_changed) {
                    calculateVirtualSize(dc);
                }
                if (m_mode == Mode::Scroll) {
                    Size viewport_and_style = m_viewport;
                        dc.sizeHintMargin(viewport_and_style, style);
                        dc.sizeHintBorder(viewport_and_style, style);
                    return viewport_and_style;
                }
                if (areColumnHeadersHidden()) {
                    Size virtual_size_and_style = Size(m_virtual_size.w, m_virtual_size.h - m_children_size.h);
                        dc.sizeHintMargin(virtual_size_and_style, style);
                        dc.sizeHintBorder(virtual_size_and_style, style);
                    return virtual_size_and_style;
                }
                Size virtual_size_and_style = m_virtual_size;
                    dc.sizeHintMargin(virtual_size_and_style, style);
                    dc.sizeHintBorder(virtual_size_and_style, style);
                return virtual_size_and_style;
            }

            bool isTable() {
                return m_table;
            }

            void setTableMode(bool table) {
                m_table = table;
                update();
            }

            void setMode(Mode mode) {
                m_mode = mode;
                layout();
            }

            Mode mode() {
                return m_mode;
            }

            void showColumnHeaders() {
                m_column_headers_hidden = false;
            }

            void hideColumnHeaders() {
                m_column_headers_hidden = true;
            }

            bool areColumnHeadersHidden() {
                return m_column_headers_hidden;
            }

            Widget* propagateMouseEvent(Window *window, State *state, MouseEvent event) override {
                if (m_vertical_scrollbar->isVisible()) {
                    if ((event.x >= m_vertical_scrollbar->rect.x && event.x <= m_vertical_scrollbar->rect.x + m_vertical_scrollbar->rect.w) &&
                        (event.y >= m_vertical_scrollbar->rect.y && event.y <= m_vertical_scrollbar->rect.y + m_vertical_scrollbar->rect.h)) {
                        return m_vertical_scrollbar->propagateMouseEvent(window, state, event);
                    }
                }
                if (m_horizontal_scrollbar->isVisible()) {
                    if ((event.x >= m_horizontal_scrollbar->rect.x && event.x <= m_horizontal_scrollbar->rect.x + m_horizontal_scrollbar->rect.w) &&
                        (event.y >= m_horizontal_scrollbar->rect.y && event.y <= m_horizontal_scrollbar->rect.y + m_horizontal_scrollbar->rect.h)) {
                        return m_horizontal_scrollbar->propagateMouseEvent(window, state, event);
                    }
                }
                if (m_vertical_scrollbar->isVisible() && m_horizontal_scrollbar->isVisible()) {
                    if ((event.x > m_horizontal_scrollbar->rect.x + m_horizontal_scrollbar->rect.w) &&
                        (event.y > m_vertical_scrollbar->rect.y + m_vertical_scrollbar->rect.h)) {
                        if (state->hovered) {
                            state->hovered->onMouseLeft.notify(this, event);
                        }
                        state->hovered = nullptr;
                        update();
                        return nullptr;
                    }
                }
                if (!areColumnHeadersHidden()) {
                    for (Widget *child : children) {
                        if ((event.x >= child->rect.x && event.x <= child->rect.x + child->rect.w) &&
                            (event.y >= child->rect.y && event.y <= child->rect.y + child->rect.h)) {
                            Widget *last = nullptr;
                            if (child->isLayout()) {
                                last = child->propagateMouseEvent(window, state, event);
                            } else {
                                child->handleMouseEvent(window, state, event);
                                last = child;
                            }
                            return last;
                        }
                    }
                }
                {
                    // Go down the Node tree to find either a Widget to pass the event to
                    // or simply record the node and pass the event to the TreeView itself as per usual.
                    int x = inner_rect.x;
                    int y = inner_rect.y;
                    if (m_horizontal_scrollbar) {
                        x -= m_horizontal_scrollbar->m_slider->m_value * ((m_virtual_size.w) - inner_rect.w);
                    }
                    if (event.x <= x + m_current_header_width) {
                        Size virtual_size = m_virtual_size;
                        if (areColumnHeadersHidden()) {
                            virtual_size.h -= m_children_size.h;
                        } else {
                            y += m_children_size.h;
                        }
                        size_t y_scroll_offset = (m_vertical_scrollbar->isVisible() ? m_vertical_scrollbar->m_slider->m_value : 0.0) * ((virtual_size.h) - inner_rect.h);
                        // size_t x_scroll_offset = (m_horizontal_scrollbar->isVisible() ? m_horizontal_scrollbar->m_slider->m_value : 0.0) * ((virtual_size.w) - inner_rect.w);
                        Option<TreeNode<T>*> result = binarySearch(m_model->roots, (event.y - y) + y_scroll_offset).value;
                        if (result) {
                            TreeNode<T> *node = result.value;
                            for (size_t i = 0; i < children.size(); i++) {
                                Column<T> *col = (Column<T>*)children[i];
                                if ((event.x >= col->rect.x) && (event.x <= (col->rect.x + col->rect.w))) {
                                    if (node->columns[i]->isWidget()) {
                                        auto widget = (Widget*)node->columns[i];
                                        widget->parent = this;
                                        if ((event.x >= widget->rect.x && event.x <= widget->rect.x + widget->rect.w) &&
                                            (event.y >= widget->rect.y && event.y <= widget->rect.y + widget->rect.h)
                                        ) {
                                            Application *app = Application::get();
                                            widget->handleMouseEvent(app, app->m_state, event);
                                            return widget;
                                        } else {
                                            m_event_node = node;
                                        }
                                    } else {
                                        m_event_node = node;
                                    }
                                }
                            }
                        } else {
                            m_event_node = nullptr;
                            m_tree_collapser = nullptr;
                        }
                    } else {
                        m_event_node = nullptr;
                        m_tree_collapser = nullptr;
                    }
                }

                handleMouseEvent(window, state, event);
                return this;
            }

            Tree<T>* model() {
                return m_model;
            }

            Tree<T> *m_model = nullptr;
            Size m_virtual_size;
            bool m_virtual_size_changed = false;
            Mode m_mode = Mode::Scroll;
            uint8_t m_indent = 24;
            TreeNode<T> *m_hovered = nullptr;
            TreeNode<T> *m_focused = nullptr;
            TreeNode<T> *m_event_node = nullptr; // node to be handled in mouse events
            TreeNode<T> *m_tree_collapser = nullptr; // the tree collapse/expand icon node if any (for highlighting)
            GridLines m_grid_lines = GridLines::Both;
            int m_treeline_size = 2;
            int m_grid_line_width = 1;
            Column<T> *m_last_sort = nullptr;
            Size m_children_size = Size();
            int m_current_header_width = 0;
            bool m_column_headers_hidden = false;
            std::vector<int> m_column_widths;
            bool m_auto_size_columns = false;
            bool m_table = false;
            Style m_column_style;
            Style m_column_button_style;
            Image *m_collapsed = (new Image(Application::get()->icons["up_arrow"]))->clockwise90();
            Image *m_expanded = (new Image(Application::get()->icons["up_arrow"]))->flipVertically();
            int m_expandable_columns = 0;

            void collapseOrExpandRecursively(TreeNode<T> *node, bool is_collapsed) {
                if (node) {
                    Traversal _unused = Traversal::Continue;
                    m_model->descend(_unused, node, [&](TreeNode<T> *_node) {
                        if (_node->children.size()) {
                            _node->is_collapsed = is_collapsed;
                        }
                        return Traversal::Continue;
                    });
                    m_virtual_size_changed = true;
                    update();
                    if (m_mode == Mode::Unroll && parent) { parent->m_size_changed = true; }
                }
            }

            void collapseOrExpandAll(bool is_collapsed) {
                Traversal _unused = Traversal::Continue;
                for (TreeNode<T> *root : m_model->roots) {
                    m_model->descend(_unused, root, [&](TreeNode<T> *node) {
                        if (node->children.size()) {
                            node->is_collapsed = is_collapsed;
                        }
                        return Traversal::Continue;
                    });
                }
                m_virtual_size_changed = true;
                update();
                if (m_mode == Mode::Unroll && parent) { parent->m_size_changed = true; }
            }

            void calculateVirtualSize(DrawingContext &dc) {
                m_virtual_size = m_children_size;
                bool collapsed = false;
                int collapsed_depth = -1;
                int parent_index = 0;
                size_t scroll_offset = 0;

                m_model->forEachNode(
                    m_model->roots,
                    [&](TreeNode<T> *node) {
                        if (node->depth == 1) {
                            node->parent_index = parent_index++;
                        }
                        if (node->depth <= collapsed_depth) {
                            collapsed = false;
                            collapsed_depth = -1;
                        }
                        if (!collapsed) {
                            // Check and set the max height of the node.
                            int index = 0;
                            assert(node->columns.size() == children.size() && "The amount of Column<T>s and Drawables should be the same!");
                            for (Drawable *drawable : node->columns) {
                                Size s = drawable->sizeHint(dc);
                                Column<T> *col = (Column<T>*)children[index];
                                if (!m_table && !index) {
                                    s.w += node->depth * indent();
                                }

                                // Automatically set the columns to be wide
                                // enough for their contents.
                                if (m_auto_size_columns && s.w > col->width()) {
                                    s.w += m_grid_line_width;
                                    col->setWidth(s.w);
                                    // The below is necessary because sizeHint won't run
                                    // again until the next update().
                                    m_children_size.w += s.w - m_column_widths[index];
                                    m_column_widths[index] = s.w;
                                    // We don't need to recalculate here specifically
                                    // because we already update the values manually.
                                    m_size_changed = false;
                                }
                                if (s.h > node->max_cell_height) {
                                    // TODO doesnt this mean that if all the columns have the same height then we dont account for grid line?
                                    node->max_cell_height = s.h + m_grid_line_width;
                                }
                                index++;
                            }
                            node->bs_data = BinarySearchData{scroll_offset, (size_t)node->max_cell_height};
                            scroll_offset += (size_t)node->max_cell_height;
                            m_virtual_size.h += node->max_cell_height;
                        } else {
                            node->bs_data = BinarySearchData{scroll_offset, 0};
                        }

                        if (node->is_collapsed && !collapsed) {
                            collapsed = true;
                            collapsed_depth = node->depth;
                        }
                        return Traversal::Continue;
                    }
                );

                m_virtual_size.w = m_children_size.w;
                m_virtual_size_changed = false;
                m_auto_size_columns = false;
            }


            BinarySearchResult<TreeNode<T>*> binarySearch(std::vector<TreeNode<T>*> &roots, size_t target) {
                if (!roots.size()) { return BinarySearchResult<TreeNode<T>*>{ 0, Option<TreeNode<T>*>() }; }
                size_t lower = 0;
                size_t upper = roots.size() - 1;
                size_t mid = 0;
                BinarySearchData point = {0, 0};

                while (lower <= upper) {
                    mid = (lower + upper) / 2;
                    point = roots[mid]->bs_data;
                    if (target < point.position) {
                        upper = mid - 1;
                    } else if (target > (roots.size() - 1 > mid ? roots[mid + 1]->bs_data.position : point.position + point.length)) {
                        lower = mid + 1;
                    } else {
                        break;
                    }
                }

                if (point.position <= target && point.position + point.length >= target) {
                   return BinarySearchResult<TreeNode<T>*>{ mid, Option<TreeNode<T>*>(roots[mid]) };
                } else if (roots[mid]->children.size()) {
                    return binarySearch(roots[mid]->children, target);
                }

                return BinarySearchResult<TreeNode<T>*>{ 0, Option<TreeNode<T>*>() };
            }

            void drawNode(DrawingContext &dc, Point &pos, TreeNode<T> *node, Rect rect, Rect drawing_rect, Rect tv_clip, int column_header) {
                int cell_start = pos.x;
                for (size_t i = 0; i < node->columns.size(); i++) {
                    int col_width = m_column_widths[i];
                    Drawable *drawable = node->columns[i];
                    Size s = drawable->sizeHint(dc);
                    if (cell_start + col_width > drawing_rect.x && cell_start < drawing_rect.x + drawing_rect.w) {
                        Rect cell_clip =
                            Rect(cell_start, pos.y, col_width, node->max_cell_height)
                                .clipTo(
                                    Rect(
                                        rect.x,
                                        rect.y + column_header,
                                        rect.w,
                                        rect.h - column_header
                                    )
                                );
                        // Clip and draw the current cell.
                        dc.setClip(cell_clip.clipTo(tv_clip));
                        int cell_x = cell_start;
                        int state = STATE_DEFAULT;
                        if (m_focused == node) { state |= STATE_HARD_FOCUSED; }
                        if (m_hovered == node) { state |= STATE_HOVERED; }
                        if (!m_table && !i) {
                            // Draw the cell background using appropriate state in treeline gutter when drawing treelines.
                            cell_x += node->depth * m_indent;
                            EmptyCell().draw(
                                dc,
                                Rect(cell_clip.x, cell_clip.y, node->depth * m_indent, cell_clip.h),
                                state
                            );
                        }
                        if (drawable->isWidget()) {
                            // Draw the cell background using appropriate state for cells with widgets in them.
                            EmptyCell().draw(
                                dc,
                                Rect(
                                    cell_x, pos.y, col_width > s.w ? col_width - m_grid_line_width : s.w - m_grid_line_width, node->max_cell_height - m_grid_line_width
                                ),
                                state
                            );
                            state = ((Widget*)drawable)->state();
                        }
                        drawable->draw(
                            dc,
                            Rect(
                                cell_x, pos.y, col_width > s.w ? col_width - m_grid_line_width : s.w - m_grid_line_width, node->max_cell_height - m_grid_line_width
                            ),
                            state
                        );
                    }
                    cell_start += col_width;
                    if (cell_start > drawing_rect.x + drawing_rect.w) {
                        break;
                    }
                }
                if (m_grid_lines == GridLines::Horizontal || m_grid_lines == GridLines::Both) {
                    dc.setClip(Rect(rect.x, rect.y + column_header, rect.w, rect.h - column_header).clipTo(tv_clip));
                    dc.fillRect(Rect(rect.x, pos.y + node->max_cell_height - m_grid_line_width, m_current_header_width, m_grid_line_width), dc.textDisabled(style));
                }
                drawTreeLine(dc, pos, rect, tv_clip, column_header, node);
                pos.y += node->max_cell_height;
            }

            TreeNode<T>* findNextNode(TreeNode<T> *node) {
                while (node->parent) {
                    if ((int)node->parent->children.size() - 1 > node->parent_index) {
                        return node->parent->children[node->parent_index + 1];
                    }
                    node = node->parent;
                }
                if (node->depth == 1) {
                    if ((int)m_model->roots.size() - 1 > node->parent_index) {
                        return m_model->roots[node->parent_index + 1];
                    }
                }

                return nullptr;
            }

            void drawTreeLine(DrawingContext &dc, Point pos, Rect rect, Rect tv_clip, int column_header, TreeNode<T> *node) {
                dc.setClip(Rect(rect.x, rect.y + column_header, m_column_widths[0], rect.h - column_header).clipTo(tv_clip));
                int x = pos.x + (node->depth * m_indent);
                int y = pos.y + (node->max_cell_height / 2);

                if (node->children.size()) {
                    // Draw a little line connecting the parent to its children.
                    // We do this so that the node status icon doesn't get drawn over.
                    if (!node->is_collapsed) {
                        dc.fillRect(
                            Rect(
                                x - (m_indent / 2) - (m_treeline_size / 2),
                                y,
                                m_treeline_size,
                                node->max_cell_height / 2
                            ),
                            dc.borderBackground(style)
                        );
                    }

                    if (node->parent) {
                        drawTreeLineConnector(dc, x, y);

                        // Lower sibling
                        if (node->parent_index < (int)node->parent->children.size() - 1) {
                            auto sibling = node->parent->children[node->parent_index + 1];
                            size_t distance = sibling->bs_data.position - node->bs_data.position;
                            // Sibling off screen
                            if (pos.y + (int)distance > rect.y + rect.h) {
                                dc.fillRect(
                                    Rect(
                                        x - (m_indent * 1.5) - (m_treeline_size / 2),
                                        y,
                                        m_treeline_size,
                                        (rect.y + rect.h) - pos.y
                                    ),
                                    dc.borderBackground(style)
                                );
                            }
                        // Higher sibling or no sibling
                        } else {
                            TreeNode<T> *sibling = nullptr;
                            size_t distance = 0;
                            if (node->parent->children.size() > 1 && node->parent_index > 0) {
                                sibling = node->parent->children[node->parent_index - 1];
                                distance = node->bs_data.position - sibling->bs_data.position;
                            } else {
                                sibling = node->parent;
                            }

                            // Sibling off screen
                            // NOTE: The reason for node->bs_data.length here is that
                            // pos.y is the top of the viewport not the beginning of the start node.
                            // So if pos.y is halfway through the start node then just the distance will not take
                            // us all the way to the beginning of the sibling and to keep it safe we use the
                            // entire height of the node rather than just the the different between pos.y and node->bs_data.position.
                            if (pos.y - (int)(distance + node->bs_data.length) <= rect.y + column_header) {
                                // When the higher sibling is off screen
                                // recursively go up the tree to root and draw a line
                                // between the parent and its last child.
                                // This is needed when not a single node directly related to the line
                                // is visible on screen but the line spans more than the screen.
                                auto _parent = node->parent;
                                while (_parent->parent) {
                                    _parent = _parent->parent;
                                    // We know here that the parent will have at least one child because
                                    // we are getting here from within the hierarchy.
                                    auto _node = _parent->children[_parent->children.size() - 1];
                                    dc.fillRect(
                                    Rect(
                                            pos.x + (_node->depth * m_indent) - (m_indent * 1.5) - (m_treeline_size / 2),
                                            pos.y - (node->bs_data.position - _parent->bs_data.position),
                                            m_treeline_size,
                                            _node->bs_data.position - _parent->bs_data.position
                                        ),
                                        dc.borderBackground(style)
                                    );
                                }
                            }
                        }

                        // Draw regular line to parent
                        drawTreeLineToParent(dc, x, pos.y, node);
                    }

                    Image *img = !node->is_collapsed ? m_expanded : m_collapsed;
                    Color fg = dc.iconForeground(style);
                    if (m_tree_collapser && m_tree_collapser == node) {
                        fg = dc.textSelected(style);
                    }
                    dc.drawTextureAligned(
                        Rect(x - m_indent, y - (node->max_cell_height / 2), m_indent, node->max_cell_height),
                        Size(m_indent / 2, m_indent / 2),
                        img->_texture(),
                        img->coords(),
                        HorizontalAlignment::Center,
                        VerticalAlignment::Center,
                        fg
                    );
                // End of the line.
                } else {
                    if (node->parent) {
                        drawTreeLineConnector(dc, x, y);
                        drawTreeLineToParent(dc, x, pos.y, node);
                        drawTreeLineNoChildrenIndicator(dc, pos.x, y, node);
                    }
                }
            }

            void drawTreeLineConnector(DrawingContext &dc, int x, int y) {
                dc.fillRect(
                    Rect(
                        x - (m_indent * 1.5) - (m_treeline_size / 2),
                        y,
                        m_indent,
                        m_treeline_size
                    ),
                    dc.borderBackground(style)
                );
            }

            void drawTreeLineToParent(DrawingContext &dc, int x, int y, TreeNode<T> *node) {
                size_t distance = node->bs_data.position - node->parent->bs_data.position;
                dc.fillRect(
                    Rect(
                        x - (m_indent * 1.5) - (m_treeline_size / 2),
                        y - (distance - node->parent->bs_data.length) - m_grid_line_width,
                        m_treeline_size,
                        (int)distance - node->parent->bs_data.length + (node->bs_data.length / 2) + m_grid_line_width
                    ),
                    dc.borderBackground(style)
                );
            }

            void drawTreeLineNoChildrenIndicator(DrawingContext &dc, int x, int y, TreeNode<T> *node) {
                dc.fillRect(
                    Rect(
                        x + ((node->depth - 1) * m_indent) + (m_indent / 3),
                        y - (m_indent / 8) + (m_treeline_size / 2),
                        m_indent / 4,
                        m_indent / 4
                    ),
                    dc.iconForeground(style)
                );
            }

            Widget* handleFocusEvent(FocusEvent event, State *state, FocusPropagationData data) override {
                return Widget::handleFocusEvent(event, state, data);
            }

            int isFocusable() override {
                return (int)FocusType::Focusable;
            }
    };
#endif
