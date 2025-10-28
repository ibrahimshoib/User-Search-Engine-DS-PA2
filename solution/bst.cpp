#include "../headers/bst.h"
#include <iostream>
#include <algorithm>
#include <stdexcept>
using namespace std;

template<typename K, typename V>
BST<K, V>::BST() : root(nullptr), nodeCount(0) {
    comparator = [](const K& a, const K& b) { return a < b; };
}

template<typename K, typename V> 
BST<K, V>::BST(function<bool(const K&, const K&)> comp) 
    : root(nullptr), nodeCount(0), comparator(comp) {
}

template<typename K, typename V>
bool BST<K, V>::insert(const K& key, const V& value) {
    if (find(key) != nullptr) return false; 
    root = insertHelper(root, key, value);
    nodeCount++;
    return true;
}

template<typename K, typename V>
shared_ptr<typename BST<K, V>::BSTNode> BST<K, V>::insertHelper(shared_ptr<BSTNode> node, const K& key, const V& value) {
    if (node ==nullptr)
        return make_shared<BSTNode>(key,value);
    if (comparator(key,node->key)) //key<node.key -- go left
    {
        node->left = insertHelper(node->left,key,value);
        if (node->left)
        node->left->parent=node;
    }
    else
    {
        node->right = insertHelper(node->right,key,value);
        if (node->right)
        node->right->parent=node;
    }

    updateHeight(node);
    return node;
}
// template<typename K, typename V>
// shared_ptr<typename BST<K, V>::BSTNode> BST<K, V>::insertHelper(shared_ptr<BSTNode> node, const K& key, const V& value) {

// }


template<typename K, typename V>
bool BST<K, V>::remove(const K& key) {
    if (find(key)==nullptr){
        return false;
    }
    root = removeHelper(root,key);
    nodeCount--;
    return true;
}

template<typename K, typename V>
shared_ptr<typename BST<K, V>::BSTNode> BST<K, V>::removeHelper(shared_ptr<BSTNode> node, const K& key) {
    // finds the key,
    // if no child then remove.
    // if one child then assign child to parent.
    // if two child then replace it with PSN or NGE.
    if (node == nullptr)
            return nullptr;
    if (comparator(node->key,key)) //node.key is less than key to find. move right
    {
        node->right = removeHelper(node->right,key);
    }
    else if (comparator(key,node->key)) //key<node.key
    {
        node->left = removeHelper(node->left,key);
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
        auto nge = (findMinHelper(node->right));
        node->key = nge->key;
        node->value = nge->value;
        node->right = removeHelper(node->right,nge->key);
    }
    if (node->left)
    node->left->parent = node;
    if (node->right)
    node->right->parent = node;
    updateHeight(node);
    return node;
}

template<typename K, typename V>
V* BST<K, V>::find(const K& key) {
    auto found = (findHelper(root,key));
    if (found==nullptr)
        return nullptr;
    return &(found->value);}

template<typename K, typename V>
const V* BST<K, V>::find(const K& key) const {
    auto found = (findHelper(root,key));
    if (found==nullptr)
        return nullptr;
    return &(found->value);}

template<typename K, typename V>
shared_ptr<typename BST<K, V>::BSTNode> BST<K, V>::findHelper(shared_ptr<BSTNode> node, const K& key) const {
    if (node==nullptr)
        return nullptr;
    if (comparator(node->key,key)) //node.key<key
    return findHelper(node->right,key);

    else if (comparator(key,node->key)) //node.key>key
    return findHelper(node->left,key);
    else return node;
}

template<typename K, typename V>
pair<K, V> BST<K, V>::min() const {
    
    auto minimum = findMinHelper(root);
    if (!minimum)
        throw std::runtime_error("the bst is empty, couldnt find minimum");
    return make_pair(minimum->key, minimum->value);
}

