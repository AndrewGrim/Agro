#ifndef TREEVIEW_HPP
    #define TREEVIEW_HPP

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

    // min col width
    // expand col to content??
    // image cache / manager
    // headings / columns
    // allow to drag columns to resize them?? (low priority)
    // current sorted column
    // sort type ie asc, desc
    // tooltips (kinda important since we want some headings to be image only), but treeview specific or lib wide?

    enum class GridLines {
        None,
        Horizontal,
        Vertical,
        Both,
    };

    template <typename T> class TreeView : public Scrollable {
        public:
            std::function<void(TreeNode<T>*)> onNodeHovered = nullptr;
            std::function<void(TreeNode<T>*)> onNodeSelected = nullptr;
            std::function<void(TreeNode<T>*)> onNodeDeselected = nullptr;
            std::function<void(TreeNode<T>*)> onNodeCollapsed = nullptr;
            std::function<void(TreeNode<T>*)> onNodeExpanded = nullptr;

            TreeView(Size min_size = Size(400, 400)) : Scrollable(min_size) {
                this->onMouseMotion = [&](MouseEvent event) {
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
                                        onNodeHovered(node);
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
                };
                this->onMouseClick = [&](MouseEvent event) {
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
                                    if (node->children.size()) {
                                        if (node->is_collapsed) {
                                            node->is_collapsed = false;
                                            if (onNodeExpanded) {
                                                onNodeExpanded(node);
                                            }
                                            update();
                                        } else {
                                            node->is_collapsed = true;
                                            if (onNodeCollapsed) {
                                                onNodeCollapsed(node);
                                            }
                                            update();
                                        }
                                    }
                                } else {
                                    if (m_selected != node) {
                                        if (m_selected && onNodeDeselected) {
                                            onNodeDeselected(node);
                                        }
                                        m_selected = node;
                                        if (onNodeSelected) {
                                            onNodeSelected(node);
                                        }
                                        update();
                                    } else {
                                        m_selected = nullptr;
                                        if (onNodeDeselected) {
                                            onNodeDeselected(node);
                                        }
                                        update();
                                    }
                                }
                            }
                            y += node->max_cell_height;
                            if (node->is_collapsed) {
                                return false;
                            }
                            return true;
                        });
                    }
                };
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
                // TODO then maybe next step is to make the columns resizable using the mouse?
                Size virtual_size = children_size;
                // TODO note that this doesnt take into account when the columns themselves change
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
                                        node->depth * m_indent, 
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
                        dc->setClip(Rect(rect.x, rect.y + children_size.h, rect.w, rect.h));
                        for (float width : column_widths) {
                            dc->fillRect(Rect(pos.x + width - 1, rect.y, 1, pos.y - children_size.h), Color(0.85, 0.85, 0.85));
                            pos.x += width;
                        }
                    }
                }
                dc->setClip(old_clip);
                drawScrollBars(dc, rect, virtual_size);
            }

            void setModel(Tree<T> *model) {
                m_model = model;
                m_virtual_size_changed = true;
                update();
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
            const TreeNode<T>* hovered() {
                return m_hovered;
            }

            /// Can return null.
            const TreeNode<T>* selected() {
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

        protected:
            Tree<T> *m_model = nullptr;
            Size m_virtual_size;
            bool m_virtual_size_changed = false;
            uint8_t m_indent = 24;
            // TODO reset when a column header gets hovered?
            TreeNode<T> *m_hovered = nullptr;
            TreeNode<T> *m_selected = nullptr;
            GridLines m_grid_lines = GridLines::Both; 
    };
#endif