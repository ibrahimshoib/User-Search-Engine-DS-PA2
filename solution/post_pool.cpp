#include "../headers/post_pool.h"
using namespace std;

PostPool::PostPool(size_t block_size) 
    : block_size(block_size), current_block_index(0), alloc_count(0), reuse_count(0) {
    allocateBlock();  // Initialize with first block
}

PostPool::~PostPool() {
    purge();
}

Post* PostPool::allocPost() {
    // First check if we have any free posts to reuse
    if (!free_list.empty()) {
        Post* reusedPost = free_list.back();
        free_list.pop_back();
        reuse_count++;
        
        // Reset the post to default state
        reusedPost->postID = 0;
        reusedPost->category.clear();
        reusedPost->views = 0;
        
        return reusedPost;
    }
    
    // If no free posts, allocate from current block
    if (current_block_index >= block_size) {
        allocateBlock();  // Need a new block
    }
    
    Post* newPost = &blocks.back()[current_block_index];
    current_block_index++;
    
    // Initialize to default state
    newPost->postID = 0;
    newPost->category.clear();
    newPost->views = 0;
    
    return newPost;
}

void PostPool::freePost(Post* p) {
    if (!p) return;
    
    // Reset the post data for security/cleanliness
    p->postID = 0;
    p->category.clear();
    p->views = 0;
    
    // Add to free list for reuse
    free_list.push_back(p);
}

size_t PostPool::totalAllocations() const {
    return alloc_count;
}

size_t PostPool::reuseCount() const {
    return reuse_count;
}

void PostPool::purge() {
    // Delete all blocks
    for (Post* block : blocks) {
        delete[] block;
    }
    blocks.clear();
    free_list.clear();
    
    // Reset counters
    current_block_index = 0;
    alloc_count = 0;
    reuse_count = 0;
}

void PostPool::allocateBlock() {
    Post* newBlock = new Post[block_size];
    blocks.push_back(newBlock);
    current_block_index = 0;
    alloc_count++;
}