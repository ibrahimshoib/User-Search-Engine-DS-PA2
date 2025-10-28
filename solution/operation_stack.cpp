#include "../headers/operation_stack.h"
#include <cstddef> // size_t
using namespace std;


UndoRedoManager::UndoRedoManager(UserManager& um, PostPool& pool)
    : userManager(um), postPool(pool) {}

void UndoRedoManager::beginTransaction() {
    transactionMarkers.push(undoStack.size());
}

void UndoRedoManager::commitTransaction() {
    if (transactionMarkers.empty()) return;

    transactionMarkers.pop();

    // Free any objects in trash list
    for (void* ptr : trashList) {
        if (ptr) {
            postPool.freePost(static_cast<Post*>(ptr));
        }
    }
    trashList.clear();
}

void UndoRedoManager::rollbackTransaction() {
    if (transactionMarkers.empty()) return;

    size_t transactionStart = transactionMarkers.top();
    transactionMarkers.pop();

    // Undo all operations back to the transaction marker
    while (undoStack.size() > transactionStart) {
        OpFrame frame = undoStack.back();
        undoStack.pop_back();

        switch (frame.type) {
            case OpType::CREATE_USER:
                userManager.deleteUser(frame.userID);
                break;

            case OpType::DELETE_USER:
                userManager.createUser(frame.userID, frame.snapshot_username_or_content);
                break;

            case OpType::CREATE_POST:
                userManager.deletePost(frame.userID, frame.postID);
                break;

            case OpType::DELETE_POST: {
                Post* newPost = postPool.allocPost();
                if (newPost) {
                    newPost->postID = frame.postID;
                    size_t colonPos = frame.snapshot_username_or_content.find(':');
                    if (colonPos != string::npos) {
                        newPost->category = frame.snapshot_username_or_content.substr(0, colonPos);
                        newPost->views = stoi(frame.snapshot_username_or_content.substr(colonPos + 1));
                    }
                    userManager.addPost(frame.userID, newPost);
                }
                break;
            }

            case OpType::FOLLOW:
                userManager.unfollow(frame.userID, frame.postID);
                break;

            case OpType::UNFOLLOW:
                userManager.follow(frame.userID, frame.postID);
                break;

            case OpType::EDIT_POST:
                // Not implemented
                break;
        }
    }

    trashList.clear();
}

void UndoRedoManager::record(const OpFrame& f) {
    undoStack.push_back(f);
    redoStack.clear();
}

bool UndoRedoManager::undo() {
    if (undoStack.empty()) return false;

    OpFrame frame = undoStack.back();
    undoStack.pop_back();

    bool success = false;

    switch (frame.type) {
        case OpType::CREATE_USER:
            success = userManager.deleteUser(frame.userID);
            break;

        case OpType::DELETE_USER:
            success = (userManager.createUser(frame.userID, frame.snapshot_username_or_content) != nullptr);
            break;

        case OpType::CREATE_POST:
            success = userManager.deletePost(frame.userID, frame.postID);
            break;

        case OpType::DELETE_POST: {
            Post* newPost = postPool.allocPost();
            if (newPost) {
                newPost->postID = frame.postID;
                size_t colonPos = frame.snapshot_username_or_content.find(':');
                if (colonPos != string::npos) {
                    newPost->category = frame.snapshot_username_or_content.substr(0, colonPos);
                    newPost->views = stoi(frame.snapshot_username_or_content.substr(colonPos + 1));
                }
                success = userManager.addPost(frame.userID, newPost);
            }
            break;
        }

        case OpType::FOLLOW:
            success = userManager.unfollow(frame.userID, frame.postID);
            break;

        case OpType::UNFOLLOW:
            success = userManager.follow(frame.userID, frame.postID);
            break;

        case OpType::EDIT_POST:
            success = false;
            break;
    }

    if (success) {
        redoStack.push_back(frame);
    }

    return success;
}

bool UndoRedoManager::redo() {
    if (redoStack.empty()) return false;

    OpFrame frame = redoStack.back();
    redoStack.pop_back();

    bool success = false;

    switch (frame.type) {
        case OpType::CREATE_USER:
            success = (userManager.createUser(frame.userID, frame.snapshot_username_or_content) != nullptr);
            break;

        case OpType::DELETE_USER:
            success = userManager.deleteUser(frame.userID);
            break;

        case OpType::CREATE_POST: {
            Post* newPost = postPool.allocPost();
            if (newPost) {
                newPost->postID = frame.postID;
                size_t colonPos = frame.snapshot_username_or_content.find(':');
                if (colonPos != string::npos) {
                    newPost->category = frame.snapshot_username_or_content.substr(0, colonPos);
                    newPost->views = stoi(frame.snapshot_username_or_content.substr(colonPos + 1));
                }
                success = userManager.addPost(frame.userID, newPost);
            }
            break;
        }

        case OpType::DELETE_POST:
            success = userManager.deletePost(frame.userID, frame.postID);
            break;

        case OpType::FOLLOW:
            success = userManager.follow(frame.userID, frame.postID);
            break;

        case OpType::UNFOLLOW:
            success = userManager.unfollow(frame.userID, frame.postID);
            break;

        case OpType::EDIT_POST:
            success = false;
            break;
    }

    if (success) {
        undoStack.push_back(frame);
    }

    return success;
}
