#include <iostream>
#include <vector>
#include <string>
#include <functional>
#include <memory>
#include <algorithm>
#include <iomanip>
#include <stdexcept>
#include <set>
#include <map>
#include <queue>

// Include the header for the code being tested
#include "category_tree.h"
#include "post.h"

using namespace std;
class CategoryTreeTester : public CategoryTree {
public:
    using CategoryTree::CategoryTree;


    bool verify_get_posts(const string& categoryPath, bool includeSubcategories, const vector<Post*>& expected_posts) {
        // Call the student's function to get the actual result.
        vector<Post*> actual_posts = this->getPostsInCategory(categoryPath, includeSubcategories);

        // For robust comparison, we'll compare the IDs of the posts, as order is not guaranteed.
        set<PostID> actual_ids;
        for (const auto& post : actual_posts) {
            actual_ids.insert(post->postID);
        }

        set<PostID> expected_ids;
        for (const auto& post : expected_posts) {
            expected_ids.insert(post->postID);
        }

        if (actual_ids.size() != expected_ids.size()) {
            cout << "\n    [FAIL] Mismatch in number of posts returned. Expected: " << expected_ids.size() << ", Actual: " << actual_ids.size();
            return false;
        }

        if (actual_ids != expected_ids) {
            cout << "\n    [FAIL] The set of returned posts does not match the expected set.";
            return false;
        }

        return true;
    }
    
    shared_ptr<CategoryNode> verify_find(const string& path) {
        if (path.empty() || path == "root") return this->root;
        
        vector<string> categories = this->parseCategoryPath(path);
        shared_ptr<CategoryNode> current = this->root;
        for (const auto& cat_name : categories) {
            shared_ptr<CategoryNode> next = nullptr;
            if (current) {
                for (const auto& child : current->children) {
                    if (child->categoryName == cat_name) {
                        next = child;
                        break;
                    }
                }
            }
            current = next;
        }
        return current;
    }
    
    /**
     * @brief Verifies the entire parent-child structure of the tree.
     * @param expected_structure A vector of pairs {parent_name, child_name}.
     * @return True if the tree's structure matches exactly, false otherwise.
     */
    bool verify_structure(const vector<pair<string, string>>& expected_structure, const vector<string>& empty_categories) {
        map<string, set<string>> actual_structure;
        queue<shared_ptr<CategoryNode>> q;
        if (this->root) q.push(this->root);
        while (!q.empty()) {
            shared_ptr<CategoryNode> node = q.front();
            cout<<node->categoryName<<endl;
            q.pop();
            set<string> children_names;
            for (const auto& child : node->children) {
                cout<<child->categoryName<<", ";
                children_names.insert(child->categoryName);
                q.push(child);
            }
            cout<<endl<<"-----------------"<<endl;
            actual_structure[node->categoryName] = children_names;
        }

        map<string, set<string>> expected_map;
        for (const auto& p : expected_structure) {
            expected_map[p.first].insert(p.second);
        }
        for (const auto& p: empty_categories){
            expected_map[p];
        }

        for (const auto& pair : actual_structure) {
        std::cout << "  Key: " << pair.first<<endl<<"val: ";
        for (const auto& element : pair.second){
            std::cout<<element<< ", ";
        }
        cout<<endl;
        }
        cout<<"-------------------------"<<endl;

        for (const auto& pair : expected_map) {
        std::cout << "  Key: " << pair.first<<endl<<"val: ";
        for (const auto& element : pair.second){
            std::cout<<element<< ", ";
        }
        cout<<endl;
        }

        return actual_structure == expected_map;
    }

