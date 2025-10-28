#include <iostream>
#include <vector>
#include <string>
#include <functional>
#include <memory>
#include <algorithm>
#include <iomanip>
#include <stdexcept>
#include <set>
#include <random>

// Assumes your bst.h is in a subdirectory named "headers"
#include "bst.h"

using namespace std;

/**
 * @class BSTTester
 * @brief Provides low-level functions to test specific features of the BST.
 */
template<typename K, typename V>
class BSTTester : public BST<K, V> {
public:
    // Resets the tree to an empty state for a new test.
    void clear_tree() {
        this->root = nullptr;
        this->nodeCount = 0;
    }

    // Tests a sequence of insertions, validating the tree's integrity after each step.
    bool run_insertion_test(const vector<pair<K, V>>& elements) {
        clear_tree();
        vector<K> inserted_keys;
        for (const auto& elem : elements) {
            auto it = find(inserted_keys.begin(), inserted_keys.end(), elem.first);
            if (it == inserted_keys.end()) {
                inserted_keys.push_back(elem.first);
            }
            this->insert(elem.first, elem.second);

            if (!is_tree_valid(inserted_keys)) {
                cerr << "\n  [FAILURE] Tree became invalid after inserting key: " << elem.first;
                return false;
            }
        }
        return true;
    }

    // Tests a sequence of removals, validating the tree's integrity after each step.
    bool run_removal_test(const vector<pair<K, V>>& initial_elements, const vector<K>& keys_to_remove) {
        clear_tree();
        vector<K> current_keys;
        for (const auto& elem : initial_elements) this->insert(elem.first, elem.second);
        for (const auto& elem : initial_elements) current_keys.push_back(elem.first);
        sort(current_keys.begin(), current_keys.end());
        current_keys.erase(unique(current_keys.begin(), current_keys.end()), current_keys.end());

        for (const auto& key : keys_to_remove) {
            this->remove(key);
            auto it = remove(current_keys.begin(), current_keys.end(), key);
            bool was_present = (it != current_keys.end());
            current_keys.erase(it, current_keys.end());

            if (this->find(key)) {
                cerr << "\n  [FAILURE] Found key " << key << " after it was supposed to be removed.";
                return false;
            }
            if (was_present && !is_tree_valid(current_keys)) {
                 cerr << "\n  [FAILURE] Tree became invalid after removing key: " << key;
                 return false;
            }
        }
        return true;
    }

    // Validates the min() method's correctness and behavior on an empty tree.
    bool run_min_test(const vector<pair<K, V>>& elements) {
        clear_tree();
        if (elements.empty()) {
            try { this->min(); return false; } catch (const std::exception&) { return true; }
        }
        for (const auto& elem : elements) this->insert(elem.first, elem.second);
        auto min_it = min_element(elements.begin(), elements.end(), [](const pair<K,V>& a, const pair<K,V>& b) { return a.first < b.first; });
        return this->min().first == min_it->first;
    }

    // Validates the max() method's correctness and behavior on an empty tree.
    bool run_max_test(const vector<pair<K, V>>& elements) {
        clear_tree();
        if (elements.empty()) {
            try { this->max(); return false; } catch (const std::exception&) { return true; }
        }
        for (const auto& elem : elements) this->insert(elem.first, elem.second);
        auto max_it = max_element(elements.begin(), elements.end(), [](const pair<K,V>& a, const pair<K,V>& b) { return a.first < b.first; });
        return this->max().first == max_it->first;
    }

    // Validates if the in-order traversal returns a correctly sorted sequence of elements.
    bool run_in_order_test(const vector<pair<K, V>>& elements) {
        clear_tree();
        vector<pair<K, V>> expected;
        set<K> seen_keys;
        for(const auto& elem : elements) {
            if(seen_keys.find(elem.first) == seen_keys.end()) {
                expected.push_back(elem);
                seen_keys.insert(elem.first);
            }
        }
        sort(expected.begin(), expected.end(), [](const pair<K,V>& a, const pair<K,V>& b){ return a.first < b.first; });
        for (const auto& elem : elements) this->insert(elem.first, elem.second);
        vector<pair<K, V>> actual = this->inOrderTraversal();
        return actual == expected;
    }

    // Validates if the tree height is correctly calculated for a predictable structure.
    bool run_height_test(const vector<pair<K, V>>& elements, int expected_height) {
        clear_tree();
        for (const auto& elem : elements) this->insert(elem.first, elem.second);
        return this->getTreeHeight() == expected_height;
    }

