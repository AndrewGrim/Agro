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
            std::string text;
            int padding;
            Color foreground;
            Color background;

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
                    dc->default_font,
                    text,
                    HorizontalAlignment::Center,
                    VerticalAlignment::Center,
                    rect,
                    padding,
                    foreground
                );
            }

            virtual Size sizeHint(DrawingContext *dc) override {
                Size s = dc->measureText(dc->default_font, text);
                    s.w += padding * 2;
                    s.h += padding * 2;
                return s;
            }
    };

    template <typename T> class TreeNode {
        public:
            std::vector<CellRenderer*> columns;
            T *hidden;
            TreeNode<T> *parent;
            std::vector<TreeNode<T>*> children;
            bool is_collapsed = false;

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

            ~Tree() { // TODO this will probably be handled by the treeview itself.
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

            TreeNode<T>* descend(TreeNode<T> *root, std::function<void(TreeNode<T> *node)> fn = nullptr) {
                if (fn) {
                    fn(root);
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
            // // Goes down the tree using the iterator
            // TreeNode get(TreeIter iter);
            // // Goes down the tree using row (skips non visible nodes)
            // TreeNode getRow(size_t row);
            // TreeNode descend(TreeNode root, std::function<void(TreeNode node)> callback = nullptr);
            // TreeNode ascend(TreeNode leaf, std::function<void(TreeNode node)> callback = nullptr);
    };

    // // TODO event for TreeView?? only one atm (onRowSelected) i guess could also have other ones
    // // like onRowHovered, onModelChanged, onNodeCollapsed etc.

    // // TODO actually probably not Widget but rather
    // // Top level box
    // // inner box for the column headers
    // // though we could maybe have the column header inside the view always at the top
    // // and just account for that, this would simplify the process of scrolling
    // // since otherwise we would need to make sure that the headers and the view
    // // are scrolled the same amount, it would also make it easier to line the up
    // // and a scrolledbox for the view
    // template <typename T> class TreeView : public Widget {
    //     public:
    //         // Tree<T> or the below? i guess the below cause they specifically need to be nodes
    //         Tree<TreeNode<T>> model;
    //         // selected row
    //         // hovered row
    //         // gridlines
    //         // header height
    //         // row height
    //         // min col width
    //         // expand col to content??
    //         // image cache / manager
    //         // headings / columns
    //         // callbacks ie onRowSelected etc.
    //         // indent size for children
    //         // allow to drag columns to resize them?? (low priority)
    //         // current sorted column
    //         // sort type ie asc, desc
    //         // tooltips (kinda important since we want some headings to be image only), but treeview specific or lib wide?
            

    //         // TODO design of the widget itself
    //         // i think im gonna go with a custom view thats somewhat based on scrolledbox
    //         // at the top we are going to have column headers which are going to always stay at the top
    //         // unless not visible (ie isVisible) so they are going to be mostly normal widgets i think
    //         // just always drawn at the top of the viewport regardless of scroll
    //         // with the rest of the visible content taken up the the tree itself
    //         // we still need to figure out size of each, each column etc.
    //         // 1. so that we can have differently size rows and still be performant
    //         // 2. to know the virtual size of the view
    //         // we could do some more experiments with binary search
    //         // but for that to work we would need to know the size of each row at all times
    //         // to successfully traverse it for drawing and event handling
    //         // i also would like the columns to be resizable in real-time
    //         // 
    //         // so for differently sized rows (they probably wouldnt be here but i dont want to limit myself since these widgets could display loads of data)
    //         // i think im gonna go with my previous idea, not of binary search but keeing track of the first visible widget/row
    //         // then we would update that on every draw, and any subsequent draws or events would start there and work their way
    //         // only through the currently visible widgets
    //         // but we would need to update it whenever the model changes
    //         // and now that i think about it in this case the rows could be different size but they would not be expandable
    //         // so the other rows dont affect others
    //         // therefore we could always make assumptions just based on whats visible? right??
    //         // because one widget wouldnt affect others (but wouldnt it affect the total scrollable area?)
    //         // dunno we would have to think about it
    //         // just a final note
    //         // all we need to know is the total size of the scrollable area
    //         // and whether there are rows above the first visible one
    //         // and we can use this for scrolledbox as well
    //         // we dont actually care about whether elements are resizable or not
    //         // because that only matters once the total virtual size of the scrolledbox
    //         // is less than its given rect by the app or layout

    //         TreeView();
    //         ~TreeView();
    // };

    template <typename T> class TreeView : public Scrollable {
        public:
            TreeView(Size min_size = Size(400, 400)) : Scrollable(min_size) {

            }

            ~TreeView() {
                delete m_model;
            }

            virtual const char* name() override {
                return "TreeView";
            }

            virtual void draw(DrawingContext *dc, Rect rect) override {
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
                // TODO so now we have to implement drawing those cellrenderers
                // when we add a model we need to compute the size for each column and row
                // so that we know the total size
                // expand any column headers as needed ?option?
                // finally draw all the rows by iterating over the tree clip as necessary
                // TODO take into account the collapsed status of nodes and their hierarchy
                // TODO then maybe next step is to make the columns resizable using the mouse?
                Size virtual_size = children_size;
                for (TreeNode<T> *root : m_model->roots) {
                    m_model->descend(root, [&](TreeNode<T> *node) {
                        virtual_size.h += node->columns[0]->sizeHint(dc).h;
                    });
                }
                Point pos = automaticallyAddOrRemoveScrollBars(dc, rect, virtual_size);
                dc->fillRect(rect, Color(0.6, 0.0, 0.2));
                float local_pos_x = pos.x;
                dc->setClip(rect);
                for (Widget *child : children) {
                    Size s = child->sizeHint(dc);
                    child->draw(dc, Rect(local_pos_x, rect.y, s.w, children_size.h));
                    local_pos_x += s.w;
                }
                pos.y += children_size.h;
                dc->setClip(Rect(rect.x, rect.y + children_size.h, rect.w, rect.h - children_size.h));
                for (TreeNode<T> *root : m_model->roots) {
                    m_model->descend(root, [&](TreeNode<T> *node) {
                        float cell_start = pos.x;
                        float row_height = 0.0;
                        for (size_t i = 0; i < node->columns.size(); i++) {
                            // TODO need to clip each column
                            // TODO because we dont query the size beforehand yet
                            // if the text in a cell is longer than the column header width
                            // the cell will center text incorrectly
                            CellRenderer *renderer = node->columns[i];
                            Size s = renderer->sizeHint(dc);
                            if (s.h > row_height) {
                                row_height = s.h; // TODO we should check that in advance so we can give each cell more space when possible
                            }
                            renderer->draw(dc, Rect(cell_start, pos.y, column_widths[i], s.h));
                            cell_start += column_widths[i];
                        }
                        pos.y += row_height;
                    });
                }
                dc->setClip(old_clip);
                drawScrollBars(dc, rect, virtual_size);
            }

            void setModel(Tree<T> *model) {
                m_model = model;
            }

            void clear() {
                if (m_model) {
                    m_model->clear();
                }
            }

        protected:
            Tree<T> *m_model = nullptr;
    };
#endif