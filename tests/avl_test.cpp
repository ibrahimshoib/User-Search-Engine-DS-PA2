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
    // Public entry point for the AVL balance check.
    bool isTreeBalanced() const {
        return is_avl_balanced_recursive(this->root).is_balanced;
    }

    // Public entry point for the BST validity check.
    bool isBSTValid(const set<K>& expected_keys) {
        return is_tree_valid_custom(expected_keys);
    }

protected:
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
        if (this->size() != expected_keys.size()) {
            cout << "\n    [FAIL] Size mismatch. Expected: " << expected_keys.size() << ", Actual: " << this->size();
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
    AVLTester<int, string> tester;
    TestRunner() : total_score(0), max_score(0) {}

    void run_all_tests() {
        cout << "=======================================================================" << endl;
        cout << "                 AVL Tree Implementation Tester" << endl;
        cout << "=======================================================================" << endl;
        
        test_correctness();
        test_dynamic_scenarios();
        test_performance();

        cout << "\n-----------------------------------------------------------------------" << endl;
        cout << "                           TESTING SUMMARY" << endl;
        cout << "-----------------------------------------------------------------------" << endl;
        cout << "  FINAL SCORE: " << total_score << " / " << max_score << endl;
        if (total_score == max_score) {
            cout << "  RESULT: All correctness tests passed!" << endl;
        } else {
            cout << "  RESULT: Some correctness tests failed." << endl;
        }
        cout << "  (Performance test results are printed above for analysis)" << endl;
        cout << "=======================================================================" << endl;
    }

private:
    int total_score;
    int max_score;
    
    void execute_correctness_test(const string& name, int points, const string& desc, const function<bool()>& test_func) {
        max_score += points;
        cout << "\n  - " << name << " [" << points << " pts]" << endl;
        cout << "    " << desc << endl;
        cout << "    Running test... ";
        if (test_func()) {
            cout << "PASSED" << endl;
            total_score += points;
        } else {
            cout << "FAILED" << endl;
        }
    }

    void test_correctness() {
        cout << "\n--- Part 1: Correctness of Rotations and Balancing (Basic Insertions) ---" << endl;
        
        execute_correctness_test("Insertion causing LL Imbalance", 10, "Inserting {30, 20, 10}", []() {
            AVLTester<int, string> avl;
            set<int> keys = {30, 20, 10};
            for (int key : keys) avl.insert(key, "");
            return avl.isBSTValid(keys) && avl.isTreeBalanced();
        });

        execute_correctness_test("Insertion causing RR Imbalance", 10, "Inserting {10, 20, 30}", []() {
            AVLTester<int, string> avl;
            set<int> keys = {10, 20, 30};
            for (int key : keys) avl.insert(key, "");
            return avl.isBSTValid(keys) && avl.isTreeBalanced();
        });

        execute_correctness_test("Insertion causing RL Imbalance", 10, "Inserting {10, 30, 20}", []() {
            AVLTester<int, string> avl;
            set<int> keys = {10, 30, 20};
            for (int key : keys) avl.insert(key, "");
            return avl.isBSTValid(keys) && avl.isTreeBalanced();
        });

        execute_correctness_test("Insertion causing LR Imbalance", 10, "Inserting {30, 10, 20}", []() {
            AVLTester<int, string> avl;
            set<int> keys = {30, 10, 20};
            for (int key : keys) avl.insert(key, "");
            return avl.isBSTValid(keys) && avl.isTreeBalanced();
        });

        execute_correctness_test("Insertion causing LR Imbalance", 10, "Inserting  {10,20,30,5,2,15}", []() {
            AVLTester<int, string> avl;
            set<int> keys = {10,20,30,5,2,15};
            for (int key : keys) avl.insert(key, "");
            return avl.isBSTValid(keys) && avl.isTreeBalanced();
        });


    }

    void test_dynamic_scenarios() {
        cout << "\n--- Part 2: Correctness of Dynamic Operations (Removals and Mixed) ---" << endl;

        execute_correctness_test("Removal without Rotation", 5, "Removing leaf 30 from {20, 10, 30}", []() {
            AVLTester<int, string> avl;
            set<int> initial_keys = {20, 10, 30};
            for (int key : initial_keys) avl.insert(key, "");
            avl.remove(30);
            set<int> expected_keys = {20, 10};
            return avl.isBSTValid(expected_keys) && avl.isTreeBalanced();
        });

        execute_correctness_test("Removal triggers Left Rotation", 10, "Removing 10 from {20, 10, 30, 40}", []() {
            AVLTester<int, string> avl;
            set<int> initial_keys = {20, 10, 30, 40};
            for (int key : initial_keys) avl.insert(key, "");
            avl.remove(10);
            set<int> expected_keys = {20, 30, 40};
            return avl.isBSTValid(expected_keys) && avl.isTreeBalanced();
        });

        execute_correctness_test("Removal triggers Double (RL) Rotation", 15, "Complex removal causing rebalance", []() {
            AVLTester<int, string> avl;
            set<int> initial_keys = {10, 5, 30, 4, 6, 20, 40, 15, 25};
            for (int k : initial_keys) avl.insert(k, "");
            avl.remove(4);
            set<int> expected_keys = {10, 5, 30, 6, 20, 40, 15, 25};
            return avl.isBSTValid(expected_keys) && avl.isTreeBalanced();
        });

        execute_correctness_test("Mixed Insert/Remove Stress Test", 15, "Insert 200, Remove 100, Insert 50; check balance throughout.", []() {
            AVLTester<int, string> avl;
            std::mt19937 rng(123);
            vector<int> data;
            for(int i = 0; i < 400; ++i) data.push_back(i);
            std::shuffle(data.begin(), data.end(), rng);

            vector<int> to_insert(data.begin(), data.begin() + 200);
            vector<int> to_remove(data.begin(), data.begin() + 100);
            vector<int> to_re_insert(data.begin() + 200, data.begin() + 250);
            set<int> current_keys;

            for (int key : to_insert) {
                avl.insert(key, "");
                current_keys.insert(key);
                if (!avl.isTreeBalanced()) return false;
            }
            if (!avl.isBSTValid(current_keys)) return false;

            for (int key : to_remove) {
                avl.remove(key);
                current_keys.erase(key);
                if (!avl.isTreeBalanced()) return false;
            }
            if (!avl.isBSTValid(current_keys)) return false;

            for (int key : to_re_insert) {
                avl.insert(key, "");
                current_keys.insert(key);
                if (!avl.isTreeBalanced()) return false;
            }
            return avl.isBSTValid(current_keys) && avl.isTreeBalanced();
        });
    }

    void plot_graph(const string& title, const string& y_axis_label, const vector<double>& y_values, const vector<int>& x_values) {
        cout << "\n--- " << title << " ---" << endl;
        double max_y = *max_element(y_values.begin(), y_values.end());
        int plot_width = 50;
        for (size_t i = 0; i < x_values.size(); ++i) {
            int num_chars = (max_y > 0) ? static_cast<int>((y_values[i] / max_y) * plot_width) : 0;
            cout << "  n = " << setw(7) << left << x_values[i]
                 << setw(15) << right << fixed << setprecision(2) << y_values[i] << " " << y_axis_label << " |"
                 << string(num_chars, '*') << endl;
        }
        cout << "--------------------------------------------------------" << endl;
    }

    void test_performance() {
        cout << "\n--- Part 3: O(log n) Performance Analysis ---" << endl;
        vector<int> sizes = {10, 100, 1000, 10000, 50000};
        vector<double> comparison_results, time_results, n_log_n_approximations;

        auto counting_comparator = [](const int& a, const int& b) {
            comparison_steps++;
            return a < b;
        };
        
        std::mt19937 rng(std::chrono::steady_clock::now().time_since_epoch().count());

        for (int n : sizes) {
            cout << "\n  Testing insert performance with n = " << n << "..." << endl;
            AVLTester<int, string> avl(counting_comparator);
            vector<int> data(n * 2);
            for(size_t i = 0; i < data.size(); ++i) data[i] = i;
            std::shuffle(data.begin(), data.end(), rng);
            data.resize(n);
            comparison_steps = 0;
            auto start_time = std::chrono::high_resolution_clock::now();
            for (int key : data) avl.insert(key, "v");
            auto end_time = std::chrono::high_resolution_clock::now();
            std::chrono::duration<double, std::milli> elapsed_ms = end_time - start_time;
            time_results.push_back(elapsed_ms.count());
            comparison_results.push_back(comparison_steps);
            n_log_n_approximations.push_back(n * log2(n));
        }

        cout << "\n\n--- Performance Results Summary ---" << endl;
        cout << "The number of comparisons should grow similarly to n*log(n)." << endl;
        cout << left << setw(10) << "N" << setw(20) << "Total Comparisons" << setw(20) << "C * n*log(n)" << setw(20) << "Time (ms)" << endl;
        cout << "----------------------------------------------------------------" << endl;
        double constant_c = comparison_results.back() / n_log_n_approximations.back();
        for (size_t i = 0; i < sizes.size(); ++i) {
            cout << left << setw(10) << sizes[i] << setw(20) << fixed << setprecision(0) << comparison_results[i]
                 << setw(20) << fixed << setprecision(0) << (constant_c * n_log_n_approximations[i])
                 << setw(20) << fixed << setprecision(3) << time_results[i] << endl;
        }

        plot_graph("Plot: Total Comparisons vs. N", "steps", comparison_results, sizes);
        plot_graph("Plot: Execution Time vs. N", "ms", time_results, sizes);
        cout << "\nTo generate a real graph, copy the summary table into a spreadsheet." << endl;
    }
};

int main() {
    TestRunner runner;
    runner.run_all_tests();
    return 0;
}