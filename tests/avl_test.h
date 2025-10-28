#include <iostream>
#include <vector>
#include <string>
#include <functional>
#include <memory>
#include <algorithm>
#include <iomanip>
#include <stdexcept>
#include <sstream>
#include <chrono>
#include <random>
#include <cmath>
#include <set>

#include "avl_tree.h"

using namespace std;

long long comparison_steps = 0;

/**
 * @class AVLTester
 * @brief Inherits from AVLTree to provide robust, self-contained validation.
 */
template<typename K, typename V>
class AVLTester : public AVLTree<K, V> {
public:
    using AVLTree<K, V>::AVLTree;

    // Public entry point for the AVL balance check.
    bool isTreeBalanced() const {

        bool ret = is_avl_balanced_recursive(this->root).is_balanced;
        cout<<ret<<endl;
        return ret;
    }

    // Public entry point for the BST validity check.
    bool isBSTValid(const set<K>& expected_keys) {
        return is_tree_valid_custom(expected_keys);
    }

public:
    // AVL check helper struct
    struct BalanceInfo {
        bool is_balanced;
        int height;
    };
    
    // The AVL balance check function you requested to keep unchanged.
    BalanceInfo is_avl_balanced_recursive(const shared_ptr<typename BST<K, V>::BSTNode>& node) const {
        if (!node) return {true, 0};
        BalanceInfo left_info = is_avl_balanced_recursive(node->left);
        if (!left_info.is_balanced) return {false, -1};
        BalanceInfo right_info = is_avl_balanced_recursive(node->right);
        if (!right_info.is_balanced) return {false, -1};
        if (abs(left_info.height - right_info.height) > 1) return {false, -1};
        return {true, 1 + max(left_info.height, right_info.height)};
    }
    
    // --- BST VALIDATION LOGIC (from BSTTester, as requested) ---

    bool find_and_validate_path(const K& key) {
        auto current = this->root;
        while (current) {
            if (current->left && !this->comparator(current->left->key, current->key)) {
                cout << "\n    [FAIL] BST Invariant Violation: Parent " << current->key << " is not > left child " << current->left->key;
                return false;
            }
            if (current->right && !this->comparator(current->key, current->right->key)) {
                cout << "\n    [FAIL] BST Invariant Violation: Parent " << current->key << " is not < right child " << current->right->key;
                return false;
            }
            if (this->comparator(key, current->key)) {
                current = current->left;
            } else if (this->comparator(current->key, key)) {
                current = current->right;
            } else {
                return true; // Key found, path valid.
            }
        }
        cout << "\n    [FAIL] Expected key not found during validation: " << key;
        return false;
    }

    bool is_tree_valid_custom(const set<K>& expected_keys) {
        for (const K& key : expected_keys) {
            if (!find_and_validate_path(key)) {
                return false;
            }
        }
        return true;
    }
};