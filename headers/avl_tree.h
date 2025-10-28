#pragma once
#include "bst.h"
using namespace std;

/**
 * AVL (self-balancing) Binary Search Tree
 */
template<typename K, typename V>
class AVLTree : public BST<K, V> {
private:
    using BSTNode = typename BST<K, V>::BSTNode;
    
    // Students must implement these rotation methods
    shared_ptr<BSTNode> rotateLeft(shared_ptr<BSTNode> node);
    shared_ptr<BSTNode> rotateRight(shared_ptr<BSTNode> node);
    shared_ptr<BSTNode> rotateLeftRight(shared_ptr<BSTNode> node);
    shared_ptr<BSTNode> rotateRightLeft(shared_ptr<BSTNode> node);
    
    // AVL-specific helper methods
    int getBalanceFactor(shared_ptr<BSTNode> node) const;
    shared_ptr<BSTNode> rebalance(shared_ptr<BSTNode> node);
    shared_ptr<BSTNode> insertAVL(shared_ptr<BSTNode> node, const K& key, const V& value);
    shared_ptr<BSTNode> removeAVL(shared_ptr<BSTNode> node, const K& key);

public:
    AVLTree();
    AVLTree(function<bool(const K&, const K&)> comp);
    
    // Override BST methods to maintain AVL property
    // Students must implement these methods
    bool insert(const K& key, const V& value) override;
    bool remove(const K& key) override;
    
    // AVL-specific methods
    bool isBalanced() const;
    int getMaxDepth() const;
    double getAverageDepth() const;
    
    // Validation methods for testing
    bool isValidAVL() const;
    
private:
    bool isValidAVLHelper(shared_ptr<BSTNode> node) const;
    void calculateDepthStats(shared_ptr<BSTNode> node, int depth, int& totalDepth, int& nodeCount, int& maxDepth) const;
};

#include "../solution/avl_tree.cpp"

