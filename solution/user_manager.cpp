#include "../headers/user_manager.h"
#include "../headers/user.h"
#include "../headers/follow_list.h"
#include "../headers/post_pool.h"
#include <fstream>
#include <sstream>
#include <iostream>
using namespace std;

UserManager::UserManager() {}

UserManager::~UserManager() {}

LinkedList<User>::Node* UserManager::createUser(int userID, const string& username) {
    // Check for duplicate userID
    if (findUserByID(userID)) {
        return nullptr;  // User with this ID already exists
    }
    
    // Check for duplicate username (optional, based on requirements)
    if (findUserByName(username)) {
        return nullptr;  // User with this name already exists
    }
    
    User newUser(userID, username);
    LinkedList<User>::Node* result = users.push_back(newUser);
    
    // Verify the following pointer is still valid after insertion
    if (result && !result->data.following) {
        cout << "WARNING: following pointer became null after insertion!" << endl;
        result->data.following = new FollowList();
    }
    
    return result;
}

bool UserManager::deleteUser(int userID) {
    LinkedList<User>::Node* userNode = findUserByID(userID);
    if (!userNode) {
        return false;
    }
    
    // Remove this user from all other users' follow lists
    LinkedList<User>::Node* current = users.head();
    while (current) {
        if (current->data.userID != userID && current->data.following) {
            current->data.following->removeFollowing(userID);
        }
        current = current->next;
    }
    
    // Remove the user from the main list
    users.remove(userNode);
    return true;
}

bool UserManager::follow(int followerID, int followeeID) {
    // Can't follow yourself
    if (followerID == followeeID) {
        return false;
    }

    LinkedList<User>::Node* followerNode = findUserByID(followerID);
    LinkedList<User>::Node* followeeNode = findUserByID(followeeID);
    
    if (!followerNode || !followeeNode) {
        cout << "follow: One or both users don't exist" << endl;
        return false;  // One or both users don't exist
    }
    
    // Check if following pointer is null and try to reinitialize if needed
    if (!followerNode->data.following) {
        cout << "follow: Following pointer is null, reinitializing" << endl;
        followerNode->data.following = new FollowList();
    }
    
    cout << "follow: Checking if already following" << endl;
    cout << "follow: About to call findFollowing..." << endl;
    
    // Use a try-catch to isolate the crash
    try {
        User* existingFollow = followerNode->data.following->findFollowing(followeeID);
        if (existingFollow) {
            cout << "follow: Already following" << endl;
            return false;  // Already following
        }
    } catch (...) {
        cout << "follow: Exception caught in findFollowing!" << endl;
        return false;
    }
    
    cout << "follow: Adding following relationship" << endl;
    try {
        followerNode->data.following->addFollowing(&followeeNode->data);
        cout << "follow: Successfully added following relationship" << endl;
        return true;
    } catch (...) {
        cout << "follow: Exception caught in addFollowing!" << endl;
        return false;
    }
}

bool UserManager::unfollow(int followerID, int followeeID) {
    LinkedList<User>::Node* followerNode = findUserByID(followerID);
    if (!followerNode || !followerNode->data.following) {
        return false;
    }
    
    return followerNode->data.following->removeFollowing(followeeID);
}

bool UserManager::isFollowing(int followerID, int followeeID) const {
    LinkedList<User>::Node* followerNode = const_cast<UserManager*>(this)->findUserByID(followerID);
    if (!followerNode || !followerNode->data.following) {
        return false;
    }
    
    return followerNode->data.following->findFollowing(followeeID) != nullptr;
}

bool UserManager::addPost(int userID, Post* post) {
    LinkedList<User>::Node* userNode = findUserByID(userID);
    if (!userNode || !post) {
        return false;
    }
    
    // Use the User's addPost method instead of directly manipulating PostList internals
    userNode->data.addPost(post->postID, post->category);
    return true;
}

bool UserManager::deletePost(int userID, PostID postID) {
    LinkedList<User>::Node* userNode = findUserByID(userID);
    if (!userNode) {
        return false;
    }
    
    return userNode->data.posts.removePost(postID);
}

