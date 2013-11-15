
#ifndef BPLUSTREE_H
#define BPLUSTREE_H

#include <pair>

template<class K>
struct Node {
    K* value_array;
    V* ptr_array;

    uint32_t now_num;
    uint32_t max_num;

    size_t nodeid;
};

class PageNodeAllocator : public NodeAllocator {
    std::string indexfilename;
    NodeAllocator(BufferManagerPtr bufferManager);
    Node getNode(uint32_t nodeid) override;
    Node createNode() override;
}


class NodeAllocator {
    virtual Node getNode(size_t nodeid) = 0;
    virtual Node createNode() = 0;
};

template<class K, class V>
class BPlusTree {
    
public:
    BPlusTree(NodeAllocator* allocator);
    ~BPlusTree();
    
    void create();
    V search(K key);
    void Insert(const K& key, const V& value);
    
private:

};

#endif

n