    // --- NEW HELPER FOR findRange ---
    bool run_findRange_test(K minKey, K maxKey, const vector<K>& expected_keys_in_range) {
        vector<pair<K,V>> result = this->findRange(minKey, maxKey);
        
        if (result.size() != expected_keys_in_range.size()) {
            cerr << "\n  [FAILURE] findRange returned incorrect number of items. Expected: " << expected_keys_in_range.size() << ", Got: " << result.size();
            return false;
        }

        set<K> actual_keys;
        for (const auto& p : result) {
            actual_keys.insert(p.first);
        }

        set<K> expected_set(expected_keys_in_range.begin(), expected_keys_in_range.end());

        if (actual_keys != expected_set) {
            cerr << "\n  [FAILURE] The set of keys returned by findRange does not match the expected set.";
            return false;
        }
        return true;
    }

private:
    bool find_and_validate_path(const K& key) {
        auto current = this->root;
        while (current) {
            if (current->left && !this->comparator(current->left->key, current->key)) {
                cerr << "\n  [FAILURE] BST Invariant Violation: Parent " << current->key << " is not greater than left child " << current->left->key;
                return false;
            }
            if (current->right && !this->comparator(current->key, current->right->key)) {
                cerr << "\n  [FAILURE] BST Invariant Violation: Parent " << current->key << " is not less than right child " << current->right->key;
                return false;
            }
            if (this->comparator(key, current->key)) {
                current = current->left;
            } else if (this->comparator(current->key, key)) {
                current = current->right;
            } else {
                return true;
            }
        }
        cerr << "\n  [FAILURE] Key not found during validation: " << key;
        return false;
    }

    bool is_tree_valid(const vector<K>& expected_keys) {
        if (this->nodeCount != expected_keys.size()) {
            cerr << "\n  [FAILURE] Node count mismatch. Expected: " << expected_keys.size() << ", Actual: " << this->nodeCount;
            return false;
        }
        for (const K& key : expected_keys) {
            if (!find_and_validate_path(key)) {
                return false;
            }
        }
        return true;
    }
};


class TestRunner {
public:
    TestRunner() : total_score(0), max_score(0) {}

    void run_all_tests() {
        cout << "=============================" << endl;
        cout << "  BST Implementation Tester  " << endl;
        cout << "=============================" << endl;
        
        test_insertion_scenarios();
        test_removal_scenarios();
        test_query_and_property_scenarios();
        test_range_queries(); // New test suite
        test_edge_case_scenarios();

        cout << "\n-----------------------------------------------------------------------" << endl;
        cout << "                      TESTING SUMMARY" << endl;
        cout << "-----------------------------------------------------------------------" << endl;
        cout << "  FINAL SCORE: " << total_score << " / " << max_score << endl;
        if (total_score == max_score) {
            cout << "  RESULT: All tests passed!" << endl;
        } else {
            cout << "  RESULT: Some tests failed. Please review the output above for details." << endl;
        }
        cout << "=======================================================================" << endl;
    }

private:
    int total_score;
    int max_score;
    BSTTester<int, string> tester;

    void execute_test(const string& name, int points, const function<bool()>& test_func) {
        max_score += points;
        cout << "  " << left << setw(55) << name << " [" << points << " pts] ... ";
        if (test_func()) {
            cout << "PASSED" << endl;
            total_score += points;
        } else {
            cout << "FAILED" << endl;
        }
    }
    
    // --- Test Suites ---

    void test_insertion_scenarios() {
        cout << "\n--- Testing Insertion Scenarios ---" << endl;
        execute_test("Insert into a balanced tree", 5, [this]() {
            return tester.run_insertion_test({{10, "A"}, {5, "B"}, {15, "C"}, {3, "D"}, {7, "E"}});
        });
        execute_test("Insert duplicate key (should ignore)", 5, [this]() {
            return tester.run_insertion_test({{10, "A"}, {5, "B"}, {10, "C"}});
        });
        execute_test("Stress: Insert 20 keys in ascending order", 7, [this]() {
            vector<pair<int, string>> data;
            for (int i = 1; i <= 20; ++i) data.push_back({i, "v"});
            return tester.run_insertion_test(data);
        });
        execute_test("Stress: Insert 20 keys in descending order", 8, [this]() {
            vector<pair<int, string>> data;
            for (int i = 20; i >= 1; --i) data.push_back({i, "v"});
            return tester.run_insertion_test(data);
        });
        // NEW TEST
        execute_test("Interleaved Insertion", 5, [this]() {
            return tester.run_insertion_test({{10,""}, {1,""}, {20,""}, {2,""}, {19,""}, {3,""}, {18,""}});
        });
    }

    void test_removal_scenarios() {
        cout << "\n--- Testing Removal Scenarios ---" << endl;
        vector<pair<int, string>> data = {{20,""},{10,""},{30,""},{5,""},{15,""},{25,""},{35,""},{12,""},{17,""}};
        execute_test("Remove a leaf node", 5, [this, data]() {
            return tester.run_removal_test(data, {5});
        });
        execute_test("Remove a node with one right child", 5, [this]() {
            return tester.run_removal_test({{10,""},{5,""},{7,""}}, {5});
        });
         execute_test("Remove a node with one left child", 5, [this]() {
            return tester.run_removal_test({{10,""},{7,""},{5,""}}, {7});
        });
        execute_test("Remove a node with two children", 10, [this, data]() {
            return tester.run_removal_test(data, {10});
        });
        execute_test("Remove the root node", 10, [this, data]() {
            return tester.run_removal_test(data, {20});
        });
        // NEW TEST
        execute_test("Remove node whose successor is its right child", 5, [this, data]() {
             return tester.run_removal_test(data, {15});
        });
        // NEW TEST
        execute_test("Remove multiple nodes sequentially", 10, [this, data]() {
            return tester.run_removal_test(data, {5, 17, 10, 30});
        });
    }

