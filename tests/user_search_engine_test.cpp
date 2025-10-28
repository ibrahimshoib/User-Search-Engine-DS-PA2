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
#include <random>

// Include the header for the code being tested
#include "user_search_engine.h"
// Include the AVLTester we need for verification
#include "avl_test.h"

using namespace std;
class UserSearchEngineTester : public UserSearchEngine {
public:
    using UserSearchEngine::UserSearchEngine;

    /**
     * @brief The master verification function. Checks all internal data structures for consistency.
     */
    bool verify_engine_consistency(const set<User*>& expected_users) {
        // 1. Check total user count
        if (this->getTotalUsers() != expected_users.size()) {
            cout << "\n    [FAIL] getTotalUsers() mismatch. Expected: " << expected_users.size() << ", Actual: " << this->getTotalUsers();
            return false;
        }

        // 2. Verify usersByID tree
        set<int> expected_ids;
        for (User* u : expected_users) expected_ids.insert(u->userID);
        AVLTester<int, User*> id_tester;
        shared_ptr<typename BST<int, User*>::BSTNode> id_root = this->usersByID.getRoot();
        id_tester.setRoot(id_root);
        if (!id_tester.isBSTValid(expected_ids) || !id_tester.isTreeBalanced()) {
            cout << "\n    [FAIL] usersByID tree is invalid, unbalanced, or has incorrect content.";
            return false;
        }

        // 3. Verify usersByName tree
        set<string> expected_names;
        for (User* u : expected_users) expected_names.insert(u->userName);
        AVLTester<string, User*> name_tester;
        name_tester.setRoot(this->usersByName.getRoot());
        if (!name_tester.isBSTValid(expected_names) || !name_tester.isTreeBalanced()) {
            cout << "\n    [FAIL] usersByName tree is invalid, unbalanced, or has incorrect content.";
            return false;
        }

        return true;
    }
};


class TestRunner {
public:
    TestRunner() {
        // Create a pool of users that the test suite will own
        for(int i=0; i < 200; ++i) {
            user_pool.emplace_back(i, "user" + to_string(i));
        }
    }

    void run_all_tests() {
        cout << "=======================================================================" << endl;
        cout << "                 User Search Engine Tester" << endl;
        cout << "=======================================================================" << endl;
        
        test_migration_and_add();
        test_removal();
        test_searches();
        test_scoring_and_advanced();
        test_dynamic_stress();

        cout << "\n-----------------------------------------------------------------------" << endl;
        cout << "                           TESTING SUMMARY" << endl;
        cout << "-----------------------------------------------------------------------" << endl;
        cout << "  FINAL SCORE: " << total_score << " / " << max_score << endl;
        if (total_score == max_score) cout << "  RESULT: All tests passed!" << endl;
        else cout << "  RESULT: Some tests failed." << endl;
        cout << "=======================================================================" << endl;
    }

private:
    int total_score = 0;
    int max_score = 0;
    vector<User> user_pool;
    
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

    // --- Test Suites ---