    /**
     * @brief Verifies the total post counts for specific categories.
     * @param expected_counts A vector of pairs {category_path, expected_total_posts}.
     * @return True if all counts match, false otherwise.
     */
    bool verify_post_counts(const vector<pair<string, int>>& expected_counts) {
        for (const auto& p : expected_counts) {
            shared_ptr<CategoryNode> node = verify_find(p.first);
            if (!node) {
                cout << "\n    [FAIL] Could not find node '" << p.first << "' to check count.";
                return false;
            }
            if (node->totalPostCount != p.second) {
                cout << "\n    [FAIL] Post count mismatch for '" << p.first << "'. Expected: " << p.second << ", Actual: " << node->totalPostCount;
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
        cout << "=======================================================================" << endl;
        cout << "                 Category Tree Implementation Tester" << endl;
        cout << "=======================================================================" << endl;
        
        test_category_node();
        test_category_tree_manipulation();
        test_post_retrieval();
        test_category_tree_iterators();

        cout << "\n-----------------------------------------------------------------------" << endl;
        cout << "                           TESTING SUMMARY" << endl;
        cout << "-----------------------------------------------------------------------" << endl;
        cout << "  FINAL SCORE: " << total_score << " / " << max_score << endl;
        if (total_score == max_score) {
            cout << "  RESULT: All tests passed!" << endl;
        } else {
            cout << "  RESULT: Some tests failed." << endl;
        }
        cout << "=======================================================================" << endl;
    }

private:
    int total_score;
    int max_score;
    
    void execute_test(const string& name, int points, const string& desc, const function<bool()>& test_func) {
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

    void test_category_node() {
        cout << "\n--- Part 1: CategoryNode Unit Tests ---" << endl;
        execute_test("Node addChild", 5, "Creating parent, adding child, checking link.", []() {
            auto parent = make_shared<CategoryNode>("Parent");
            auto child = make_shared<CategoryNode>("Child");
            parent->addChild(child);
            return parent->children.size() == 1 && parent->children[0]->categoryName == "Child" && child->parent.lock() == parent;
        });
        execute_test("Node removeChild", 5, "Adding two children, removing one by name.", []() {
            auto parent = make_shared<CategoryNode>("Parent");
            parent->addChild(make_shared<CategoryNode>("Child1"));
            parent->addChild(make_shared<CategoryNode>("Child2"));
            bool removed = parent->removeChild("Child1");
            return removed && parent->children.size() == 1 && parent->children[0]->categoryName == "Child2";
        });
        execute_test("Node findChild", 5, "Adding a child and verifying findChild retrieves it.", []() {
            auto parent = make_shared<CategoryNode>("Parent");
            auto child = make_shared<CategoryNode>("Child");
            parent->addChild(child);
            return parent->findChild("Child") == child;
        });
    }

        void test_category_tree_manipulation() {
        cout << "\n--- Part 2: CategoryTree Manipulation and Post Management ---" << endl;
        
        // --- Sub-section: Category Addition ---
        cout << "\n  --- Testing Category Addition ---" << endl;

        execute_test("ADD-1: Simple Root Children", 5, "Adding 'A', 'B', 'C' to root.", []() {
            CategoryTreeTester tree;
            tree.addCategory("A"); tree.addCategory("B"); tree.addCategory("C");
            return tree.verify_structure({{"root", "A"}, {"root", "B"}, {"root", "C"}}, {"A", "B", "C"});
        });
        execute_test("ADD-2: Deep Linear Path", 5, "Adding 'A_B_C_D_E'.", []() {
            CategoryTreeTester tree;
            tree.addCategory("A_B_C_D_E");
            return tree.verify_structure({{"root", "A"}, {"A", "B"}, {"B", "C"}, {"C", "D"}, {"D", "E"}}, {"E"});
        });
        execute_test("ADD-3: Branching from Existing Node", 5, "Adding 'A_B', then 'A_C'.", []() {
            CategoryTreeTester tree;
            tree.addCategory("A_B"); tree.addCategory("A_C");
            return tree.verify_structure({{"root", "A"}, {"A", "B"}, {"A", "C"}}, {"B", "C"});
        });
        execute_test("ADD-4: Adding a Redundant Path", 5, "Adding 'A_B', then adding 'A'.", []() {
            CategoryTreeTester tree;
            tree.addCategory("A_B");
            bool result = tree.addCategory("A"); // Should do nothing and return false
            return !result && tree.verify_structure({{"root", "A"}, {"A", "B"}}, {"B"});
        });
        execute_test("ADD-5: Adding Duplicate Full Path", 5, "Adding 'A_B', then 'A_B' again.", []() {
            CategoryTreeTester tree;
            tree.addCategory("A_B");
            bool result = tree.addCategory("A_B"); // Should do nothing and return false
            return !result && tree.verify_structure({{"root", "A"}, {"A", "B"}}, {"B"});
        });
        execute_test("ADD-6: Edge Case - Empty Path", 5, "Attempting to add an empty string path.", []() {
            CategoryTreeTester tree;
            bool result = tree.addCategory("");
            return !result && tree.verify_structure({}, {"root"});
        });
        execute_test("ADD-7: Edge Case - Path with Trailing Underscore", 5, "Adding 'A_B_'.", []() {
            CategoryTreeTester tree;
            tree.addCategory("A_B_");
            return tree.verify_structure({{"root", "A"}, {"A", "B"}}, {"B"});
        });
        execute_test("ADD-8: Interleaved Deep Paths", 5, "Adding 'A_B_C' then 'X_Y_Z'.", []() {
            CategoryTreeTester tree;
            tree.addCategory("A_B_C"); tree.addCategory("X_Y_Z");
            return tree.verify_structure({{"root", "A"}, {"A", "B"}, {"B", "C"}, {"root", "X"}, {"X", "Y"}, {"Y", "Z"}}, {"C", "Z"});
        });
        execute_test("ADD-9: Adding to Multiple Sub-Branches", 5, "Create 'A_B', 'C_D', then add 'A_E' and 'C_F'.", []() {
            CategoryTreeTester tree;
            tree.addCategory("A_B"); tree.addCategory("C_D");
            tree.addCategory("A_E"); tree.addCategory("C_F");
            return tree.verify_structure({{"root", "A"}, {"root", "C"}, {"A", "B"}, {"A", "E"}, {"C", "D"}, {"C", "F"}}, {"B", "E", "D", "F"});
        });
        execute_test("ADD-10: Stress - Add 100 Children to Root", 5, "Adding 'Cat_0' through 'Cat_99'.", []() {
            CategoryTreeTester tree;
            vector<pair<string, string>> expected_struct;
            vector<string> expected_leaves;
            for(int i = 0; i < 100; ++i) {
                string cat = "Cat-" + to_string(i);
                tree.addCategory(cat);
                expected_struct.push_back({"root", cat});
                expected_leaves.push_back(cat);
            }
            return tree.verify_structure(expected_struct, expected_leaves);
        });

        // --- Sub-section: Category Deletion ---
        cout << "\n  --- Testing Category Deletion ---" << endl;

        execute_test("REM-1: Remove Leaf Node", 5, "Adding 'A_B_C', then removing 'A_B_C'.", []() {
            CategoryTreeTester tree;
            tree.addCategory("A_B_C");
            bool result = tree.removeCategory("A_B_C");
            return result && tree.verify_structure({{"root", "A"}, {"A", "B"}}, {"B"});
        });
        execute_test("REM-2: Remove Node with Children (Subtree)", 5, "Adding 'A_B_C' and 'A_B_D', then removing 'A_B'.", []() {
            CategoryTreeTester tree;
            tree.addCategory("A_B_C"); tree.addCategory("A_B_D");
            bool result = tree.removeCategory("A_B");
            return result && tree.verify_structure({{"root", "A"}}, {"A"});
        });
        execute_test("REM-3: Remove Non-Existent Leaf", 5, "Adding 'A_B', attempting to remove 'A_C'.", []() {
            CategoryTreeTester tree;
            tree.addCategory("A_B");
            bool result = tree.removeCategory("A_C");
            return !result && tree.verify_structure({{"root", "A"}, {"A", "B"}}, {"B"});
        });
        execute_test("REM-4: Remove Non-Existent Path", 5, "Adding 'A_B', attempting to remove 'X_Y'.", []() {
            CategoryTreeTester tree;
            tree.addCategory("A_B");
            bool result = tree.removeCategory("X_Y");
            return !result && tree.verify_structure({{"root", "A"}, {"A", "B"}}, {"B"});
        });
        execute_test("REM-5: Edge Case - Remove root", 5, "Attempting to remove 'root'. Should fail.", []() {
            CategoryTreeTester tree;
            tree.addCategory("A");
            bool result = tree.removeCategory("root");
            return !result && tree.verify_structure({{"root", "A"}}, {"A"});
        });
        execute_test("REM-6: Remove First of Multiple Children", 5, "Adding 'A','B','C', remove 'A'.", []() {
            CategoryTreeTester tree;
            tree.addCategory("A"); tree.addCategory("B"); tree.addCategory("C");
            tree.removeCategory("A");
            return tree.verify_structure({{"root", "B"}, {"root", "C"}}, {"B", "C"});
        });
        execute_test("REM-7: Remove Middle of Multiple Children", 5, "Adding 'A','B','C', remove 'B'.", []() {
            CategoryTreeTester tree;
            tree.addCategory("A"); tree.addCategory("B"); tree.addCategory("C");
            tree.removeCategory("B");
            return tree.verify_structure({{"root", "A"}, {"root", "C"}}, {"A", "C"});
        });
        execute_test("REM-8: Remove Last of Multiple Children", 5, "Adding 'A','B','C', remove 'C'.", []() {
            CategoryTreeTester tree;
            tree.addCategory("A"); tree.addCategory("B"); tree.addCategory("C");
            tree.removeCategory("C");
            return tree.verify_structure({{"root", "A"}, {"root", "B"}}, {"A", "B"});
        });
        execute_test("REM-9: Edge Case - Path with double underscore", 5, "Adding 'A__B', attempt to remove 'A__B'.", []() {
            CategoryTreeTester tree;
            tree.addCategory("A__B");
            bool result = tree.removeCategory("A__B");
            return result && tree.verify_structure({{"root", "A"}}, {"A"});
        });
        execute_test("REM-10: Stress - Remove from Middle of Long Chain", 5, "Create 'A_B_C_D_E', remove 'A_B_C'.", []() {
            CategoryTreeTester tree;
            tree.addCategory("A_B_C_D_E");
            tree.removeCategory("A_B_C");
            return tree.verify_structure({{"root", "A"}, {"A", "B"}}, {"B"});
        });
        
        // --- Sub-section: Post Management ---
        cout << "\n  --- Testing Post Management ---" << endl;

        execute_test("POST-1: Add Posts & Verify Counts", 5, "Adding posts to 'A','A_B','A_B_C'.", []() {
            CategoryTreeTester tree;
            tree.addCategory("A_B_C");
            Post p1(1, "A"), p2(2, "A_B"), p3(3, "A_B_C");
            tree.addPost(&p1); tree.addPost(&p2); tree.addPost(&p3);
            return tree.verify_post_counts({{"root", 3}, {"A", 3}, {"A_B", 2}, {"A_B_C", 1}});
        });
        execute_test("POST-2: Remove Posts & Verify Counts", 5, "Removing post from 'A_B' and checking propagation.", []() {
            CategoryTreeTester tree;
            tree.addCategory("A_B_C");
            Post p1(1, "A"), p2(2, "A_B"), p3(3, "A_B_C");
            tree.addPost(&p1); tree.addPost(&p2); tree.addPost(&p3);
            tree.removePost(&p2);
            return tree.verify_post_counts({{"root", 2}, {"A", 2}, {"A_B", 1}, {"A_B_C", 1}});
        });
        execute_test("POST-3: Add Multiple Posts to Same Category", 5, "Adding 5 posts to 'A_B'.", []() {
            CategoryTreeTester tree;
            tree.addCategory("A_B");
            Post p1(1,"A_B"), p2(2,"A_B"), p3(3,"A_B"), p4(4,"A_B"), p5(5,"A_B");
            tree.addPost(&p1); tree.addPost(&p2); tree.addPost(&p3); tree.addPost(&p4); tree.addPost(&p5);
            return tree.verify_post_counts({{"root", 5}, {"A", 5}, {"A_B", 5}});
        });
        execute_test("POST-4: Remove Non-Existent Post", 5, "Removing a post not in the tree.", []() {
            CategoryTreeTester tree;
            tree.addCategory("A");
            Post p1(1, "A"), p_other(99, "A");
            tree.addPost(&p1);
            bool result = tree.removePost(&p_other); // Should fail
            return !result && tree.verify_post_counts({{"root", 1}, {"A", 1}});
        });
        execute_test("POST-5: Stress - Add/Remove Many Posts", 5, "Adding 100 posts, then removing 50.", []() {
            CategoryTreeTester tree;
            tree.addCategory("A_B");
            vector<Post> posts;
            for(int i=0; i<100; ++i) posts.emplace_back(i, i < 50 ? "A" : "A_B");
            for(auto& p : posts) tree.addPost(&p);
            if (!tree.verify_post_counts({{"root", 100}, {"A", 100}, {"A_B", 50}})) return false;
            for(int i=0; i<50; ++i) tree.removePost(&posts[i]);
            return tree.verify_post_counts({{"root", 50}, {"A", 50}, {"A_B", 50}});
        });
        
        // --- Sub-section: Advanced Dynamic Tests ---
        cout << "\n  --- Testing Advanced Dynamic Scenarios ---" << endl;
        
        execute_test("DYN-1: Move Category with Posts", 10, "Moving 'A_B' (with posts) to 'C'.", []() {
            CategoryTreeTester tree;
            tree.addCategory("A_B"); tree.addCategory("C");
            Post p1(1, "A"), p2(2, "A_B");
            tree.addPost(&p1); tree.addPost(&p2);
            tree.moveCategory("A_B", "C");
            return tree.verify_structure({{"root", "A"}, {"root", "C"}, {"C", "B"}}, {"A", "B"}) &&
                   tree.verify_post_counts({{"root", 2}, {"A", 1}, {"C", 1}, {"C_B", 1}});
        });
        execute_test("DYN-2: Remove Category with Posts", 10, "Removing 'A_B' (with posts). Should fail.", []() {
            CategoryTreeTester tree;
            tree.addCategory("A_B");
            Post p1(1, "A_B");
            tree.addPost(&p1);
            bool result = tree.removeCategory("A_B");
            return result && tree.verify_post_counts({{"root", 0}, {"A", 0}});
        });
        execute_test("DYN-3: Remove Category after its Posts are Removed", 10, "Removing 'A_B' after its posts are gone.", []() {
            CategoryTreeTester tree;
            tree.addCategory("A_B");
            Post p1(1, "A_B");
            tree.addPost(&p1);
            tree.removePost(&p1);
            bool result = tree.removeCategory("A_B");
            return result && tree.verify_structure({{"root", "A"}}, {"A"}) &&
                   tree.verify_post_counts({{"root", 0}, {"A", 0}});
        });
        execute_test("DYN-4: Add post to moved category", 10, "Move 'A_B' to 'C', then add post to 'C_B'.", []() {
            CategoryTreeTester tree;
            tree.addCategory("A_B"); tree.addCategory("C");
            tree.moveCategory("A_B", "C");
            Post p1(1, "C_B"); // Use the NEW path
            tree.addPost(&p1);
            return tree.verify_post_counts({{"root", 1}, {"A", 0}, {"C", 1}, {"C_B", 1}});
        });
        execute_test("DYN-5: Interleaved Add/Remove/Post", 10, "Seq: Add A_B, Add P1(A), Rem A, Add C, Add P2(C)", []() {
            CategoryTreeTester tree;
            Post p1(1, "A"), p2(2, "C");
            tree.addCategory("A_B");
            tree.addPost(&p1);
            if (!tree.verify_post_counts({{"A",1}})) return false;
            tree.removePost(&p1); // Must clear posts before removing parent
            tree.removeCategory("A");
            if (!tree.verify_structure({}, {"root"})) return false;
            tree.addCategory("C");
            tree.addPost(&p2);
            return tree.verify_structure({{"root", "C"}}, {"C"}) &&
                   tree.verify_post_counts({{"root", 1}, {"C", 1}});
        });
        execute_test("DYN-6: Move into deep branch", 10, "Create A_B_C, D_E, then move C to D_E.", []() {
            CategoryTreeTester tree;
            tree.addCategory("A_B_C"); tree.addCategory("D_E");
            Post p1(1, "A_B_C");
            tree.addPost(&p1);
            tree.moveCategory("A_B_C", "D_E");
            return tree.verify_structure({{"root", "A"}, {"A", "B"}, {"root", "D"}, {"D", "E"}, {"E", "C"}}, {"B", "C"}) &&
                   tree.verify_post_counts({{"root", 1}, {"D", 1}, {"D_E", 1}, {"D_E_C", 1}, {"A", 0}, {"A_B", 0}});
        });
        execute_test("DYN-7: Move from deep branch to root", 10, "Create A_B_C, move C to root.", []() {
            CategoryTreeTester tree;
            tree.addCategory("A_B_C");
            Post p1(1, "A_B_C");
            tree.addPost(&p1);
            tree.moveCategory("A_B_C", "");
            return tree.verify_structure({{"root", "A"}, {"A", "B"}, {"root", "C"}}, {"B", "C"}) &&
                   tree.verify_post_counts({{"root", 1}, {"C", 1}, {"A", 0}, {"A_B", 0}});
        });
        execute_test("DYN-8: Add post to non-existent category", 10, "Add post with path 'X_Y' to empty tree.", []() {
            CategoryTreeTester tree;
            Post p1(1, "X_Y");
            tree.addPost(&p1);
            return tree.verify_structure({{"root", "X"}, {"X", "Y"}}, {"Y"}) &&
                   tree.verify_post_counts({{"root", 1}, {"X", 1}, {"X_Y", 1}});
        });
        execute_test("DYN-9: Move parent of post's category", 10, "Create A_B_C, add post to C, move A.", []() {
            CategoryTreeTester tree;
            tree.addCategory("A_B_C"); tree.addCategory("D");
            Post p1(1, "A_B_C");
            tree.addPost(&p1);
            tree.moveCategory("A", "D");
            return tree.verify_structure({{"root", "D"}, {"D", "A"}, {"A", "B"}, {"B", "C"}}, {"C"}) &&
                   tree.verify_post_counts({{"root", 1}, {"D", 1}, {"D_A", 1}, {"D_A_B", 1}, {"D_A_B_C", 1}});
        });
    }

        void test_post_retrieval() {
        cout << "\n  --- Testing Post Retrieval (getPostsInCategory) ---" << endl;
        
        // --- Test Setup ---
        // A standard tree is created once with a variety of posts.
        // All tests in this suite will run against this single tree instance.
        CategoryTreeTester tree;
        tree.addCategory("Tech_Hardware_CPU");
        tree.addCategory("Tech_Hardware_GPU");
        tree.addCategory("Tech_Software");
        tree.addCategory("Finance");

        // Create posts to be used in the tests.
        // In a real app these would be dynamically allocated.
        // Static/global lifetime simplifies memory management for the test suite.
        static Post p1(101, "Tech"), p2(102, "Tech_Hardware"), p3(103, "Tech_Hardware_CPU");
        static Post p4(201, "Tech_Software"), p5(202, "Tech_Software");
        static Post p6(301, "Finance");

        tree.addPost(&p1); tree.addPost(&p2); tree.addPost(&p3);
        tree.addPost(&p4); tree.addPost(&p5); tree.addPost(&p6);
        
        // Expected Final Post Counts for reference:
        // CPU(1), GPU(0), Hardware(1 direct + 1 from CPU = 2), Software(2),
        // Tech(1 direct + 2 from Hardware + 2 from Software = 5),
        // Finance(1), root(6)
        // --- End Setup ---

        execute_test("GET-1: Leaf Node (No Subcategories)", 5, "Getting posts from 'Tech_Hardware_CPU' (subs=false). Expecting 1 post.", [&]() {
            // Should only get posts directly in this category.
            return tree.verify_get_posts("Tech_Hardware_CPU", false, {&p3});
        });

        execute_test("GET-2: Parent Node (No Subcategories)", 5, "Getting posts from 'Tech_Hardware' (subs=false). Expecting 1 post.", [&]() {
            // Should only get posts directly in 'Tech_Hardware', ignoring posts in its child 'CPU'.
            return tree.verify_get_posts("Tech_Hardware", false, {&p2});
        });

        execute_test("GET-3: Parent Node (With Subcategories)", 10, "Getting posts from 'Tech_Hardware' (subs=true). Expecting 2 posts.", [&]() {
            // Should get posts from 'Tech_Hardware' AND its child 'CPU'.
            return tree.verify_get_posts("Tech_Hardware", true, {&p2, &p3});
        });

        execute_test("GET-4: Top-Level Node (With Subcategories)", 10, "Getting posts from 'Tech' (subs=true). Expecting 5 posts.", [&]() {
            // Should get posts from 'Tech', 'Tech_Hardware', 'Tech_Hardware_CPU', and 'Tech_Software'.
            return tree.verify_get_posts("Tech", true, {&p1, &p2, &p3, &p4, &p5});
        });

        execute_test("GET-5: Edge Case - Category with No Posts", 5, "Getting posts from 'Tech_Hardware_GPU' (subs=true). Expecting 0 posts.", [&]() {
            // Should return an empty vector correctly.
            return tree.verify_get_posts("Tech_Hardware_GPU", true, {});
        });
    }
    
    void test_category_tree_iterators() {
        cout << "\n--- Part 3: CategoryTree Iterator Tests ---" << endl;
        
        // Setup a standard tree for all iterator tests
        CategoryTreeTester tree;
        tree.addCategory("F_A"); tree.addCategory("F_B");
        tree.addCategory("S_C");
        tree.addCategory("T_D"); tree.addCategory("T_E");

        execute_test("Pre-Order Iterator", 10, "Traversing {F(A,B), S(C), T(D,E)}", [&tree]() {
            vector<string> expected = {"root", "F", "A", "B", "S", "C", "T", "D", "E"};
            vector<string> actual;
            for (auto it = tree.preOrderBegin(); it != tree.preOrderEnd(); ++it) {
                actual.push_back((*it)->categoryName);
            }
            return actual == expected;
        });

        execute_test("Post-Order Iterator", 10, "Traversing {F(A,B), S(C), T(D,E)}", [&tree]() {
            vector<string> expected = {"A", "B", "F", "C", "S", "D", "E", "T", "root"};
            vector<string> actual;
            for (auto it = tree.postOrderBegin(); it != tree.postOrderEnd(); ++it) {
                actual.push_back((*it)->categoryName);
            }
            return actual == expected;
        });

        execute_test("Breadth-First Iterator", 10, "Traversing {F(A,B), S(C), T(D,E)}", [&tree]() {
            vector<string> expected = {"root", "F", "S", "T", "A", "B", "C", "D", "E"};
            vector<string> actual;
            for (auto it = tree.breadthFirstBegin(); it != tree.breadthFirstEnd(); ++it) {
                actual.push_back((*it)->categoryName);
            }
            return actual == expected;
        });
    }
};

int main() {
    // Create some posts that will be used across tests
    // Note: In a real app, these would be dynamically allocated.
    // Here we use static to avoid memory management issues in the test suite.
    Post p1(1, "Sports_Soccer");
    Post p2(2, "Sports_Soccer_Leagues");
    Post p3(3, "Sports_Basketball");
    Post p4(4, "A_B_C");

    TestRunner runner;
    runner.run_all_tests();

    return 0;
}