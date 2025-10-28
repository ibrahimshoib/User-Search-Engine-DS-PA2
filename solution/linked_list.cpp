#include "../headers/linked_list.h"
#include "../headers/user.h"  // Need this for explicit instantiation
using namespace std;

template<typename T>
LinkedList<T>::LinkedList() : _head(nullptr), _tail(nullptr), _size(0) {}

template<typename T>
LinkedList<T>::~LinkedList() {
    clear();
}

template<typename T>
typename LinkedList<T>::Node* LinkedList<T>::push_back(const T& val) {
    Node* newNode = new Node(val);
    if (_tail) {
        _tail->next = newNode;
        newNode->prev = _tail;
        _tail = newNode;
    } else {
        _head = _tail = newNode;
    }
    _size++;
    return newNode;
}

template<typename T>
typename LinkedList<T>::Node* LinkedList<T>::push_front(const T& val) {
    Node* newNode = new Node(val);
    if (_head) {
        _head->prev = newNode;
        newNode->next = _head;
        _head = newNode;
    } else {
        _head = _tail = newNode;
    }
    _size++;
    return newNode;
}

template<typename T>
void LinkedList<T>::insert_after(Node* pos, const T& val) {
    if (!pos) return;
    
    Node* newNode = new Node(val);
    newNode->next = pos->next;
    newNode->prev = pos;
    
    if (pos->next) {
        pos->next->prev = newNode;
    } else {
        _tail = newNode;
    }
    pos->next = newNode;
    _size++;
}

template<typename T>
void LinkedList<T>::remove(Node* node) {
    if (!node) return;
    
    if (node->prev) {
        node->prev->next = node->next;
    } else {
        _head = node->next;
    }
    
    if (node->next) {
        node->next->prev = node->prev;
    } else {
        _tail = node->prev;
    }
    
    delete node;
    _size--;
}

template<typename T>
typename LinkedList<T>::Node* LinkedList<T>::find(function<bool(const T&)> pred) {
    Node* current = _head;
    while (current) {
        if (pred(current->data)) {
            return current;
        }
        current = current->next;
    }
    return nullptr;
}

template<typename T>
typename LinkedList<T>::Node* LinkedList<T>::head() const {
    return _head;
}

template<typename T>
typename LinkedList<T>::Node* LinkedList<T>::tail() const {
    return _tail;
}

template<typename T>
size_t LinkedList<T>::size() const {
    return _size;
}

template<typename T>
void LinkedList<T>::clear() {
    while (_head) {
        Node* temp = _head;
        _head = _head->next;
        delete temp;
    }
    _tail = nullptr;
    _size = 0;
}

// Explicit template instantiation for all used types
template class LinkedList<int>;
template class LinkedList<string>;
template class LinkedList<User>;