    void test_query_and_property_scenarios() {
        cout << "\n--- Testing Queries and Properties ---" << endl;
        vector<pair<int, string>> data = {{50,""}, {25,""}, {75,""}, {-10,""}, {100,""}, {60,""}};
        execute_test("Min and Max on a populated tree", 10, [this, data]() {
            return tester.run_min_test(data) && tester.run_max_test(data);
        });
        execute_test("In-order traversal produces sorted list", 10, [this, data]() {
            return tester.run_in_order_test(data);
        });
        execute_test("Tree height for a balanced tree", 5, [this]() {
            return tester.run_height_test({{10,""},{5,""},{15,""}}, 2);
        });
        execute_test("Tree height for a degenerate (right-skewed) tree", 5, [this]() {
            vector<pair<int, string>> asc_data;
            for (int i = 1; i <= 5; ++i) asc_data.push_back({i, "v"});
            return tester.run_height_test(asc_data, 5);
        });
    }

    // --- NEW TEST SUITE ---
    void test_range_queries() {
        cout << "\n--- Testing Range Queries (findRange) ---" << endl;
        vector<pair<int, string>> data = {{50,""}, {25,""}, {75,""}, {10,""}, {40,""}, {60,""}, {90,""}, {5,""}, {15,""}, {35,""}, {45,""}, {85,""}, {100,""}};
        tester.clear_tree();
        for(const auto& p : data) tester.insert(p.first, p.second);

        execute_test("Range: Middle of the tree", 5, [this]() {
            return tester.run_findRange_test(30, 80, {35, 40, 45, 50, 60, 75});
        });
        execute_test("Range: Including tree minimum", 5, [this]() {
            return tester.run_findRange_test(5, 25, {5, 10, 15, 25});
        });
        execute_test("Range: Including tree maximum", 5, [this]() {
            return tester.run_findRange_test(75, 100, {75, 85, 90, 100});
        });
        execute_test("Range: Full tree range", 5, [this]() {
            return tester.run_findRange_test(0, 110, {5,10,15,25,35,40,45,50,60,75,85,90,100});
        });
        execute_test("Range: No results (above max)", 5, [this]() {
            return tester.run_findRange_test(200, 300, {});
        });
        execute_test("Range: No results (below min)", 5, [this]() {
            return tester.run_findRange_test(0, 4, {});
        });
        execute_test("Range: No results (between nodes)", 5, [this]() {
            return tester.run_findRange_test(51, 59, {});
        });
        execute_test("Range: Single element (leaf)", 5, [this]() {
            return tester.run_findRange_test(15, 15, {15});
        });
        execute_test("Range: Single element (root)", 5, [this]() {
            return tester.run_findRange_test(50, 50, {50});
        });
        execute_test("Range: Invalid range (min > max)", 5, [this]() {
            return tester.run_findRange_test(80, 30, {});
        });
    }

    void test_edge_case_scenarios() {
        cout << "\n--- Testing Edge Cases ---" << endl;
        execute_test("Min/Max on empty tree", 5, [this]() {
            return tester.run_min_test({}) && tester.run_max_test({});
        });
        execute_test("Remove from empty tree / non-existent key", 5, [this]() {
            return tester.run_removal_test({}, {99}) && tester.run_removal_test({{10,""}}, {99});
        });
        execute_test("Operations on a single-node tree", 5, [this]() {
            vector<pair<int,string>> single = {{100, "A"}};
            bool pass = tester.run_height_test(single, 1);
            pass &= tester.run_in_order_test(single);
            pass &= tester.run_removal_test(single, {100});
            return pass;
        });
        // NEW TEST
        execute_test("findRange on empty tree", 5, [this]() {
            tester.clear_tree();
            return tester.run_findRange_test(0, 100, {});
        });
        // NEW TEST
        execute_test("Dynamic Add/Remove Stress Test", 10, [this]() {
            vector<pair<int, string>> initial_data;
            vector<int> to_remove;
            std::mt19937 rng(123);
            std::uniform_int_distribution<int> dist(0, 1000);
            for(int i=0; i<200; ++i) initial_data.push_back({dist(rng), ""});
            for(int i=0; i<100; ++i) to_remove.push_back(initial_data[i].first);
            return tester.run_removal_test(initial_data, to_remove);
        });
    }
};

int main() {
    TestRunner runner;
    runner.run_all_tests();
    return 0;
}