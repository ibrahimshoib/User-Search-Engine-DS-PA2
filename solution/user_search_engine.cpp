#include "../headers/user_search_engine.h"
#include "../headers/user_manager.h"
#include <algorithm>
#include <iostream>
using namespace std;

UserSearchEngine::UserSearchEngine() 
    : usersByID([](const int& a, const int& b) { return a < b; }),
      usersByName([](const string& a, const string& b) { return a < b; }) {
}

UserSearchEngine::~UserSearchEngine() {
}

void UserSearchEngine::migrateFromLinkedList(const LinkedList<User>& userList) {
}

bool UserSearchEngine::addUser(User* user) {
}

bool UserSearchEngine::removeUser(int userID) {
}

bool UserSearchEngine::removeUser(const string& username) {
}

User* UserSearchEngine::searchByID(int userID) const {
}

User* UserSearchEngine::searchByUsername(const std::string& username) const {
}

std::vector<User*> UserSearchEngine::searchByUsernamePrefix(const string& prefix) const {
}

std::vector<User*> UserSearchEngine::getUsersInIDRange(int minID, int maxID) const {
}

std::vector<User*> UserSearchEngine::fuzzyUsernameSearch(const string& username, int maxEditDistance) const {
}

int UserSearchEngine::calculateEditDistance(const string& str1, const string& str2) const {
}

vector<User*> UserSearchEngine::getAllUsersSorted(bool byID) const {
}

size_t UserSearchEngine::getTotalUsers() const {
}

void UserSearchEngine::displaySearchStats() const {
}

bool UserSearchEngine::isConsistent() const {
}