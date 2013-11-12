
#ifndef BPLUSTREE_H
#define BPLUSTREE_H

#include <pair>

template<class T>
struct Node {
    int n_keys;
    int n_ptrs;
    bool is_leaf;

<<<<<<< HEAD
    T* keys;
    void** ptrs;
=======
        T *keys;
        void** ptrs;
>>>>>>> master
        
    Node():is_leaf(true), n_keys(0) {}
};

template<class T>
class NodeAllocator {

    // get a node via id from nodes allocated
    // lock the mem of node
    virtual Node<T>* getNode(void* id) = 0;
    
    // write back node 
    // unlock the mem of node
    virtual void writebackNode(Node<T>* node) = 0;

    // create a new node, and return the id
    virtual Node<T>* createNode() = 0;

};


template <class T, class NodeAllocator, unsigned int degree>
class BPlusTree {
public:
    BPlusTree() {
    }
    
    ~BPlusTree() {
        if (root_) {
            allocator.writeback(root_);
        }
    }
    
    void create() {
        auto id = allocator.createNode();
        root_ = allocator.getNode();
    }

    std::pair<Node<T>*, int> search(Node<T>* node, T key) {
        int i = 1;
        while (i <= node->n_ptrs && key > node->keys[i]) i++;
        if (i <= node->n_ptrs && key == node->keys[i]) {
            return std::make_pair(node, i);
        }
        if (node->is_leaf) {
            return make_pair<Node<T>*, int>(nullptr, -1);
        }
        else {
            auto nextNode = allocator.getNode();
            auto ret = search(nextNode, key);
            allocator.writeback(nextNode);
            return ret;
        }
    }

    void Insert(T key, void* ptr) {
        auto r = root_;
        if (r->n_keys == degree) {
            auto s = allocator.createNode();
            root_ = s;
            s->is_leaf = false;
            s->n_keys = 0;
            s->ptrs[0] = r;
            splitChild(s, 1, r);
            insertNonFull(s, key);
        }
        else {
            insertNonFull(r, k);
        }
    }

    void InsertNonFull(T key, Node<T>* nodex) {
        int i = nodex->n_ptrs - 1;
        if (nodex->is_leaf) {
            while (i >= 0 && key < nodex->keys[i]) {
                nodex->keys[i + 1] = nodex[i];
                i--;
            }
            nodex->keys[i + 1] = k;
        }
        else {
            while (i >= 0 && key < nodex->keys[i]) i--;
            auto node = allocator.getNode(nodex->ptrs[i]);
            if (node->n_ptrs == degree) {
                splitChild(nodex, i, node);
                if (key > nodex->keys[i]) {
                    i++;
                }
            }
            insertNonFull(node, key);
        }
    }

    void splitChild(Node<T>* nodex, int pos, Node<T>* nodey) {

        auto znode = allocator.createNode();
        znode->is_leaf = znode->is_leaf;
        znode->n_keys = degree / 2 - 1;

        for (int j = 1; j <= degree / 2 - 1; j++) {
            znode->keys[j] = ynode->keys[j + degree / 2];
        }
        if ( ! ynode->is_leaf) {
            for (int j = 1; j <= degree / 2; j++) {
                znode->pointers[j] = ynode->pointers[j + degree / 2];
            }
        }

        ynode->n_keys = degree / 2 - 1;
        for (int j = xnode->n_keys + 1; j >= pos + 1; j--) {
            xnode->ptrs[j + 1]; = xnode->ptrs[j];
        }
        xnode->ptrs[pos] = z
                       for (auto j = xnode->n_keys; j >= pos; j--) {
                           xnode->keys[j + 1] = xnode->keys[j];
                       }
        xnode->keys[pos] = ynode->keys[degree / 2];
        xnode->n_keys++;

        allocator.writeback(nodez);
        allocator.writeback(nodey);
        allocator.writeback(nodex);
    }


private:
    Node* root_;
    NodeAllocator<T> allocator;

};

#endif

n
