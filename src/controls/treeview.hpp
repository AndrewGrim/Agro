#ifndef TREEVIEW_HPP
    #define TREEVIEW_HPP

    #include "widget.hpp"
    #include "scrollable.hpp"

    // // TODO implement
    // class CellRenderer;

    // // TODO implement
    // class TreeIter;

    // struct ExampleHiddenStruct {
    //     size_t id;
    // };

    // template <typename T> class TreeNode {
    //     public:
    //         std::vector<CellRenderer> columns;
    //         T hidden;
    //         std::vector<TreeNode> children;
    //         bool is_visible;
    //         bool is_collapsed;
    //         TreeIter iter; // TODO not too sure about this, could maybe make it just a naked std::array?
    //         // especially considering that in the ts version its just TreeIter{path: number[]}

    //         // TODO not too sure about children in ctor, i dont think we really used this
    //         // method of constructing within the ts project
    //         TreeNode(std::vector<CellRenderer> columns, T hidden, std::vector<TreeNode> children) {

    //         }

    //         ~TreeNode() {

    //         }
    // };

    // template <typename T> class Tree {
    //     public:
    //         std::vector<TreeNode<T>> roots;

    //         Tree();  

    //         ~Tree();

    //         // TODO note iter needs to be nullable or optional? hint hint
    //         // but also why just not like have an append on a TreeNode instead? or both
    //         TreeIter append(TreeIter iter, TreeNode node);
    //         // Goes down the tree using the iterator
    //         TreeNode get(TreeIter iter);
    //         // Goes down the tree using row (skips non visible nodes)
    //         TreeNode getRow(size_t row);
    //         TreeNode descend(TreeNode root, std::function<void(TreeNode node)> callback = nullptr);
    //         TreeNode ascend(TreeNode leaf, std::function<void(TreeNode node)> callback = nullptr);
    // };

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
    //         // tooltips (kinda importan since we want some headings to be image only), but treeview specific or lib wide?
            

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

    class TestTree : public Scrollable {
        public:
            TestTree(Size min_size = Size(400, 400));
            ~TestTree();
            virtual const char* name() override;
            virtual void draw(DrawingContext *dc, Rect rect) override;
    };
#endif