    void test_migration_and_add() {
        cout << "\n--- Part 1: Data Population (migrateFromLinkedList & addUser) ---" << endl;

        execute_test("MIG-1: Migrate Empty List", 5, "Migrating from an empty list.", []() {
            UserSearchEngineTester engine;
            LinkedList<User> list;
            engine.migrateFromLinkedList(list);
            return engine.verify_engine_consistency({});
        });

        execute_test("MIG-2: Standard Migration", 10, "Migrating 10 unique users.", [&]() {
            UserSearchEngineTester engine;
            LinkedList<User> list;
            set<User*> expected_users;
            for(int i=0; i<10; ++i) {
                list.push_back(user_pool[i]);
                expected_users.insert(&user_pool[i]);
            }
            engine.migrateFromLinkedList(list);
            return engine.verify_engine_consistency(expected_users);
        });
        
        execute_test("MIG-3: Idempotent Migration", 5, "Migrating the same list twice. State should not change.", [&]() {
            UserSearchEngineTester engine;
            LinkedList<User> list;
            set<User*> expected_users;
            for(int i=0; i<10; ++i) { list.push_back(user_pool[i]); expected_users.insert(&user_pool[i]); }
            engine.migrateFromLinkedList(list);
            engine.migrateFromLinkedList(list); // Second call
            return engine.verify_engine_consistency(expected_users);
        });

        execute_test("MIG-4: Migration with Duplicate ID in List", 5, "List has {User(1,A), User(1,B)}. Engine should only contain one.", [&]() {
            UserSearchEngineTester engine;
            LinkedList<User> list;
            User u1(1, "userA"), u2(2, "userB"), u3(1, "userC_dupID");
            list.push_back(u1); list.push_back(u2); list.push_back(u3);
            engine.migrateFromLinkedList(list);
            return engine.getTotalUsers() == 2 && engine.searchByID(1) != nullptr && engine.searchByUsername("userC_dupID") == nullptr;
        });

        execute_test("MIG-5: Migration with Duplicate Name in List", 5, "List has {User(1,A), User(3,A)}. Engine should only contain the one that comes first.", [&]() {
            UserSearchEngineTester engine;
            LinkedList<User> list;
            User u1(1, "userA"), u2(2, "userB"), u3(3, "userA");
            list.push_back(u1); list.push_back(u2); list.push_back(u3);
            engine.migrateFromLinkedList(list);
            return engine.getTotalUsers() == 2 && engine.searchByUsername("userA") != nullptr && engine.searchByID(3) == nullptr;
        });

        execute_test("ADD-1: Add Unique User", 5, "Adding a new user to a populated engine.", [&]() {
            UserSearchEngineTester engine;
            set<User*> expected_users;
            for(int i=0; i<5; ++i) { engine.addUser(&user_pool[i]); expected_users.insert(&user_pool[i]); }
            bool result = engine.addUser(&user_pool[5]);
            expected_users.insert(&user_pool[5]);
            return result && engine.verify_engine_consistency(expected_users);
        });

        execute_test("ADD-2: Add Duplicate User ID", 5, "Attempting to add user with existing ID.", [&]() {
            UserSearchEngineTester engine;
            set<User*> expected_users;
            engine.addUser(&user_pool[0]); expected_users.insert(&user_pool[0]);
            User duplicate_id_user(0, "newUser");
            bool result = engine.addUser(&duplicate_id_user);
            return !result && engine.verify_engine_consistency(expected_users);
        });

        execute_test("ADD-3: Add Duplicate Username", 5, "Attempting to add user with existing username.", [&]() {
            UserSearchEngineTester engine;
            set<User*> expected_users;
            engine.addUser(&user_pool[0]); expected_users.insert(&user_pool[0]);
            User duplicate_name_user(999, "user0");
            bool result = engine.addUser(&duplicate_name_user);
            return !result && engine.verify_engine_consistency(expected_users);
        });
        
        execute_test("ADD-4: Edge Case - Add Null User", 5, "Calling addUser(nullptr).", [&]() {
            UserSearchEngineTester engine;
            set<User*> expected_users;
            for(int i=0; i<5; ++i) { engine.addUser(&user_pool[i]); expected_users.insert(&user_pool[i]); }
            bool result = engine.addUser(nullptr);
            return !result && engine.verify_engine_consistency(expected_users);
        });

        execute_test("ADD-5: Stress - Add 100 Unique Users", 10, "Adding 100 users sequentially.", [&]() {
            UserSearchEngineTester engine;
            set<User*> expected_users;
            for(int i=0; i<100; ++i) {
                if (!engine.addUser(&user_pool[i])) return false;
                expected_users.insert(&user_pool[i]);
            }
            return engine.verify_engine_consistency(expected_users);
        });
    }

