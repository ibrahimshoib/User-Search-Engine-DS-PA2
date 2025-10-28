#pragma once
#include <functional>
#include <memory>
#include <vector>
#include <iostream>
using namespace std;

/**
 * Binary Search Tree template class
 */
template<typename K, typename V>
class BST {
public:
    struct BSTNode {
        K key;
        V value;
        shared_ptr<BSTNode> left;
        shared_ptr<BSTNode> right;
        weak_ptr<BSTNode> parent;
        int height;  // For AVL tree extension
        
        BSTNode(const K& k, const V& v) : key(k), value(v), left(nullptr), right(nullptr), height(1) {}
    };
protected:    
    shared_ptr<BSTNode> root;
    size_t nodeCount;
    function<bool(const K&, const K&)> comparator;
    
    // Helper methods for students to implement
    shared_ptr<BSTNode> insertHelper(shared_ptr<BSTNode> node, const K& key, const V& value);
    shared_ptr<BSTNode> removeHelper(shared_ptr<BSTNode> node, const K& key);
    shared_ptr<BSTNode> findHelper(shared_ptr<BSTNode> node, const K& key) const;
    shared_ptr<BSTNode> findMinHelper(shared_ptr<BSTNode> node) const;
    
    shared_ptr<BSTNode> findMaxHelper(shared_ptr<BSTNode> node) const;
    void updateHeight(shared_ptr<BSTNode> node);  // For AVL extension
    int getHeight(shared_ptr<BSTNode> node) const;

public:
    BST();
    BST(function<bool(const K&, const K&)> comp);
    virtual ~BST() = default;
    
    // Students must implement these methods
    virtual bool insert(const K& key, const V& value);
    virtual bool remove(const K& key);
    V* find(const K& key);
    const V* find(const K& key) const;
    
    pair<K, V> min() const;
    pair<K, V> max() const;
    vector<pair<K, V>> findRange(const K& minKey, const K& maxKey) const;
    
    size_t size() const { return nodeCount; }
    bool empty() const { return nodeCount == 0; }
    int getTreeHeight() const { return getHeight(root); }
    
    // Traversal methods
    vector<pair<K, V>> inOrderTraversal() const;
    void displayTree() const;
    
    // For testing and debugging
    bool isValidBST() const;
    shared_ptr<BSTNode> getRoot(){
        return this->root;
    }
    void setRoot(shared_ptr<BSTNode> ptr){
        this->root = ptr;
    }
    
protected:
    // Helper methods for traversals and validation
    void inOrderHelper(shared_ptr<BSTNode> node, vector<pair<K, V>>& result) const;
    void displayHelper(shared_ptr<BSTNode> node, int depth) const;
    bool isValidBSTHelper(shared_ptr<BSTNode> node, const K* minVal, const K* maxVal) const;
    void rangeHelper(shared_ptr<BSTNode> node, const K& minKey, const K& maxKey, vector<pair<K, V>>& result) const;
};

#include "../solution/bst.cpp"