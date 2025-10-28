#include "../headers/post_pool.h"
using namespace std;

PostPool::PostPool(size_t block_size) 
    : block_size(block_size), current_block_index(0), alloc_count(0), reuse_count(0) {
}

PostPool::~PostPool() {
}

Post* PostPool::allocPost() {
}

void PostPool::freePost(Post* p) {
}

size_t PostPool::totalAllocations() const {
}

size_t PostPool::reuseCount() const {
}

void PostPool::purge() {
}

void PostPool::allocateBlock() {
}