LinkedList<User>::Node* UserManager::findUserByID(int userID) {
    return users.find([userID](const User& user) {
        return user.userID == userID;
    });
}

LinkedList<User>::Node* UserManager::findUserByName(const string& username) {
    return users.find([&username](const User& user) {
        return user.userName == username;
    });
}

void UserManager::exportUsersCSV(const string& path) const {
    ofstream file(path);
    if (!file.is_open()) {
        return;
    }
    
    // CSV format: userID,username,followee1|followee2|...,postID1:category1:views1|postID2:category2:views2|...
    LinkedList<User>::Node* current = users.head();
    while (current) {
        const User& user = current->data;
        
        // Write userID and username
        file << user.userID << "," << user.userName << ",";
        
        // Write following list - check if following exists
        if (user.following) {
            FollowNode* followNode = user.following->head;
            bool first_follow = true;
            while (followNode) {
                if (!first_follow) file << "|";
                if (followNode->user) {
                    file << followNode->user->userID;
                }
                followNode = followNode->next;
                first_follow = false;
            }
        }
        file << ",";
        
        // Write posts
        PostNode* postNode = user.posts.head;
        bool first_post = true;
        while (postNode) {
            if (!first_post) file << "|";
            if (postNode->post) {
                file << postNode->post->postID << ":" 
                     << postNode->post->category << ":" 
                     << postNode->post->views;
            }
            postNode = postNode->next;
            first_post = false;
        }
        file << endl;
        
        current = current->next;
    }
    
    file.close();
}

void UserManager::importUsersCSV(const string& path) {
    ifstream file(path);
    if (!file.is_open()) {
        return;
    }
    
    // Clear existing users
    users.clear();
    
    string line;
    while (getline(file, line)) {
        stringstream ss(line);
        string userID_str, username, follows_str, posts_str;
        
        getline(ss, userID_str, ',');
        getline(ss, username, ',');
        getline(ss, follows_str, ',');
        getline(ss, posts_str, ',');
        
        // Create user
        if (!userID_str.empty()) {
            int userID = stoi(userID_str);
            LinkedList<User>::Node* userNode = createUser(userID, username);
            if (!userNode) continue;
            
            // Parse and add posts (do this first, before follows)
            if (!posts_str.empty()) {
                stringstream posts_ss(posts_str);
                string post_entry;
                while (getline(posts_ss, post_entry, '|')) {
                    stringstream post_ss(post_entry);
                    string postID_str, category, views_str;
                    
                    getline(post_ss, postID_str, ':');
                    getline(post_ss, category, ':');
                    getline(post_ss, views_str, ':');
                    
                    if (!postID_str.empty()) {
                        int postID = stoi(postID_str);
                        userNode->data.addPost(postID, category);
                    }
                }
            }
        }
    }
    
    // Second pass: handle follows (now that all users exist)
    file.clear();
    file.seekg(0);
    while (getline(file, line)) {
        stringstream ss(line);
        string userID_str, username, follows_str, posts_str;
        
        getline(ss, userID_str, ',');
        getline(ss, username, ',');
        getline(ss, follows_str, ',');
        getline(ss, posts_str, ',');
        
        if (!userID_str.empty()) {
            int userID = stoi(userID_str);
            
            // Parse and add follows
            if (!follows_str.empty()) {
                stringstream follows_ss(follows_str);
                string followee_id_str;
                while (getline(follows_ss, followee_id_str, '|')) {
                    if (!followee_id_str.empty()) {
                        int followeeID = stoi(followee_id_str);
                        follow(userID, followeeID);
                    }
                }
            }
        }
    }
    
    file.close();
}

void UserManager::dumpAllUsers(ostream& out) const {
    LinkedList<User>::Node* current = users.head();
    while (current) {
        const User& user = current->data;
        out << "User ID: " << user.userID << ", Name: " << user.userName << endl;
        user.displayFollowing();
        user.posts.displayPosts();
        out << "---" << endl;
        current = current->next;
    }
}