    void test_removal() {
        cout << "\n--- Part 2: User Removal ---" << endl;

        execute_test("REM-1: Remove User by ID", 10, "Removing a user that is a leaf in both trees.", [&]() {
            UserSearchEngineTester engine;
            set<User*> expected_users;
            for(int i=0; i<10; ++i) { engine.addUser(&user_pool[i]); expected_users.insert(&user_pool[i]); }
            bool result = engine.removeUser(9);
            expected_users.erase(&user_pool[9]);
            return result && engine.searchByID(9) == nullptr && engine.verify_engine_consistency(expected_users);
        });

        execute_test("REM-2: Remove User by Username", 10, "Removing a user by their name.", [&]() {
            UserSearchEngineTester engine;
            set<User*> expected_users;
            for(int i=0; i<10; ++i) { engine.addUser(&user_pool[i]); expected_users.insert(&user_pool[i]); }
            bool result = engine.removeUser("user5");
            expected_users.erase(&user_pool[5]);
            return result && engine.searchByUsername("user5") == nullptr && engine.verify_engine_consistency(expected_users);
        });

        execute_test("REM-3: Remove Non-Existent User", 5, "Attempting to remove a user ID that isn't in the engine.", [&]() {
            UserSearchEngineTester engine;
            set<User*> expected_users;
            for(int i=0; i<5; ++i) { engine.addUser(&user_pool[i]); expected_users.insert(&user_pool[i]); }
            bool result = engine.removeUser(999);
            return !result && engine.verify_engine_consistency(expected_users);
        });

        execute_test("REM-4: Edge Case - Remove from Empty Engine", 5, "Calling removeUser(1) on an empty engine.", []() {
            UserSearchEngineTester engine;
            return !engine.removeUser(1);
        });

        execute_test("REM-5: Remove and Re-add User", 5, "Removing user 5, then adding them back.", [&]() {
            UserSearchEngineTester engine;
            set<User*> expected_users;
            for(int i=0; i<10; ++i) { engine.addUser(&user_pool[i]); expected_users.insert(&user_pool[i]); }
            engine.removeUser(5);
            bool re_add_result = engine.addUser(&user_pool[5]);
            return re_add_result && engine.verify_engine_consistency(expected_users);
        });

        execute_test("REM-6: Remove Root of ID Tree", 10, "Adding users 50-100, then removing ID 75 (likely root).", [&]() {
            UserSearchEngineTester engine;
            set<User*> expected_users;
            for(int i=50; i<100; ++i) { engine.addUser(&user_pool[i]); expected_users.insert(&user_pool[i]); }
            bool result = engine.removeUser(75);
            expected_users.erase(&user_pool[75]);
            return result && engine.verify_engine_consistency(expected_users);
        });

        execute_test("REM-7: Remove All Users", 10, "Adding 10 users, then removing all of them.", [&]() {
            UserSearchEngineTester engine;
            for(int i=0; i<10; ++i) { engine.addUser(&user_pool[i]); }
            for(int i=0; i<10; ++i) {
                if (!engine.removeUser(i)) return false;
            }
            return engine.verify_engine_consistency({});
        });
    }

    void test_searches() {
        cout << "\n--- Part 3: Search Operations ---" << endl;
        UserSearchEngineTester engine;
        for(int i=0; i<100; ++i) engine.addUser(&user_pool[i]);

        execute_test("SEARCH-1: Find by ID and Username (Success)", 5, "Finding existing user 50 ('user50').", [&]() {
            User* u_id = engine.searchByID(50);
            User* u_name = engine.searchByUsername("user50");
            return u_id != nullptr && u_name != nullptr && u_id == u_name && u_id == &user_pool[50];
        });

        execute_test("SEARCH-2: Find by ID and Username (Failure)", 5, "Searching for non-existent user 999.", [&]() {
            return engine.searchByID(999) == nullptr && engine.searchByUsername("user999") == nullptr;
        });

        execute_test("SEARCH-3: Prefix Search (Multiple Matches)", 10, "Searching for prefix 'user1'. Should find user1, user10-19.", [&]() {
            auto results = engine.searchByUsernamePrefix("user1");
            if (results.size() != 11) return false;
            return results[0]->userName == "user1" && results[1]->userName == "user10";
        });

        execute_test("SEARCH-4: ID Range Search", 10, "Getting users in ID range [15, 20].", [&]() {
            auto results = engine.getUsersInIDRange(15, 20);
            if (results.size() != 6) return false;
            return results[0]->userID == 15 && results.back()->userID == 20;
        });
        
        execute_test("SEARCH-5: Prefix Search (No Matches)", 5, "Searching for prefix 'xyz'.", [&]() {
            return engine.searchByUsernamePrefix("xyz").empty();
        });

        execute_test("SEARCH-6: Prefix Search (Empty Prefix)", 5, "Searching for prefix ''. Should return all users.", [&]() {
            return engine.searchByUsernamePrefix("").size() == 100;
        });

        execute_test("SEARCH-7: ID Range (Full Range)", 5, "Getting users in ID range [-100, 1000].", [&]() {
            return engine.getUsersInIDRange(-100, 1000).size() == 100;
        });

        execute_test("SEARCH-8: ID Range (Single Element)", 5, "Getting users in ID range [50, 50].", [&]() {
            auto results = engine.getUsersInIDRange(50, 50);
            return results.size() == 1 && results[0]->userID == 50;
        });

        execute_test("SEARCH-9: ID Range (Invalid Range)", 5, "Getting users in ID range [60, 50]. Should be empty.", [&]() {
            return engine.getUsersInIDRange(60, 50).empty();
        });
    }
    
