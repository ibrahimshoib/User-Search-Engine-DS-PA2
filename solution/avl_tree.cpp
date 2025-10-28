#include "../headers/avl_tree.h"
#include <algorithm>
#include <cmath>
using namespace std;

template<typename K, typename V>
AVLTree<K, V>::AVLTree() : BST<K, V>() {
}

template<typename K, typename V>
AVLTree<K, V>::AVLTree(function<bool(const K&, const K&)> comp) : BST<K, V>(comp) {
}

template<typename K, typename V>
bool AVLTree<K, V>::insert(const K& key, const V& value) {
    if (this->find(key))
    {
        return false;
    }
    this->root = insertAVL(this->root,key,value);
    this->nodeCount++;
    return true;

}

template<typename K, typename V>
shared_ptr<typename AVLTree<K, V>::BSTNode> AVLTree<K, V>::insertAVL(shared_ptr<BSTNode> node, const K& key, const V& value) {
    if (node ==nullptr)
        return make_shared<BSTNode>(key,value);
    if (this->comparator(key,node->key)) //key<node.key -- go left
    {
        node->left = insertAVL(node->left,key,value);
        if (node->left)
        node->left->parent=node;
    }
    else
    {
        node->right = insertAVL(node->right,key,value);
        if (node->right)
        node->right->parent=node;
    }
    this->updateHeight(node);
    return rebalance(node);
}

template<typename K, typename V>
bool AVLTree<K, V>::remove(const K& key) {
    if (this->find(key)==nullptr){
        return false;
    }
    this->root = removeAVL(this->root,key);
    this->nodeCount--;
    return true;
}

template<typename K, typename V>
shared_ptr<typename AVLTree<K, V>::BSTNode> AVLTree<K, V>::removeAVL(shared_ptr<BSTNode> node, const K& key) {
    {
    // finds the key,
    // if no child then remove.
    // if one child then assign child to parent.
    // if two child then replace it with PSN or NGE.
    if (node == nullptr)
            return nullptr;
    if (this->comparator(node->key,key)) //node.key is less than key to find. move right
    {
        node->right = removeAVL(node->right,key);
    }
    else if (this->comparator(key,node->key)) //key<node.key
    {
        node->left = removeAVL(node->left,key);
    }
    else{ //node found
        if (node->right == nullptr) //right doesnt exist
        {
            return node->left;
        }
        if (node->left == nullptr) //left doesnt exist
            return node->right;
        //both children exist, replace with Next greater element.
        // change key value, then call remove on the NGE node
        auto nge = (this->findMinHelper(node->right));
        node->key = nge->key;
        node->value = nge->value;
        node->right = removeAVL(node->right,nge->key);
    }
    if (node->left)
    node->left->parent = node;
    if (node->right)
    node->right->parent = node;

    this->updateHeight(node);
    return rebalance(node);
}
}

template<typename K, typename V>
shared_ptr<typename AVLTree<K, V>::BSTNode> AVLTree<K, V>::rotateLeft(shared_ptr<BSTNode> node) {
    shared_ptr<BSTNode> child =  node->right;
    shared_ptr<BSTNode> T1 =  child->left;
    child->left = node;
    node->right = T1;
    child->parent=node->parent;
    if(T1)
        T1->parent = node;
    node->parent = child;

    this->updateHeight(node);
    this->updateHeight(child);
    return child;

}

template<typename K, typename V>
shared_ptr<typename AVLTree<K, V>::BSTNode> AVLTree<K, V>::rotateRight(shared_ptr<BSTNode> node) {
    shared_ptr<BSTNode> child =  node->left;
    shared_ptr<BSTNode> T1 =  child->right;
    child->right = node;
    node->left = T1;
    child->parent=node->parent;
    if(T1)
        T1->parent = node;
    node->parent = child;
    this->updateHeight(node);
    this->updateHeight(child);
    return child;
}

template<typename K, typename V>
shared_ptr<typename AVLTree<K, V>::BSTNode> AVLTree<K, V>::rotateLeftRight(shared_ptr<BSTNode> node) {
    // 1. left rotation on left node
    // 2. right rotation on root
    node->left = rotateLeft(node->left);
    return rotateRight(node);
}

template<typename K, typename V>
shared_ptr<typename AVLTree<K, V>::BSTNode> AVLTree<K, V>::rotateRightLeft(shared_ptr<BSTNode> node) {
    node->right = rotateRight(node->right);
    return rotateLeft(node);
}

template<typename K, typename V>
int AVLTree<K, V>::getBalanceFactor(shared_ptr<BSTNode> node) const {
    if (!node)
    return 0;
    return (this->getHeight(node->left) - this->getHeight(node->right));
}

template<typename K, typename V>
shared_ptr<typename AVLTree<K, V>::BSTNode> AVLTree<K, V>::rebalance(shared_ptr<BSTNode> node) {
    int bf;
    bf = getBalanceFactor(node);
    int bfr = getBalanceFactor(node->right);
    int bfl = getBalanceFactor(node->left);
    
    if (bf>1){//left heavy
        if (bfl>=0) //LL heavy
            return rotateRight(node);
        else    //LR heavy
            return rotateLeftRight(node);
    }else if (bf<-1)//R 
    {    if (bfr<=0)//RR = left rotate
            return rotateLeft(node);
        else    //RL
            return rotateRightLeft(node);
    }
    return node;
    /// should i do bfl<=0?
    
    
}

template<typename K, typename V>
bool AVLTree<K, V>::isBalanced() const {
}

template<typename K, typename V>
bool AVLTree<K, V>::isValidAVLHelper(shared_ptr<BSTNode> node) const {
}

template<typename K, typename V>
int AVLTree<K, V>::getMaxDepth() const {
}

template<typename K, typename V>
double AVLTree<K, V>::getAverageDepth() const {
}

template<typename K, typename V>
void AVLTree<K, V>::calculateDepthStats(shared_ptr<BSTNode> node, int depth, int& totalDepth, int& nodeCount, int& maxDepth) const {
}

template<typename K, typename V>
bool AVLTree<K, V>::isValidAVL() const {
}

template class AVLTree<int, string>;
template class AVLTree<string, string>;
template class AVLTree<int, int>;
template class AVLTree<string, void*>;