#pragma once
#include <string>
#include <vector>
#include <memory>
using namespace std;

struct Post; // Forward declaration

/**
 * Node in the category hierarchy tree
 */
class CategoryNode : public std::enable_shared_from_this<CategoryNode>
{
public:
    string categoryName;
    vector<shared_ptr<CategoryNode>> children;
    weak_ptr<CategoryNode> parent;
    vector<Post *> posts; // Posts directly in this category
    int totalPostCount;   // Posts in this category + all subcategories

    CategoryNode(const string &name);

    // Students must implement these methods
    void addChild(shared_ptr<CategoryNode> child);
    bool removeChild(const string &childName);
    shared_ptr<CategoryNode> findChild(const string &childName) const;
    void addPost(Post *post);
    bool removePost(Post *post);
    void updatePostCounts(); // Recalculate totalPostCount recursively
};

/**
 * Hierarchical category tree for organizing posts
 */
class CategoryTree
{
protected:
    shared_ptr<CategoryNode> root;

    // Helper method to parse category path (implemented for students)
    vector<string> parseCategoryPath(const string &path) const
    {
        vector<string> categories;
        string current;
        for (char c : path)
        {
            if (c == '_')
            {
                if (!current.empty())
                {
                    categories.push_back(current);
                    current.clear();
                }
            }
            else
            {
                current += c;
            }
        }
        if (!current.empty())
        {
            categories.push_back(current);
        }
        return categories;
    }

public:
    CategoryTree();

    // Students must implement these methods
    bool addCategory(const string &categoryPath);
    bool removeCategory(const string &categoryPath);
    bool moveCategory(const string &fromPath, const string &toPath);
    shared_ptr<CategoryNode> findCategory(const string &categoryPath) const;
    void addPost(Post *post);
    bool removePost(Post *post);
    vector<Post *> getPostsInCategory(const string &categoryPath, bool includeSubcategories = true) const;

    // Tree traversal methods - students must implement
    class PreOrderIterator;
    class PostOrderIterator;
    class BreadthFirstIterator;

    PreOrderIterator preOrderBegin() const;
    PreOrderIterator preOrderEnd() const;
    PostOrderIterator postOrderBegin() const;
    PostOrderIterator postOrderEnd() const;
    BreadthFirstIterator breadthFirstBegin() const;
    BreadthFirstIterator breadthFirstEnd() const;

    void displayTree() const;
    shared_ptr<CategoryNode> getRoot() const { return root; }
};

/**
 * Pre-order traversal iterator
 */
class CategoryTree::PreOrderIterator
{
private:
    vector<shared_ptr<CategoryNode>> stack;
    shared_ptr<CategoryNode> current;

public:
    PreOrderIterator(shared_ptr<CategoryNode> root);
    PreOrderIterator(); // End iterator

    // Students must implement these operators
    shared_ptr<CategoryNode> operator*() const;
    PreOrderIterator &operator++();
    bool operator!=(const PreOrderIterator &other) const;
    bool operator==(const PreOrderIterator &other) const;
};

/**
 * Post-order traversal iterator
 */
class CategoryTree::PostOrderIterator
{
private:
    vector<shared_ptr<CategoryNode>> stack;
    shared_ptr<CategoryNode> current;
    shared_ptr<CategoryNode> lastVisited;

public:
    PostOrderIterator(shared_ptr<CategoryNode> root);
    PostOrderIterator(); // End iterator

    // Students must implement these operators
    shared_ptr<CategoryNode> operator*() const;
    PostOrderIterator &operator++();
    bool operator!=(const PostOrderIterator &other) const;
    bool operator==(const PostOrderIterator &other) const;
};

/**
 * Breadth-first traversal iterator
 */
class CategoryTree::BreadthFirstIterator
{
private:
    vector<shared_ptr<CategoryNode>> queue;
    size_t currentIndex;

public:
    BreadthFirstIterator(shared_ptr<CategoryNode> root);
    BreadthFirstIterator(); // End iterator

    // Students must implement these operators
    shared_ptr<CategoryNode> operator*() const;
    BreadthFirstIterator &operator++();
    bool operator!=(const BreadthFirstIterator &other) const;
    bool operator==(const BreadthFirstIterator &other) const;
};

// #include "../solution/category_tree.cpp"