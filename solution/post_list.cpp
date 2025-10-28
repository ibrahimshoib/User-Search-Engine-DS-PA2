#include "../headers/post_list.h"
#include <iostream>

PostList::~PostList() {
    while (head) {
        PostNode* temp = head;
        head = head->next;
        delete temp;
    }
}

void PostList::addPost(const Post& p) {
    // Create a new Post on heap and copy data
    Post* newPost = new Post(p.postID, p.category, p.views);
    PostNode* newNode = new PostNode(newPost);
    newNode->next = head;
    head = newNode;
}

bool PostList::removePost(int postID) {
    PostNode* current = head;
    PostNode* prev = nullptr;
    
    while (current) {
        if (current->post && current->post->postID == postID) {
            if (prev) {
                prev->next = current->next;
            } else {
                head = current->next;
            }
            delete current->post;  // Free the Post object
            delete current;        // Free the PostNode
            return true;
        }
        prev = current;
        current = current->next;
    }
    return false;
}

Post* PostList::findPost(int postID) {
    PostNode* current = head;
    while (current) {
        if (current->post && current->post->postID == postID) {
            return current->post;
        }
        current = current->next;
    }
    return nullptr;
}

void PostList::displayPosts() const {
    PostNode* current = head;
    std::cout << "Posts: ";
    while (current) {
        if (current->post) {
            std::cout << "[ID: " << current->post->postID 
                      << ", Category: " << current->post->category 
                      << ", Views: " << current->post->views << "]";
            if (current->next) std::cout << ", ";
        }
        current = current->next;
    }
    std::cout << std::endl;
}

bool PostList::isEmpty() const {
    return head == nullptr;
}