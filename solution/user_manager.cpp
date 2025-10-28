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

LinkedList<User>::Node *UserManager::createUser(int userID, const string &username)
{
}

bool UserManager::deleteUser(int userID)
{
}

bool UserManager::follow(int followerID, int followeeID)
{
}

bool UserManager::unfollow(int followerID, int followeeID)
{
}

bool UserManager::isFollowing(int followerID, int followeeID) const
{
}

bool UserManager::addPost(int userID, Post *post)
{
}

bool UserManager::deletePost(int userID, PostID postID)
{
}

LinkedList<User>::Node *UserManager::findUserByID(int userID)
{
}

LinkedList<User>::Node *UserManager::findUserByName(const string &username)
{
}

void UserManager::exportUsersCSV(const string &path) const
{
}

void UserManager::importUsersCSV(const string &path)
{
}

void UserManager::dumpAllUsers(ostream &out) const
{
}