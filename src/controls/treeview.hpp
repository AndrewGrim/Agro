#ifndef TREEVIEW_HPP
    #define TREEVIEW_HPP

    #include <algorithm>

    #include "widget.hpp"
    #include "scrollable.hpp"

    class CellRenderer {
        public:
            CellRenderer() {}
            virtual ~CellRenderer() {}
            virtual void draw(DrawingContext *dc, Rect rect) = 0;
            virtual Size sizeHint(DrawingContext *dc) = 0;
    };

    class TextCellRenderer : public CellRenderer {
        public:
            // TODO setters for text, alignment, padding, font to set m_size_changed
            std::string text;
            Color foreground;
            Color background;
            int padding;
            Font *font = nullptr;
            HorizontalAlignment align = HorizontalAlignment::Left;

            TextCellRenderer(
                    std::string text, 
                    Color foreground = Color(), 
                    Color background = Color(0, 0, 0, 0), 
                    int padding = 10
                ) {
                this->text = text;
                this->foreground = foreground;
                this->background = background;
                this->padding = padding;
            }

            ~TextCellRenderer() {

            }

            virtual void draw(DrawingContext *dc, Rect rect) override {
                dc->fillRect(rect, background);
                dc->fillTextAligned(
                    font ? font : dc->default_font,
                    text,
                    align,
                    VerticalAlignment::Center,
                    rect,
                    padding,
                    foreground
                );
            }

            virtual Size sizeHint(DrawingContext *dc) override {
                if (m_size_changed) {
                    Size s = dc->measureText(font ? font : dc->default_font, text);
                        s.w += padding * 2;
                        s.h += padding * 2;
                    m_size = s;
                    return s;
                } else {
                    return m_size;
                }
            }

        protected:
            Size m_size;
            bool m_size_changed = true;
    };

    template <typename T> class TreeNode {
        public:
            std::vector<CellRenderer*> columns;
            T *hidden;
            TreeNode<T> *parent;
            std::vector<TreeNode<T>*> children;
            bool is_collapsed = false;
            float max_cell_height = 0.0;
            int depth = 0;

            TreeNode(std::vector<CellRenderer*> columns, T *hidden) {
                this->columns = columns;
                this->hidden = hidden;
            }

            ~TreeNode() {
                for (TreeNode<T> *child : children) {
                    delete child;
                }
                for (CellRenderer *renderer : columns) {
                    delete renderer;
                }
                delete hidden;
            }
    };

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
                    roots.push_back(node); 
                    
                    return node;
                } else {
                    parent_node->children.push_back(node);
                    node->parent = parent_node;

                    return node;
                }
            }

            void clear() {
                for (TreeNode<T> *root : roots) {
                    delete root;
                }
                roots.clear();
            }

            TreeNode<T>* descend(TreeNode<T> *root, std::function<bool(TreeNode<T> *node)> fn = nullptr) {
                if (fn) {
                    if (!fn(root)) {
                        return root;
                    }
                }
                if (root->children.size()) {
                    TreeNode<T> *last = nullptr;
                    for (TreeNode<T> *child : root->children) {
                        last = descend(child, fn);
                    }
                    return last;
                } else {
                    return root;
                }
            }
            // TreeNode find(compare function);
            // Goes down the tree using row (skips non visible nodes)
            // TreeNode get(size_t row);
            // TreeNode ascend(TreeNode leaf, std::function<void(TreeNode node)> callback = nullptr);
    };

    enum class Sort {
        None,
        Ascending,
        Descending
    };

    template <typename T> class Column : public Box {
        public:
            std::function<bool(TreeNode<T> *lhs, TreeNode<T> *rhs)> sort_fn = nullptr;

            Column(std::function<bool(TreeNode<T> *lhs, TreeNode<T> *rhs)> sort_function = nullptr, Align alignment = Align::Horizontal) : Box(alignment) {
                Widget::m_bg = Color(0.9, 0.9, 0.9);
                this->sort_fn = sort_function;
                this->onMouseDown.addEventListener([&](Widget *widget, MouseEvent event) {
                    if (event.x >= (rect.x + rect.w) - 5) {
                        m_dragging = true;
                        m_custom_width = m_custom_size ? m_custom_width : m_size.w;
                        m_custom_size = true;
                        ((Application*)app)->setMouseCursor(Cursor::SizeWE);
                    }
                });
                this->onMouseClick.addEventListener([&](Widget *widget, MouseEvent event) {
                    if (!m_dragging && sort_fn) {
                        if (m_sort == Sort::None) {
                            children[children.size() - 1]->show();
                            m_sort = Sort::Ascending;
                        } else if (m_sort == Sort::Ascending) {
                            Image *sort_icon = (Image*)children[children.size() - 1];
                                sort_icon->flipVertically();
                            m_sort = Sort::Descending;
                        } else {
                            Image *sort_icon = (Image*)children[children.size() - 1];
                                sort_icon->flipVertically();
                            m_sort = Sort::Ascending;
                        }
                        sort(m_sort);
                    } else {
                        m_dragging = false;
                        ((Application*)app)->setMouseCursor(Cursor::Default);
                    }
                });
                this->onMouseMotion.addEventListener([&](Widget *widget, MouseEvent event) {
                    if (m_dragging) {
                        m_custom_width = event.x - rect.x;
                        // TODO when changing the width we need to notify tv
                        // so that it can recalculate the virtual width 
                        if (m_custom_width < m_min_width) {
                            m_custom_width = m_min_width; 
                        }
                        m_size_changed = true;
                        update();
                    } else {
                        if (event.x >= (rect.x + rect.w) - 5) {
                            ((Application*)app)->setMouseCursor(Cursor::SizeWE);
                        } else {
                            ((Application*)app)->setMouseCursor(Cursor::Default);
                        }
                    }
                });
                this->onMouseLeft.addEventListener([&](Widget *widget, MouseEvent event) {
                    if (m_dragging) {
                        m_dragging = false;
                    }
                    ((Application*)app)->setMouseCursor(Cursor::Default);
                });
            }

            ~Column() {

            }

            virtual const char* name() override {
                return "Column";
            }

            virtual void draw(DrawingContext *dc, Rect rect) override {
                this->rect = rect;
                Color color;
                if (m_dragging) {
                    color = this->background();
                } else if (this->isPressed() && this->isHovered()) {
                    color = this->m_pressed_bg; 
                } else if (this->isHovered()) {
                    color = this->m_hovered_bg;
                } else {
                    color = this->background();
                }
                // TODO border, for now we are skipping working on this
                // because ideally i would like to add padding, border, margins
                // to all widgets through a styling system.
                Rect old_clip = dc->clip();
                dc->setClip(rect);
                dc->fillRect(rect, color);
                layoutChildren(dc, rect);
                dc->setClip(old_clip);
            }

            virtual bool isLayout() override {
                return false;
            }

            void sort(Sort sort) {
                if (sort_fn) {
                    if (sort == Sort::None) {
                        Image *sort_icon = (Image*)children[children.size() - 1];
                            sort_icon->hide();
                        if (m_sort == Sort::Descending) {
                            sort_icon->flipVertically();
                        }
                        m_sort = sort;
                    } else {
                        assert(m_model && "Model cannot be null when sorting! Only sort once youve set the model.");
                        std::sort(m_model->roots.begin(), m_model->roots.end(), sort_fn);
                    }
                }
            }

            Sort isSorted() {
                return m_sort;
            }

            void setModel(Tree<T> *model) {
                m_model = model;
            }

            virtual Size sizeHint(DrawingContext *dc) override {
                unsigned int visible = 0;
                unsigned int vertical_non_expandable = 0;
                unsigned int horizontal_non_expandable = 0;
                if (this->m_size_changed || ((Application*)this->app)->hasLayoutChanged()) {
                    Size size = Size();
                    if (this->m_align_policy == Align::Horizontal) {
                        for (Widget* child : this->children) {
                            if (child->isVisible()) {
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
                        }
                    } else {
                        for (Widget* child : this->children) {
                            if (child->isVisible()) {
                                Size s = child->sizeHint(dc);
                                size.h += s.h;
                                if (s.w > size.w) {
                                    size.w = s.w;
                                }
                                visible += child->proportion();
                                if (child->fillPolicy() == Fill::Horizontal || child->fillPolicy() == Fill::None) {
                                    vertical_non_expandable++;
                                }
                            }
                        }
                    }
                    this->m_vertical_non_expandable = vertical_non_expandable;
                    this->m_horizontal_non_expandable = horizontal_non_expandable;
                    this->m_visible_children = visible;
                    this->m_size = size;
                    this->m_size_changed = false;

                    if (m_custom_size) {
                        return Size(m_custom_width, m_size.h);
                    }
                    return size;
                } else {
                    if (m_custom_size) {
                        return Size(m_custom_width, m_size.h);
                    }
                    return this->m_size;
                }
            }

        protected:
            Sort m_sort = Sort::None;
            Tree<T> *m_model = nullptr;
            bool m_dragging = false;
            bool m_custom_size = false;
            float m_custom_width = 0.0;
            float m_min_width = 15;
    };

    // expand col to content??
    // image cache / manager
    // tooltips (kinda important since we want some headings to be image only), but treeview specific or lib wide?

    enum class GridLines {
        None,
        Horizontal,
        Vertical,
        Both,
    };

    template <typename T> class TreeView : public Scrollable {
        public:
            std::function<void(TreeView<T> *treeview, TreeNode<T> *node)> onNodeHovered = nullptr;
            std::function<void(TreeView<T> *treeview, TreeNode<T> *node)> onNodeSelected = nullptr;
            std::function<void(TreeView<T> *treeview, TreeNode<T> *node)> onNodeDeselected = nullptr;
            std::function<void(TreeView<T> *treeview, TreeNode<T> *node)> onNodeCollapsed = nullptr;
            std::function<void(TreeView<T> *treeview, TreeNode<T> *node)> onNodeExpanded = nullptr;

            TreeView(Size min_size = Size(400, 400)) : Scrollable(min_size) {
                this->onMouseMotion.addEventListener([&](Widget *widget, MouseEvent event) {
                    float y = rect.y;
                    if (m_vertical_scrollbar) {
                        y -= m_vertical_scrollbar->m_slider->m_value * ((m_virtual_size.h) - rect.h);
                    }
                    Size children_size = Size();
                    std::vector<float> column_widths;
                    DrawingContext *dc = ((Application*)this->app)->dc;
                    for (Widget *child : children) {
                        // TODO we could probably store this somewhere, so we dont have to recompute it all the time
                        Size s = child->sizeHint(dc);
                        column_widths.push_back(s.w);
                        children_size.w += s.w;
                        if (s.h > children_size.h) {
                            children_size.h = s.h;
                        }
                    }
                    y += children_size.h;
                    for (TreeNode<T> *root : m_model->roots) {
                        m_model->descend(root, [&](TreeNode<T> *node) -> bool {
                            if (event.x <= rect.x + children_size.w && (event.y >= y && event.y <= y + node->max_cell_height)) {
                                if (m_hovered != node) {
                                    m_hovered = node;
                                    if (onNodeHovered) {
                                        onNodeHovered(this, node);
                                    }
                                    update();
                                }
                            }
                            y += node->max_cell_height;
                            if (node->is_collapsed) {
                                return false;
                            }
                            return true;
                        });
                    }
                });
                this->onMouseClick.addEventListener([&](Widget *widget, MouseEvent event) {
                    float x = rect.x;
                    float y = rect.y;
                    if (m_horizontal_scrollbar) {
                        x -= m_horizontal_scrollbar->m_slider->m_value * ((m_virtual_size.w) - rect.w);
                    }
                    if (m_vertical_scrollbar) {
                        y -= m_vertical_scrollbar->m_slider->m_value * ((m_virtual_size.h) - rect.h);
                    }
                    Size children_size = Size();
                    std::vector<float> column_widths;
                    DrawingContext *dc = ((Application*)this->app)->dc;
                    for (Widget *child : children) {
                        // TODO we could probably store this somewhere
                        Size s = child->sizeHint(dc);
                        column_widths.push_back(s.w);
                        children_size.w += s.w;
                        if (s.h > children_size.h) {
                            children_size.h = s.h;
                        }
                    }
                    y += children_size.h;
                    for (TreeNode<T> *root : m_model->roots) {
                        m_model->descend(root, [&](TreeNode<T> *node) -> bool {
                            if (event.x <= rect.x + children_size.w && (event.y >= y && event.y <= y + node->max_cell_height)) {
                                if (event.x >= x + (node->depth - 1) * m_indent && event.x <= x + node->depth * m_indent) {
                                    if (node->is_collapsed) {
                                        expand(node);
                                    } else {
                                        collapse(node);
                                    }
                                } else {
                                    select(node);
                                }
                            }
                            y += node->max_cell_height;
                            if (node->is_collapsed) {
                                return false;
                            }
                            return true;
                        });
                    }
                });
            }

            ~TreeView() {
                delete m_model;
            }

            virtual const char* name() override {
                return "TreeView";
            }

            virtual void draw(DrawingContext *dc, Rect rect) override {
                assert(m_model && "A TreeView needs a model to work!");
                this->rect = rect;
                Rect old_clip = dc->clip();
                Size children_size = Size();
                std::vector<float> column_widths;
                for (Widget *child : children) {
                    Size s = child->sizeHint(dc);
                    column_widths.push_back(s.w);
                    children_size.w += s.w;
                    if (s.h > children_size.h) {
                        children_size.h = s.h;
                    }
                }
                // TODO expand any column headers as needed ?option? ie if the content of the cell is longer than the column header do you want to expand the column header?
                Size virtual_size = children_size;
                // TODO note that this doesnt take into account when the columns themselves change
                // like when making the sort icon visible
                if (m_virtual_size_changed) {
                    std::unordered_map<void*, int> depth_map;
                    for (TreeNode<T> *root : m_model->roots) {
                        depth_map.clear();
                        int depth = 0;
                        void *previous_parent = nullptr;
                        m_model->descend(root, [&](TreeNode<T> *node) {
                            // Check and set the depth of each node.
                            if (node->parent != previous_parent || !node->parent) {
                                if (!node->parent) {
                                    depth += 1;
                                    depth_map.insert(std::make_pair(node->parent, depth));
                                    node->depth = depth;
                                } else {
                                    std::unordered_map<void*, int>::iterator iter = depth_map.find(node->parent);
                                    if (iter != depth_map.end()) {
                                        depth = iter->second;
                                        node->depth = depth;
                                    } else {
                                        depth += 1;
                                        depth_map.insert(std::make_pair(node->parent, depth));
                                        node->depth = depth;
                                    }
                                }
                                previous_parent = node->parent;
                            }

                            // Check and set the max height of the node.
                            for (CellRenderer *renderer : node->columns) {
                                Size s = renderer->sizeHint(dc);
                                if (s.h > node->max_cell_height) {
                                    node->max_cell_height = s.h;
                                }
                            }
                            virtual_size.h += node->max_cell_height;
                            return true;
                        });
                    }
                    m_virtual_size = virtual_size;
                    m_virtual_size_changed = false;
                } else {
                    virtual_size = m_virtual_size;
                }
                Point pos = automaticallyAddOrRemoveScrollBars(dc, rect, virtual_size);
                dc->fillRect(rect, Color(1, 1, 1));
                float local_pos_x = pos.x;
                dc->setClip(rect);
                for (Widget *child : children) {
                    Size s = child->sizeHint(dc);
                    child->draw(dc, Rect(local_pos_x, rect.y, s.w, children_size.h));
                    local_pos_x += s.w;
                }
                pos.y += children_size.h;
                for (TreeNode<T> *root : m_model->roots) {
                    bool collapsed = false;
                    int collapsed_depth = -1;

                    m_model->descend(root, [&](TreeNode<T> *node) -> bool {
                        if (node->depth <= collapsed_depth) {
                            collapsed = false;
                            collapsed_depth = -1;
                        }
                        if (!collapsed) {
                            if (pos.y + node->max_cell_height > rect.y + children_size.h && pos.y < rect.y + rect.h) {
                                // Clip and draw selection and or hover. 
                                if (m_selected == node) { // TODO we might want to think about drawing the selection and hover after drawing the cell itself
                                    dc->setClip(Rect(rect.x, rect.y + children_size.h, children_size.w, rect.h));
                                    dc->fillRect(Rect(rect.x, pos.y, children_size.w, node->max_cell_height), Color(0.2, 0.5, 1.0));
                                } else if (m_hovered == node) {
                                    dc->setClip(Rect(rect.x, rect.y + children_size.h, children_size.w, rect.h));
                                    dc->fillRect(Rect(rect.x, pos.y, children_size.w, node->max_cell_height), Color(0.5, 0.5, 0.5, 0.1));
                                }
                                // Clip and draw the collapsible "button".
                                dc->setClip(
                                    Rect(
                                        rect.x, 
                                        pos.y > rect.y + children_size.h ? pos.y : rect.y + children_size.h, 
                                        column_widths[0], 
                                        node->max_cell_height
                                    )
                                );
                                // End of the line.
                                if (node->children.size()) {
                                    dc->fillRect(Rect(pos.x + (node->depth - 1) * m_indent, pos.y, m_indent, node->max_cell_height), Color(0, 1));
                                } else {
                                    dc->fillRect(Rect(pos.x + (node->depth - 1) * m_indent, pos.y, m_indent, node->max_cell_height), Color(1, 0, 1));
                                }

                                float cell_start = pos.x;
                                for (size_t i = 0; i < node->columns.size(); i++) {
                                    float col_width = column_widths[i];
                                    CellRenderer *renderer = node->columns[i];
                                    Size s = renderer->sizeHint(dc);
                                    if (cell_start + col_width > rect.x && cell_start < rect.x + rect.w) {
                                        Rect cell_clip = Rect(cell_start, pos.y, col_width, node->max_cell_height);
                                        if (cell_start + col_width > rect.x && !(cell_start > rect.x)) {
                                            cell_clip.x = rect.x;
                                            cell_clip.w = cell_start + col_width - rect.x;
                                        }
                                        if (pos.y + node->max_cell_height > rect.y + children_size.h && !(pos.y > rect.y + children_size.h)) {
                                            cell_clip.y = rect.y + children_size.h;
                                            cell_clip.h = pos.y + node->max_cell_height - rect.y - children_size.h;
                                        }
                                        // Clip and draw the current cell.
                                        dc->setClip(cell_clip);
                                        float cell_x = cell_start;
                                        if (!i) {
                                            cell_x += node->depth * m_indent;
                                        }
                                        renderer->draw(
                                            dc,
                                            Rect(
                                                cell_x, pos.y, col_width > s.w ? col_width : s.w, node->max_cell_height
                                            )
                                        );
                                    }
                                    cell_start += col_width;
                                    if (cell_start > rect.x + rect.w) {
                                        break;
                                    }
                                }
                            }
                            pos.y += node->max_cell_height;
                            // Clip and draw row grid line.
                            if (m_grid_lines == GridLines::Horizontal || m_grid_lines == GridLines::Both) {
                                dc->setClip(Rect(rect.x, rect.y + children_size.h, rect.w, rect.h));
                                dc->fillRect(Rect(rect.x, pos.y - 1, children_size.w, 1), Color(0.85, 0.85, 0.85));
                            }
                        }

                        if (node->is_collapsed && !collapsed) {
                            collapsed = true;
                            collapsed_depth = node->depth;
                        }
                        if (pos.y > rect.y + rect.h) {
                            return false;
                        }
                        return true;
                    });
                }
                if (m_model->roots.size()) {
                    // Clip and draw column grid lines.
                    if (m_grid_lines == GridLines::Vertical || m_grid_lines == GridLines::Both) {
                        dc->setClip(Rect(rect.x, rect.y + children_size.h, rect.w, pos.y - (children_size.h * 2)));
                        for (float width : column_widths) {
                            dc->fillRect(Rect(pos.x + width - 1, rect.y + children_size.h, 1, pos.y - (children_size.h * 2) - 4), Color(0.85, 0.85, 0.85));
                            pos.x += width;
                        }
                    }
                }
                dc->setClip(old_clip);
                drawScrollBars(dc, rect, virtual_size);
            }

            void setModel(Tree<T> *model) {
                m_model = model;
                for (Widget *widget : children) {
                    ((Column<T>*)widget)->setModel(model);
                }
                m_virtual_size_changed = true;
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
                    m_selected = nullptr;
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
                return m_selected;
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
                if (m_selected != node) {
                    deselect();
                    m_selected = node;
                    if (onNodeSelected) {
                        onNodeSelected(this, node);
                    }
                    update();
                } else {
                    deselect();
                }
            }

            void deselect() {
                if (m_selected) {
                    if (onNodeDeselected) {
                        onNodeDeselected(this, m_selected);
                    }
                    m_selected = nullptr;
                    update();
                }
            }

            void collapse(TreeNode<T> *node) {
                if (node && node->children.size()) {
                    node->is_collapsed = true;
                    if (onNodeCollapsed) {
                        onNodeCollapsed(this, node);
                    }
                    update();
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
                    if (onNodeExpanded) {
                        onNodeExpanded(this, node);
                    }
                    update();
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
                // TODO the below might be better off in column ctor instead
                if (column->sort_fn) {
                    Image *sort_icon = (new Image("up_arrow.png"))->setForeground(Color());
                        sort_icon->width = 12;
                        sort_icon->height = 12;
                        sort_icon->hide();
                    column->append(sort_icon, Fill::Both);
                }
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
                if (app) column->app = app;
                m_size_changed = true;
                update();
                layout();

                return this;
            }

        protected:
            Tree<T> *m_model = nullptr;
            Size m_virtual_size;
            bool m_virtual_size_changed = false;
            uint8_t m_indent = 24;
            TreeNode<T> *m_hovered = nullptr;
            TreeNode<T> *m_selected = nullptr;
            GridLines m_grid_lines = GridLines::Both;
            Column<T> *m_last_sort = nullptr;

            void collapseOrExpandRecursively(TreeNode<T> *node, bool is_collapsed) {
                if (node) {
                    m_model->descend(node, [&](TreeNode<T> *_node){
                        _node->is_collapsed = is_collapsed;
                        return true;
                    });
                    update();
                }
            }

            void collapseOrExpandAll(bool is_collapsed) {
                for (TreeNode<T> *root : m_model->roots) {
                    m_model->descend(root, [&](TreeNode<T> *node){
                        node->is_collapsed = is_collapsed;
                        return true;
                    });
                }
                update();
            }
    };
#endif