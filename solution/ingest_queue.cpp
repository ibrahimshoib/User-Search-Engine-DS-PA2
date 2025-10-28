#include "../include/ingest_queue.h"
using namespace std;

IngestQueue::IngestQueue(size_t capacity) 
    : capacity(capacity), head_idx(0), tail_idx(0), count(0) {
    buffer = new Post*[capacity];
    // Initialize buffer to nullptr for safety
    for (size_t i = 0; i < capacity; ++i) {
        buffer[i] = nullptr;
    }
}

IngestQueue::~IngestQueue() {
    delete[] buffer;
}

bool IngestQueue::enqueue(Post* p) {
    if (count >= capacity) {
        return false;  // Queue is full
    }
    
    buffer[tail_idx] = p;
    tail_idx = (tail_idx + 1) % capacity;
    count++;
    return true;
}

Post* IngestQueue::dequeue() {
    if (count == 0) {
        return nullptr;  // Queue is empty
    }
    
    Post* result = buffer[head_idx];
    buffer[head_idx] = nullptr;  // Clear for safety
    head_idx = (head_idx + 1) % capacity;
    count--;
    return result;
}

size_t IngestQueue::size() const {
    return count;
}

bool IngestQueue::empty() const {
    return count == 0;
}

size_t IngestQueue::dequeueBatch(Post** out_array, size_t max_k) {
    if (!out_array || max_k == 0) return 0;
    
    size_t items_to_dequeue = min(count, max_k);
    
    for (size_t i = 0; i < items_to_dequeue; ++i) {
        out_array[i] = buffer[head_idx];
        buffer[head_idx] = nullptr;  // Clear for safety
        head_idx = (head_idx + 1) % capacity;
        count--;
    }
    
    return items_to_dequeue;
}