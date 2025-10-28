#include "../headers/user.h"
#include "../headers/follow_list.h"
#include <iostream>
using namespace std;

User::User(int id, const string &name)
    : userID(id), userName(name), following(new FollowList())
{
}

User::User(const User &other)
    : userID(other.userID), userName(other.userName), posts(other.posts), following(new FollowList())
{
}

User &User::operator=(const User &other)
{
}

User::User(User &&other) noexcept
    : userID(other.userID), userName(move(other.userName)),
      posts(move(other.posts)), following(other.following)
{
}

User &User::operator=(User &&other) noexcept
{
}

User::~User()
{
}

void User::addPost(int postID, const string &category)
{
}

void User::followUser(User *otherUser)
{
}

void User::displayFollowing() const
{
}