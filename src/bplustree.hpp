
#ifndef BPLUSTREE_H
#define BPLUSTREE_H

template <class T, unsigned int degree>
class BPlusTree {

public:

    struct TreeNode {
        int n_keys;
        int n_ptrs;
        bool is_leaf;

        T *keys;
        void** ptrs;
        
        TreeNode():is_leaf(true), n_keys(0) {}
    };

private:
    TreeNode* root_;
    
public:
    BPlusTree() {
    }
    
    ~BPlusTree() {
    }
    
    void Create() {
        root_ = new TreeNode;
    }

    void* find(T value) {
        auto c = root_;

        while ( ! c->is_leaf) {
            int i = 0;
            // find the smallest number i such that value <= c.keys[i]
            while (i < c->n_keys && c.keys[i] < value) i++;
            // no such key
            if (i == c->n_keys) {
                // set c to the last non-null pointer in the node
                c = c.ptrs[n_ptrs - 1];
            }
            // there's such i
            else {
                c = c.ptrs[i];
            }
        }

        // now c is a leaf node
        int i = 0;
        while (i < c->n_keys && c.keys[i] < value) i++;
        if (c.keys[i] == value) {
            return c.ptrs[i];
        }
        else {
            return NULL;
        }
    }

    void Insert(T key, void* ptr) {
        auto l = find(key);
        if (r->n_keys == degree) {

        }
        else {

        }
    }

    void InsertNonfull(T key, void* ptr) {

    }

    void SplitChild(TreeNode* x, int pos, TreeNode* y) {

        auto z = new TreeNode;
        z->is_leaf = z->is_leaf;
        z->n_keys = degree / 2 - 1;

        for (auto j = 1; j <= degree / 2 - 1; j++) {
            z->keys[j] = y->keys[j + degree / 2];
        }
        if ( ! y->is_leaf) {
            for (auto j = 1; j <= degree / 2; j++) {
                z->pointers[j] = y->pointers[j + degree / 2];
            }
        }

        y->n_keys = degree / 2 - 1;
        for (auto j = x->n_keys + 1; j >= pos + 1; j--) {
            x->ptrs[j + 1]; = x->ptrs[j];
        }
        x->ptrs[pos] = z
                       for (auto j = x->n_keys; j >= pos; j--) {
                           x->keys[j + 1] = x->keys[j];
                       }
        x->keys[pos] = y->keys[degree / 2];
        x->n_keys++;
    }

    // void Insert(T key, void* ptr) {
    //   TreeNode* node;

    //   // if the tree is empty
    //   if (NULL == root_) {
    //     root_ = new TreeNode;
    //     node = root_;
    //   }
    //   else {
    //     // findt the leaf node that should contain key K
    //   }

    //   if (node->n_keys < degree - 1) {
    //     InsertInLeaf(node, key, ptr);
    //   }
    //   // node has n - 1 key values already, split it
    //   else {
    //     auto new_node = new TreeNode;
    //     for (int i = 0; i < )
    //   }
    // }

    // // pre-condition : the node must NOT be FULL
    // void InsertInLeaf(TreeNode* node, T key, void * ptr) {

    //   // if the key less than the first key
    //   if (key < node->keys[0]) {
      
    //     // move all the key and ptrs forward
    //     for (int i = node->n_keys - 1; i > 0; i--) {
    //       node->keys[i] = node->keys[i - 1];
    //     }
    //     for (int i = node->n_ptrs - 1; i > 0; i--) {
    //       node->ptrs[i] = node->ptrs[i - 1];
    //     }

    //     // insert the key in front
    //     node->ptrs[0] = ptr;
    //     node->keys[0] = key;
    //   }
    //   else {

    //     // insert the key into the node
    //     int i = 0;
    //     while (node->keys[i] <= key) i++;

    //     // now i is just after the highest value in node that is less than key
    //     // insert key and ptr
    //     for (int j = node->n_keys - 1; j > i; j--) {
    //       node->keys[j] = node->keys[j - 1];
    //     }
    //     for (int j = node->n_ptrs - 1; j > i; j--) {
    //       node->ptrs[j] = node->ptrs[j - 1];
    //     }
    //     node->ptrs[i] = ptr;
    //     node->keys[i] = key;
    //   }
    // }

    // // pre-condition : the node must NOT be FULL
    // void InsertInParent(TreeNode* node, T key, void* ptr) {
    //   if (node == root_) {
    //     auto 
    //   }
    // }
    // const TreeNode* Search(const TreeNode* node, T value) {
    //   auto i = 1;
    //   while (i <= node->is_leaf && value > node->keys[i]) {
    //     i++;
    //   }

    //   if (i <= node->is_leaf && value == node->keys[i]) {
    //     return node, i
    //   }

    //   if (node->is_leaf) {
    //     return NULL;
    //   }
    //   else {
    //     Search(node->pointers[i], value);
    //   }
    // }



};

#endif

