#ifndef TREEVIEW_HPP
    #define TREEVIEW_HPP

    #define TREEVIEW_CONTINUE true
    #define TREEVIEW_EARLY_EXIT false

    #include <algorithm>

    #include "widget.hpp"
    #include "scrollable.hpp"
    #include "spacer.hpp"

    class CellRenderer {
        public:
            enum State {
                STATE_DEFAULT = 0x0000,
                STATE_HOVERED = 0x0010,
                STATE_SELECTED = 0x0100,
            };

            CellRenderer() {}
            virtual ~CellRenderer() {}
            virtual void draw(DrawingContext *dc, Rect rect, int state) = 0;
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
            HorizontalAlignment h_align = HorizontalAlignment::Left;
            VerticalAlignment v_align = VerticalAlignment::Center;

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

            virtual void draw(DrawingContext *dc, Rect rect, int state) override {
                Color fg = foreground;
                Color bg = background;
                if (state & STATE_SELECTED) {
                    fg = COLOR_WHITE;
                    bg = Color(0.2, 0.5, 1.0);
                }

                dc->fillRect(rect, bg);
                dc->fillTextAligned(
                    font ? font : dc->default_font,
                    text,
                    h_align,
                    v_align,
                    rect,
                    padding,
                    fg
                );

                if (state & STATE_HOVERED) {
                    dc->fillRect(rect, Color(0.4, 0.4, 0.4, 0.1));
                }
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

    class ImageCellRenderer : public CellRenderer {
        public:
            Image *image = nullptr;

            ImageCellRenderer(std::string file_path) {
                image = new Image(file_path);
            }

            ImageCellRenderer(bool from_memory, const unsigned char *image_data, int length) {
                image = new Image(from_memory, image_data, length);
            }

            ImageCellRenderer(std::shared_ptr<Texture> texture) {
                image = new Image(texture);
            }

            ImageCellRenderer(Image *img) {
                image = img;
            }

            ~ImageCellRenderer() {
                delete image;
            }

            virtual void draw(DrawingContext *dc, Rect rect, int state) override {
                dc->fillRect(
                    rect,
                    image->background()
                );
                dc->drawTextureAligned(
                    rect,
                    image->size(),
                    image->_texture(),
                    image->coords(),
                    HorizontalAlignment::Center,
                    VerticalAlignment::Center,
                    image->foreground()
                );
                if (state & STATE_SELECTED) {
                    dc->fillRect(rect, Color(0.2, 0.5, 1.0, 0.7));
                } else if (state & STATE_HOVERED) {
                    dc->fillRect(rect, Color(0.5, 0.5, 0.5, 0.1));
                }

            }
            
            virtual Size sizeHint(DrawingContext *dc) override {
                return image->size();
            }
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
                    node->depth = 1;
                    roots.push_back(node); 
                    
                    return node;
                } else {
                    parent_node->children.push_back(node);
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

            Column(
                std::string text, Image *image = nullptr, 
                HorizontalAlignment alignment = HorizontalAlignment::Center, 
                std::function<bool(TreeNode<T> *lhs, TreeNode<T> *rhs)> sort_function = nullptr) : Box(Align::Horizontal) {
                Widget::m_bg = Color(0.9, 0.9, 0.9);
                if (alignment == HorizontalAlignment::Right) {
                    this->append(new Spacer(), Fill::Both);
                }
                Button *b = new Button(text);
                    b->setImage(image);
                    b->setBackground(Color(0, 0, 0, 0));
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
                    Image *sort_icon = (new Image("up_arrow.png"))->setForeground(Color());
                        sort_icon->setMinSize(Size(12, 12));
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
                        Application::get()->setMouseCursor(Cursor::Default);
                    }
                });
                this->onMouseMotion.addEventListener([&](Widget *widget, MouseEvent event) {
                    if (m_dragging) {
                        setWidth(event.x - rect.x); // TODO take scroll into account?
                    } else {
                        if (event.x >= (rect.x + rect.w) - 5) {
                            Application::get()->setMouseCursor(Cursor::SizeWE);
                        } else {
                            Application::get()->setMouseCursor(Cursor::Default);
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
                dc->drawBorder(rect, style);

                dc->fillRect(rect, color);
                layoutChildren(dc, rect);
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
                        if (isSorted() == Sort::Ascending) {
                            std::sort(m_model->roots.rbegin(), m_model->roots.rend(), sort_fn);
                        } else {
                            std::sort(m_model->roots.begin(), m_model->roots.end(), sort_fn);
                        }
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
                unsigned int horizontal_non_expandable = 0;
                if (m_size_changed) {
                    Size size = Size();
                    for (Widget* child : children) {
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

                    dc->sizeHintBorder(size, style);

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

            float width() {
                if (m_custom_size) {
                    return m_custom_width;
                }
                return m_size.w;
            }

            void setWidth(float width) {
                if (width < m_min_width) {
                    return;
                }
                m_custom_size = true;
                m_custom_width = width;
                parentLayout(); // TODO ideally this would only affect the column on which it is called
            }

            void setColumnStyle(Style column, Style button) {
                setStyle(column);
                for (auto child : children) {
                    child->setStyle(button);
                }
            }

        protected:
            Sort m_sort = Sort::None;
            Tree<T> *m_model = nullptr;
            bool m_dragging = false;
            bool m_custom_size = false;
            float m_custom_width = 0.0;
            float m_min_width = 16;
    };

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

            TreeView(Size min_size = Size(100, 100)) : Scrollable(min_size) {
                this->onMouseMotion.addEventListener([&](Widget *widget, MouseEvent event) {
                    float y = inner_rect.y;
                    if (m_vertical_scrollbar) {
                        y -= m_vertical_scrollbar->m_slider->m_value * ((m_virtual_size.h) - inner_rect.h);
                    }
                    y += m_children_size.h;
                    if (event.x <= inner_rect.x + m_children_size.w) {
                        for (TreeNode<T> *root : m_model->roots) {
                            m_model->descend(root, [&](TreeNode<T> *node) -> bool {
                                if (event.y >= y && event.y <= y + node->max_cell_height) {
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
                                    return TREEVIEW_EARLY_EXIT;
                                }
                                return TREEVIEW_CONTINUE;
                            });
                        }
                    } else {
                        m_hovered = nullptr;
                        update();
                    }
                });
                this->onMouseClick.addEventListener([&](Widget *widget, MouseEvent event) {
                    float x = inner_rect.x;
                    float y = inner_rect.y;
                    if (m_horizontal_scrollbar) {
                        x -= m_horizontal_scrollbar->m_slider->m_value * ((m_virtual_size.w) - inner_rect.w);
                    }
                    if (m_vertical_scrollbar) {
                        y -= m_vertical_scrollbar->m_slider->m_value * ((m_virtual_size.h) - inner_rect.h);
                    }
                    y += m_children_size.h;
                    if (event.x <= inner_rect.x + m_children_size.w) {
                        for (TreeNode<T> *root : m_model->roots) {
                            m_model->descend(root, [&](TreeNode<T> *node) -> bool {
                                if (event.y >= y && event.y <= y + node->max_cell_height) {
                                    if (node->children.size() && (!m_table && (event.x >= x + (node->depth - 1) * m_indent && event.x <= x + node->depth * m_indent))) {
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
                                    return TREEVIEW_EARLY_EXIT;
                                }
                                return TREEVIEW_CONTINUE;
                            });
                        }
                    }
                });
                m_column_style = Style();
                m_column_style.border.type = STYLE_BOTTOM | STYLE_RIGHT;
                m_column_style.border.bottom = 1;
                m_column_style.border.right = 1;
                m_column_style.margin.type = STYLE_NONE;

                m_column_button_style = Style();
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

            virtual void draw(DrawingContext *dc, Rect rect) override {
                assert(m_model && "A TreeView needs a model to work!");
                this->rect = rect;

                dc->margin(rect, style);
                dc->drawBorder(rect, style);
                this->inner_rect = rect;
                dc->fillRect(rect, Color(1, 1, 1));

                Rect old_clip = dc->clip();
                Size virtual_size;
                if (m_virtual_size_changed) {
                    virtual_size = calculateVirtualSize(dc);
                } else {
                    virtual_size = m_virtual_size;
                }
                Point pos = automaticallyAddOrRemoveScrollBars(dc, rect, virtual_size);
                float y_start = pos.y;

                float local_pos_x = pos.x;
                for (Widget *child : children) {
                    Size s = child->sizeHint(dc);
                    Rect clip_rect = rect;
                        clip_rect.x = local_pos_x < rect.x ? rect.x : local_pos_x;
                        clip_rect.w = local_pos_x + s.w > rect.x + rect.w ? (rect.x + rect.w) - clip_rect.x : s.w;
                        clip_rect.h = m_children_size.h;
                    dc->setClip(clip_rect);
                    child->draw(dc, Rect(local_pos_x, rect.y, s.w, m_children_size.h));
                    local_pos_x += s.w;
                }
                pos.y += m_children_size.h;
                std::vector<TreeNode<T>*> tree_roots;
                for (TreeNode<T> *root : m_model->roots) {
                    bool collapsed = false;
                    int collapsed_depth = -1;

                    m_model->descend(root, [&](TreeNode<T> *node) -> bool {
                        if (node->depth <= collapsed_depth) {
                            collapsed = false;
                            collapsed_depth = -1;
                        }
                        if (!collapsed) {
                            if (pos.y + node->max_cell_height > rect.y + m_children_size.h && pos.y < rect.y + rect.h) {
                                if (!m_table) {
                                    TreeNode<T> *tree_root = node;
                                    while (tree_root) {
                                        if (!tree_root->parent) {
                                            break;
                                        }
                                        tree_root = tree_root->parent;
                                    }
                                    auto result = std::find(tree_roots.begin(), tree_roots.end(), tree_root);
                                    if (result == tree_roots.end()) {
                                        if (!tree_root->is_collapsed && tree_root->children.size()) {
                                            dc->setClip(Rect(rect.x, rect.y + m_children_size.h, rect.w, rect.h - m_children_size.h));
                                            float local_x = pos.x;
                                            float tree_line_start = y_start + m_children_size.h;
                                            if (m_model->roots[0] != tree_root) {
                                                for (TreeNode<T> *_root : m_model->roots) {
                                                    if (tree_root == _root) {
                                                        break;
                                                    }
                                                    m_model->descend(_root, [&](TreeNode<T> *node) {
                                                        if (!node->is_collapsed) {
                                                            tree_line_start += node->max_cell_height;
                                                        } else {
                                                            tree_line_start += node->max_cell_height;
                                                            return TREEVIEW_EARLY_EXIT;
                                                        }
                                                        return TREEVIEW_CONTINUE;
                                                    });
                                                }
                                            }

                                            m_model->descend(tree_root, [&](TreeNode<T> *node) {
                                                if (!node->is_collapsed) {
                                                    // TODO draw lines to children
                                                }
                                                // TODO can we early exit here?
                                                // does it interrupt current branch or only the next one?
                                                return TREEVIEW_CONTINUE;
                                            });

                                            // TODO look at drawTreeLinesToChildren from ts project for this next bit
                                            float line_height = 0;
                                            float last_node_height = 0;
                                            m_model->descend(tree_root, [&](TreeNode<T> *node) {
                                                line_height += node->max_cell_height;
                                                last_node_height = node->max_cell_height;
                                                return TREEVIEW_CONTINUE;
                                            });
                                            dc->fillRect(
                                                Rect(
                                                    local_x  + (tree_root->depth * m_indent) - (m_indent / 2), 
                                                    tree_line_start,// - (tree_root->max_cell_height / 2),
                                                    1, 
                                                    line_height// - last_node_height
                                                ), 
                                                Color(1, 0, 1)
                                            );
                                            // TODO draw tree lines
                                        }
                                        tree_roots.push_back(tree_root);
                                    }

                                    // Clip and draw the collapsible "button".
                                    dc->setClip(
                                        Rect(
                                            rect.x, 
                                            pos.y > rect.y + m_children_size.h ? pos.y : rect.y + m_children_size.h, 
                                            m_column_widths[0], 
                                            node->max_cell_height
                                        )
                                    );
                                    // End of the line.
                                    if (node->children.size()) {
                                        if (node->is_collapsed) {
                                            dc->drawTextureAligned(
                                                Rect(pos.x + (node->depth - 1) * m_indent, pos.y, m_indent, node->max_cell_height),
                                                Size(m_indent / 2, m_indent / 2),
                                                m_collapsed->_texture(),
                                                m_collapsed->coords(),
                                                HorizontalAlignment::Center,
                                                VerticalAlignment::Center,
                                                m_collapsed->foreground()
                                            );
                                        } else {
                                            dc->drawTextureAligned(
                                                Rect(pos.x + (node->depth - 1) * m_indent, pos.y, m_indent, node->max_cell_height),
                                                Size(m_indent / 2, m_indent / 2),
                                                m_expanded->_texture(),
                                                m_expanded->coords(),
                                                HorizontalAlignment::Center,
                                                VerticalAlignment::Center,
                                                m_expanded->foreground()
                                            );
                                        }
                                    } else {
                                        // TODO an image indicating that this node is the end of the line could go here
                                    }
                                }

                                float cell_start = pos.x;
                                for (size_t i = 0; i < node->columns.size(); i++) {
                                    float col_width = m_column_widths[i];
                                    CellRenderer *renderer = node->columns[i];
                                    Size s = renderer->sizeHint(dc);
                                    if (cell_start + col_width > rect.x && cell_start < rect.x + rect.w) {
                                        Rect cell_clip = Rect(cell_start, pos.y, col_width, node->max_cell_height);
                                        if (cell_start + col_width > rect.x + rect.w) {
                                            cell_clip.w = (rect.x + rect.w) - cell_start;
                                        }
                                        if (pos.y + node->max_cell_height > rect.y + rect.h) {
                                            cell_clip.h = rect.h - pos.y + node->max_cell_height;
                                        }
                                        if (cell_start + col_width > rect.x && !(cell_start > rect.x)) {
                                            cell_clip.x = rect.x;
                                            cell_clip.w = cell_start + col_width - rect.x;
                                        }
                                        if (pos.y + node->max_cell_height > rect.y + m_children_size.h && !(pos.y > rect.y + m_children_size.h)) {
                                            cell_clip.y = rect.y + m_children_size.h;
                                            cell_clip.h = pos.y + node->max_cell_height - rect.y - m_children_size.h;
                                        }
                                        // Clip and draw the current cell.
                                        dc->setClip(cell_clip);
                                        float cell_x = cell_start;
                                        if (!m_table && !i) {
                                            cell_x += node->depth * m_indent;
                                        }
                                        int state = CellRenderer::State::STATE_DEFAULT;
                                        if (m_selected == node) {
                                            state = state | CellRenderer::State::STATE_SELECTED;
                                        }
                                        if (m_hovered == node) {
                                            state = state | CellRenderer::State::STATE_HOVERED;
                                        }
                                        renderer->draw(
                                            dc,
                                            Rect(
                                                cell_x, pos.y, col_width > s.w ? col_width : s.w, node->max_cell_height
                                            ),
                                            state
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
                                dc->setClip(Rect(rect.x, rect.y + m_children_size.h, rect.w, rect.h - m_children_size.h));
                                dc->fillRect(Rect(rect.x, pos.y - 1, m_children_size.w, 1), Color(0.85, 0.85, 0.85));
                            }
                        }

                        if (node->is_collapsed && !collapsed) {
                            collapsed = true;
                            collapsed_depth = node->depth;
                        }
                        if (pos.y > rect.y + rect.h) {
                            return TREEVIEW_EARLY_EXIT;
                        }
                        return TREEVIEW_CONTINUE;
                    });
                }
                if (m_model->roots.size()) {
                    // Clip and draw column grid lines.
                    if (m_grid_lines == GridLines::Vertical || m_grid_lines == GridLines::Both) {
                        dc->setClip(Rect(rect.x, rect.y + m_children_size.h, rect.w, rect.h - m_children_size.h));
                        for (float width : m_column_widths) {
                            dc->fillRect(Rect(pos.x + width - 1, rect.y + m_children_size.h, 1, m_virtual_size.h - m_children_size.h), Color(0.85, 0.85, 0.85));
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
                    ((Column<T>*)widget)->setColumnStyle(m_column_style, m_column_button_style);
                }
                m_virtual_size_changed = true;
                m_auto_size_columns = true;
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

            void select(int index) {
                int i = 0;
                bool early = false;
                for (TreeNode<T> *root : m_model->roots) {
                    if (early) {
                        return;
                    }
                    m_model->descend(root, [&](TreeNode<T> *node) -> bool {
                        if (i == index) {
                            select(node);
                            early = true;
                            return TREEVIEW_EARLY_EXIT;
                        }
                        i++;
                        return TREEVIEW_CONTINUE;
                    });
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
                    m_virtual_size_changed = true;
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
                    m_virtual_size_changed = true;
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

            virtual Size sizeHint(DrawingContext *dc) override {
                if (m_size_changed) { // TODO this will be slow for a large number of columns
                    m_virtual_size.w = 0.0;
                    m_column_widths.clear();
                    Size size = Size();
                    for (Widget *child : children) {
                        Size s = child->sizeHint(dc);
                        m_column_widths.push_back(s.w);
                        size.w += s.w;
                        if (s.h > size.h) {
                            size.h = s.h;
                        }
                    }
                    m_children_size = size;
                    m_virtual_size.w = size.w;
                    m_size_changed = false;
                }
                Size viewport_and_style = m_viewport;
                    dc->sizeHintMargin(viewport_and_style, style);
                    dc->sizeHintBorder(viewport_and_style, style);
                return viewport_and_style;
            }

            bool isTable() {
                return m_table;
            }

            void setTableMode(bool table) {
                m_table = table;
                update();
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
            Size m_children_size = Size();
            std::vector<float> m_column_widths;
            bool m_auto_size_columns = false;
            bool m_table = false;
            Style m_column_style;
            Style m_column_button_style;
            std::shared_ptr<Texture> m_arrow = std::make_shared<Texture>("up_arrow.png");
            Image *m_collapsed = (new Image(m_arrow))->clockwise90()->setForeground(COLOR_BLACK);
            Image *m_expanded = (new Image(m_arrow))->flipVertically()->setForeground(COLOR_BLACK);

            void collapseOrExpandRecursively(TreeNode<T> *node, bool is_collapsed) {
                if (node) {
                    m_model->descend(node, [&](TreeNode<T> *_node){
                        _node->is_collapsed = is_collapsed;
                        return TREEVIEW_CONTINUE;
                    });
                    m_virtual_size_changed = true;
                    update();
                }
            }

            void collapseOrExpandAll(bool is_collapsed) {
                for (TreeNode<T> *root : m_model->roots) {
                    m_model->descend(root, [&](TreeNode<T> *node){
                        node->is_collapsed = is_collapsed;
                        return TREEVIEW_CONTINUE;
                    });
                }
                m_virtual_size_changed = true;
                update();
            }

            Size calculateVirtualSize(DrawingContext *dc) {
                Size virtual_size = m_children_size;
                for (TreeNode<T> *root : m_model->roots) {
                    void *previous_parent = nullptr;
                    bool collapsed = false;
                    int collapsed_depth = -1;

                    m_model->descend(root, [&](TreeNode<T> *node) {
                        if (node->depth <= collapsed_depth) {
                            collapsed = false;
                            collapsed_depth = -1;
                        }
                        if (!collapsed) {
                            // Check and set the max height of the node.
                            int index = 0;
                            for (CellRenderer *renderer : node->columns) {
                                Size s = renderer->sizeHint(dc);
                                Column<T> *col = (Column<T>*)children[index];
                                if (!m_table && !index) {
                                    s.w += node->depth * indent();
                                }
                                
                                // Automatically set the columns to be wide
                                // enough for their contents.
                                if (m_auto_size_columns && s.w > col->width()) {
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
                                    node->max_cell_height = s.h;
                                }
                                index++;
                            }
                            virtual_size.h += node->max_cell_height;
                        }

                        if (node->is_collapsed && !collapsed) {
                            collapsed = true;
                            collapsed_depth = node->depth;
                        }
                        return TREEVIEW_CONTINUE;
                    });
                }
                virtual_size.w = m_children_size.w;
                m_virtual_size = virtual_size;
                m_virtual_size_changed = false;
                m_auto_size_columns = false;

                return virtual_size;
            }
    };
#endif