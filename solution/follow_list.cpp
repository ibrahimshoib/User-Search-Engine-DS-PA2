#include "../headers/follow_list.h"
#include "../headers/user.h"
#include <iostream>
using namespace std;

FollowList::~FollowList() {
    while (head) {
        FollowNode* temp = head;
        head = head->next;
        delete temp;
    }
}

void FollowList::addFollowing(User* u) {
    if (!u) return;
    
    // Check if already following to avoid duplicates
    if (findFollowing(u->userID)) return;
    
    FollowNode* newNode = new FollowNode(u);
    newNode->next = head;
    head = newNode;
}

bool FollowList::removeFollowing(int userID) {
    FollowNode* current = head;
    FollowNode* prev = nullptr;
    
    while (current) {
        if (current->user && current->user->userID == userID) {
            if (prev) {
                prev->next = current->next;
            } else {
                head = current->next;
            }
            delete current;
            return true;
        }
        prev = current;
        current = current->next;
    }
    return false;
}

User* FollowList::findFollowing(int userID) {
    // Add extra safety checks
    FollowNode* current = head;
    int iterations = 0; // Prevent infinite loops
    const int MAX_ITERATIONS = 10000; // Safety limit
    
    while (current && iterations < MAX_ITERATIONS) {
        // Extra safety: check if current is a valid pointer
        if (!current) {
            cout << "ERROR: current pointer became null unexpectedly!" << endl;
            break;
        }
        
        // Check if user pointer is valid before dereferencing
        if (current->user) {
            if (current->user->userID == userID) {
                return current->user;
            }
        } else {
            cout << "WARNING: Found FollowNode with null user pointer!" << endl;
        }
        
        current = current->next;
        iterations++;
    }
    
    if (iterations >= MAX_ITERATIONS) {
        cout << "ERROR: Possible infinite loop detected in FollowList!" << endl;
    }
    
    return nullptr;
}

void FollowList::displayFollowing() const {
    FollowNode* current = head;
    cout << "Following: ";
    
    if (!current) {
        cout << "None";
    }
    
    while (current) {
        if (current->user) {
            cout << current->user->userName << " (ID: " << current->user->userID << ")";
            if (current->next) cout << ", ";
        } else {
            cout << "[NULL USER]";
            if (current->next) cout << ", ";
        }
        current = current->next;
    }
    cout << endl;
}