template<typename K, typename V>
shared_ptr<typename BST<K, V>::BSTNode> BST<K, V>::findMinHelper(shared_ptr<BSTNode> node) const {
    if (!node) return nullptr;
    if (node->left)
        return findMinHelper(node->left);
    else return node ;
}

template<typename K, typename V>
pair<K, V> BST<K, V>::max() const {

    auto maximum =  findMaxHelper(root);
        if (!maximum)
        throw std::runtime_error("the bst is empty, couldnt find max");
    return make_pair(maximum->key, maximum->value);}

template<typename K, typename V>
shared_ptr<typename BST<K, V>::BSTNode> BST<K, V>::findMaxHelper(shared_ptr<BSTNode> node) const {
    if (!node) return nullptr;
    if (node->right)
        return findMaxHelper(node->right);
    else return node ;
}

template<typename K, typename V>
vector<pair<K, V>> BST<K, V>::findRange(const K& minKey, const K& maxKey) const {
    vector<pair<K,V>> result;
    rangeHelper(root,minKey,maxKey,result);
    return result;
}

template<typename K, typename V>
void BST<K, V>::rangeHelper(shared_ptr<BSTNode> node, const K& minKey, const K& maxKey, vector<pair<K, V>>& result) const {
    //MUST be INCLUSIVE!!
    if (node == nullptr)
        return;
    if (node->left && comparator(minKey,node->key)) //min<key go left
    {
        rangeHelper(node->left,minKey,maxKey,result);
    }
    
    if (!(comparator(node->key,minKey) || comparator(maxKey,node->key))) //min< current key <max, push 
    {
        auto kv_pair = make_pair(node->key,node->value);
        result.push_back(kv_pair);
    }
    if (node->right && comparator(node->key,maxKey))
    {
        rangeHelper(node->right,minKey,maxKey,result);
    }
}

template<typename K, typename V>
vector<pair<K, V>> BST<K, V>::inOrderTraversal() const {
    vector<pair<K,V>> result;
    inOrderHelper(root,result);
    return result;
}

template<typename K, typename V>
void BST<K, V>::inOrderHelper(shared_ptr<BSTNode> node, vector<pair<K, V>>& result) const {
    if (node->left)
    inOrderHelper(node->left,result);
    result.push_back(make_pair(node->key,node->value));
    
    if (node->right)
        inOrderHelper(node->right,result);

}

template<typename K, typename V>
void BST<K, V>::displayTree() const {
    vector<pair<K,V>> result;
    result = inOrderTraversal();
    for (int i=0;i<result.size();i++)
    {
        cout<<(result[i].first)<<" ";
    }
}

template<typename K, typename V>
void BST<K, V>::displayHelper(shared_ptr<BSTNode> node, int depth) const {
}

template<typename K, typename V>
bool BST<K, V>::isValidBST() const {
    if (!root)
        return true;    //tree doesnt even exist
    // auto minptr = &((min()).first);
    // auto maxptr = &((max()).first);
    return isValidBSTHelper(root,nullptr,nullptr);
}

template<typename K, typename V>
bool BST<K, V>::isValidBSTHelper(shared_ptr<BSTNode> node, const K* minVal, const K* maxVal) const {

    if(minVal && comparator(node->key,*minVal))
        return false;
    if(maxVal && comparator(*maxVal,node->key))
        return false;
    if (node->left)
        return isValidBSTHelper(node,minVal,&node->key);
    if (node->right)
        return isValidBSTHelper(node,&node->key,maxVal);

    return true;
}

template<typename K, typename V>
void BST<K, V>::updateHeight(shared_ptr<BSTNode> node) {
    if (!node)
        node->height=0;
    node->height = 1+std::max(getHeight(node->left),getHeight(node->right));
}

template<typename K, typename V>
int BST<K, V>::getHeight(shared_ptr<BSTNode> node) const {
    if (!node)
        return 0;
    return node->height;
}

template class BST<int, string>;
template class BST<string, string>;
template class BST<int, int>;
template class BST<string, void*>;