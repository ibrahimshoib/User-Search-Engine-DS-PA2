#include <iostream>
#include <cstdlib>
#include <string>
#include <vector>
#include <algorithm>
using namespace std;

// Adjust compiler + flags to match your Makefile
const string CXX = "g++";
const string CXXFLAGS = "-std=c++17 -Wall -g -Iheaders -Isolution";
const string SOLUTION_SRCS =
    "solution/category_tree.cpp "
    "solution/follow_list.cpp "
    "solution/linked_list.cpp "
    "solution/post_list.cpp "
    "solution/post_pool.cpp "
    "solution/user.cpp "
    "solution/user_manager.cpp "
    "solution/user_search_engine.cpp";
const string TESTS_DIR = "tests/";

// Structure for one test entry
struct TestEntry
{
    int id;
    string name;
    string sourceFile;
    string exeFile;
    int points;  // Points assigned based on complexity
};

// Table of all available tests with point distribution
// Total: 100 points
vector<TestEntry> tests = {
    {1, "BST Tester", "bst_test.cpp", "tests/bst_tester_exe", 20},
    {2, "AVL Tester", "avl_test.cpp", "tests/avl_tester_exe", 25},
    {3, "Category Tree Tester", "category_tree_test.cpp", "tests/category_tree_test_exe", 30},
    {4, "User Search Engine Tester", "user_search_engine_test.cpp", "tests/user_search_engine_tester_exe", 25}
};

void print_menu()
{
    cout << "======================================================" << endl;
    cout << "         Programming Assignment 2 - Test Suite        " << endl;
    cout << "======================================================" << endl;
    cout << "\n  Available Tests (Total: 100 points):\n" << endl;
    for (const auto &t : tests)
    {
        cout << "  " << t.id << ". " << t.name 
             << " (" << t.points << " points)" << endl;
    }
    cout << "\n  0. Exit" << endl;
    cout << "======================================================" << endl;
    cout << "Select a test to compile and run: ";
}

void print_scoring_breakdown()
{
    cout << "\n======================================================" << endl;
    cout << "              SCORING BREAKDOWN (100 pts)             " << endl;
    cout << "======================================================" << endl;
    cout << "  1. BST Tester                    : 20 points" << endl;
    cout << "     - Basic tree operations" << endl;
    cout << "     - Insert, remove, find" << endl;
    cout << "     - Range queries & traversal" << endl;
    cout << "\n  2. AVL Tester                    : 25 points" << endl;
    cout << "     - Self-balancing operations" << endl;
    cout << "     - Rotations (Left, Right, LR, RL)" << endl;
    cout << "     - Balance maintenance" << endl;
    cout << "     - Most complex due to rebalancing logic" << endl;
    cout << "\n  3. Category Tree Tester          : 30 points" << endl;
    cout << "     - Hierarchical structure management" << endl;
    cout << "     - Path-based operations" << endl;
    cout << "     - Post count propagation" << endl;
    cout << "     - Multiple traversal iterators" << endl;
    cout << "\n  4. User Search Engine Tester     : 25 points" << endl;
    cout << "     - Dual-index consistency" << endl;
    cout << "     - Migration from PA1" << endl;
    cout << "     - Advanced fuzzy search" << endl;
    cout << "     - Edit distance algorithm" << endl;
    cout << "======================================================\n" << endl;
}

int main()
{
    print_scoring_breakdown();
    
    while (true)
    {
        print_menu();
        int choice;
        cin >> choice;
        
        if (cin.fail()) {
            cin.clear();
            cin.ignore(10000, '\n');
            cout << "Invalid input. Please enter a number." << endl;
            continue;
        }
        
        if (choice == 0)
        {
            cout << "\nExiting test runner. Good luck with your assignment!" << endl;
            break;
        }

        // Find the selected test
        auto it = find_if(tests.begin(), tests.end(),
                          [&](const TestEntry &t)
                          { return t.id == choice; });
        if (it == tests.end())
        {
            cout << "Invalid choice. Try again." << endl;
            continue;
        }

        const TestEntry &test = *it;
        cout << "\n>>> Compiling " << test.name << " (" << test.points << " points) ..." << endl;

        string compileCmd = CXX + " " + CXXFLAGS +
                            " " + SOLUTION_SRCS +
                            " " + TESTS_DIR + test.sourceFile +
                            " -o " + test.exeFile;

        if (system(compileCmd.c_str()) != 0)
        {
            cout << "[ERROR] Compilation failed for " << test.name << endl;
            cout << "Make sure all solution files are implemented." << endl;
            continue;
        }

        cout << ">>> Running " << test.name << " ..." << endl;
        string runCmd = "./" + test.exeFile;
        system(runCmd.c_str());

        cout << "\n======================================================" << endl;
        cout << "Test completed. " << test.name << " is worth " 
             << test.points << " points." << endl;
        cout << "======================================================" << endl;
        cout << endl;
    }
    return 0;
}