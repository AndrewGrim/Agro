#ifndef TREE_HPP
    #define TREE_HPP

    #include <vector>

    class TreeIter {
        public:
            std::vector<int> path;

            TreeIter(std::vector<int> path) {
                this->path = path;
            }
    };

    template <class T> class TreeNode {
        public:
            T item;
            std::vector<TreeNode<T>> children;
            TreeNode<T> *parent;
            // is_visible: boolean = true;
            // is_collapsed: boolean = false;
            TreeIter iter = TreeIter(std::vector<int>());

            TreeNode(T item) {
                this->item = item;
            }
    };

    template <class T> class Tree {
        public:
            std::vector<TreeNode<T>> tree;

            TreeIter append(TreeIter tree_iter, TreeNode<T> item) {
                if (!tree_iter.path.size()) {
                    item.iter = TreeIter(std::vector<int>(this->tree.size()));
                    item.parent = nullptr;
                    this->tree.push_back(item); 
                    
                    return item.iter;
                } else {
                    TreeNode<T> root = this->tree[tree_iter.path[0]];
                    for (int i = 1; i < tree_iter.path.size(); i++) {
                        if (root.children.size()) {
                            root = root.children[tree_iter.path[i]];
                        } else {
                            break;
                        }
                    }

                    root.children.push_back(item);
                    int last_index = root.children.size() - 1; // TODO not sure here
                    TreeIter new_iter = TreeIter(tree_iter.path);
                    new_iter.path.push_back(last_index);
                    TreeIter iter = TreeIter(new_iter.path);
                    item.iter = iter;
                    item.parent = &root;

                    return iter;
                }
            }
    };

#endif