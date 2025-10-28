#pragma once
#include "avl_tree.h"
#include "../headers/linked_list.h"
#include "../headers/user.h"
#include <string>
#include <vector>
using namespace std;

/**
 * High-performance user search engine using AVL trees
 */
class UserSearchEngine {
protected:
    AVLTree<int, User*> usersByID;           // Primary index: userID -> User*
    AVLTree<string, User*> usersByName; // Secondary index: username -> User*

public:
    UserSearchEngine();
    ~UserSearchEngine();
    
    // Migration from PA1 - students must implement
    void migrateFromLinkedList(const LinkedList<User>& userList);
    
    // Basic user management - students must implement
    bool addUser(User* user);
    bool removeUser(int userID);
    bool removeUser(const string& username);
    
    // Search operations - students must implement
    User* searchByID(int userID) const;
    User* searchByUsername(const string& username) const;
    vector<User*> searchByUsernamePrefix(const string& prefix) const;
    vector<User*> getUsersInIDRange(int minID, int maxID) const;
    
    // Advanced search features - students must implement
    vector<User*> fuzzyUsernameSearch(const string& username, int maxEditDistance = 2) const;
    vector<User*> getAllUsersSorted(bool byID = true) const;
    
    // Statistics and utilities
    size_t getTotalUsers() const;
    void displaySearchStats() const;
    bool isConsistent() const;  // Verify both indices are in sync
    
private:
    // Helper methods for fuzzy search
    int calculateEditDistance(const string& str1, const string& str2) const;
    void collectPrefixMatches(const AVLTree<string, User*>& tree, const string& prefix, vector<User*>& results) const;
};

// #include "../solution/user_search_engine.cpp"