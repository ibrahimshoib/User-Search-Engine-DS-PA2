#include "../headers/user.h"
#include "../headers/follow_list.h"
#include <iostream>
using namespace std;

User::User(int id, const string& name) 
    : userID(id), userName(name), following(new FollowList()) {
}

// Copy constructor
User::User(const User& other) 
    : userID(other.userID), userName(other.userName), posts(other.posts), following(new FollowList()) {
    // Don't copy the following relationships - they should be rebuilt separately
    // This prevents circular dependency issues and dangling pointers
}

// Copy assignment operator
User& User::operator=(const User& other) {
    if (this != &other) {
        userID = other.userID;
        userName = other.userName;
        posts = other.posts;
        
        // Clean up old following list and create new one
        delete following;
        following = new FollowList();
    }
    return *this;
}

// Move constructor
User::User(User&& other) noexcept 
    : userID(other.userID), userName(move(other.userName)), 
      posts(move(other.posts)), following(other.following) {
    other.following = nullptr; // Transfer ownership
}

// Move assignment operator
User& User::operator=(User&& other) noexcept {
    if (this != &other) {
        userID = other.userID;
        userName = move(other.userName);
        posts = move(other.posts);
        
        delete following;
        following = other.following;
        other.following = nullptr;
    }
    return *this;
}

User::~User() {
    delete following;
}

void User::addPost(int postID, const string& category) {
    Post newPost(postID, category, 0);  // Initialize views to 0
    posts.addPost(newPost);
}

void User::followUser(User* otherUser) {
    if (!otherUser || otherUser->userID == this->userID) return;  // Can't follow self
    following->addFollowing(otherUser);
}

void User::displayFollowing() const {
    cout << userName << " is following: ";
    if (following) {
        following->displayFollowing();
    } else {
        cout << "No following list!" << endl;
    }
}