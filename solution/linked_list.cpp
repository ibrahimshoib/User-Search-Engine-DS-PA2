#include "../headers/linked_list.h"
#include "../headers/user.h"
using namespace std;

template <typename T>
LinkedList<T>::LinkedList() : _head(nullptr), _tail(nullptr), _size(0) {}

template <typename T>
LinkedList<T>::~LinkedList()
{
}

template <typename T>
typename LinkedList<T>::Node *LinkedList<T>::push_back(const T &val)
{
}

template <typename T>
typename LinkedList<T>::Node *LinkedList<T>::push_front(const T &val)
{
}

template <typename T>
void LinkedList<T>::insert_after(Node *pos, const T &val)
{
}

template <typename T>
void LinkedList<T>::remove(Node *node)
{
}

template <typename T>
typename LinkedList<T>::Node *LinkedList<T>::find(function<bool(const T &)> pred)
{
}

template <typename T>
typename LinkedList<T>::Node *LinkedList<T>::head() const
{
}

template <typename T>
typename LinkedList<T>::Node *LinkedList<T>::tail() const
{
}

template <typename T>
size_t LinkedList<T>::size() const
{
}

template <typename T>
void LinkedList<T>::clear()
{
}

template class LinkedList<int>;
template class LinkedList<string>;

struct User;
template class LinkedList<User>;