    void test_scoring_and_advanced() {
        cout << "\n--- Part 4: Advanced Features ---" << endl;
        UserSearchEngineTester engine;
        for(int i=0; i<10; ++i) engine.addUser(&user_pool[i]);

        // Keep retrieval of sorted users (this uses existing APIs)
        execute_test("ADV-1: Get All Users Sorted", 10, "Getting all users sorted by ID, then by name.", [&]() {
            auto by_id = engine.getAllUsersSorted(true);
            auto by_name = engine.getAllUsersSorted(false);
            if(by_id.size() != 10 || by_name.size() != 10) return false;
            return by_id[0]->userID == 0 && by_name[0]->userName == "user0";
        });
        
        execute_test("ADV-2: Fuzzy Username Search", 10, "Searching for 'user_x' (dist 1) and 'uer1' (dist 1) near 'user1'.", [&]() {
            UserSearchEngineTester fuzzy_engine;
            User u1(100, "userx"), u2(101, "uer1"), u3(102, "uxer");
            fuzzy_engine.addUser(&user_pool[1]); // user1
            fuzzy_engine.addUser(&u1); fuzzy_engine.addUser(&u2); fuzzy_engine.addUser(&u3);
            auto results = fuzzy_engine.fuzzyUsernameSearch("user1", 1);
            if (results.size() != 3) return false;
            set<int> found_ids;
            for(auto u : results) found_ids.insert(u->userID);
            return found_ids.count(1) && found_ids.count(100) && found_ids.count(101);
        });

        execute_test("ADV-3: Fuzzy Search (No Matches)", 5, "Searching for 'xyz' with max distance 1.", [&]() {
            return engine.fuzzyUsernameSearch("xyz", 1).empty();
        });
    }

    void test_dynamic_stress() {
        cout << "\n--- Part 5: Dynamic Stress Test ---" << endl;

        execute_test("DYN-1: Mixed Operations", 15, "1000 random add/remove ops.", [&]() {
            UserSearchEngineTester engine;
            std::mt19937 rng(12345);
            set<User*> expected_users;
            for (int i = 0; i < 1000; ++i) {
                int user_idx = rng() % user_pool.size();
                User* u = &user_pool[user_idx];
                int op = rng() % 3;
                if (op == 0) {
                    if (expected_users.find(u) == expected_users.end()) {
                        engine.addUser(u); expected_users.insert(u);
                    }
                } else if (op == 1 && !expected_users.empty()) {
                    if (expected_users.find(u) != expected_users.end()) {
                        engine.removeUser(u->userID); expected_users.erase(u);
                    }
                } else {
                    // No scoring operations - skip
                }
                if (i > 0 && i % 100 == 0) {
                    if (!engine.isConsistent()) return false;
                }
            }
            return engine.verify_engine_consistency(expected_users);
        });
        
        execute_test("DYN-2: Rapid Add/Remove Same User", 10, "Adding and removing the same user 100 times.", [&]() {
            UserSearchEngineTester engine;
            for(int i=0; i<5; ++i) engine.addUser(&user_pool[i]);
            for(int i=0; i<100; ++i) {
                if(!engine.addUser(&user_pool[10])) return false;
                if(!engine.removeUser(10)) return false;
            }
            return engine.getTotalUsers() == 5;
        });
    }
};

int main() {
    TestRunner runner;
    runner.run_all_tests();
    return 0;
}
