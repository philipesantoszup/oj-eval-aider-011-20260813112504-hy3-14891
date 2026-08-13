#ifndef SJTU_PRIORITY_QUEUE_HPP
#define SJTU_PRIORITY_QUEUE_HPP

#include <cstddef>
#include <functional>
#include "exceptions.hpp"

namespace sjtu {
/**
 * @brief a container like std::priority_queue which is a heap internal.
 * **Exception Safety**: The `Compare` operation might throw exceptions for certain data.
 * In such cases, any ongoing operation should be terminated, and the priority queue should be restored to its original state before the operation began.
 */
template<typename T, class Compare = std::less<T>>
class priority_queue {
private:
    struct Node {
        T data;
        Node *left;
        Node *right;
        size_t npl;
        Node(const T &d) : data(d), left(nullptr), right(nullptr), npl(0) {}
    };

    Node *root;
    size_t sz;
    Compare comp;

    static Node* clone(Node *node) {
        if (!node) return nullptr;
        Node *n = new Node(node->data);
        n->left = clone(node->left);
        n->right = clone(node->right);
        n->npl = node->npl;
        return n;
    }

    static void destroy(Node *node) {
        if (!node) return;
        destroy(node->left);
        destroy(node->right);
        delete node;
    }

    static Node* merge_trees(Node *a, Node *b, Compare &cmp) {
        if (!a) return b;
        if (!b) return a;
        if (cmp(b->data, a->data)) {
            Node *tmp = a; a = b; b = tmp;
        }
        a->right = merge_trees(a->right, b, cmp);
        if (a->left == nullptr || (a->right && a->left->npl < a->right->npl)) {
            Node *tmp = a->left; a->left = a->right; a->right = tmp;
        }
        a->npl = (a->right ? a->right->npl : 0) + 1;
        return a;
    }

public:
    /**
     * @brief default constructor
     */
    priority_queue() : root(nullptr), sz(0) {}

    /**
     * @brief copy constructor
     * @param other the priority_queue to be copied
     */
    priority_queue(const priority_queue &other) : root(nullptr), sz(0), comp(other.comp) {
        root = clone(other.root);
        sz = other.sz;
    }

    /**
     * @brief deconstructor
     */
    ~priority_queue() {
        destroy(root);
    }

    /**
     * @brief Assignment operator
     * @param other the priority_queue to be assigned from
     * @return a reference to this priority_queue after assignment
     */
    priority_queue &operator=(const priority_queue &other) {
        if (this == &other) return *this;
        Node *new_root = clone(other.root);
        destroy(root);
        root = new_root;
        sz = other.sz;
        comp = other.comp;
        return *this;
    }

    /**
     * @brief get the top element of the priority queue.
     * @return a reference of the top element.
     * @throws container_is_empty if empty() returns true
     */
    const T & top() const {
        if (empty()) throw container_is_empty();
        return root->data;
    }

    /**
     * @brief push new element to the priority queue.
     * @param e the element to be pushed
     */
    void push(const T &e) {
        Node *n = new Node(e);
        try {
            root = merge_trees(root, n, comp);
            ++sz;
        } catch (...) {
            delete n;
            throw runtime_error();
        }
    }

    /**
     * @brief delete the top element from the priority queue.
     * @throws container_is_empty if empty() returns true
     */
    void pop() {
        if (empty()) throw container_is_empty();
        Node *old = root;
        try {
            root = merge_trees(old->left, old->right, comp);
        } catch (...) {
            throw runtime_error();
        }
        delete old;
        --sz;
    }

    /**
     * @brief return the number of elements in the priority queue.
     * @return the number of elements.
     */
    size_t size() const {
        return sz;
    }

    /**
     * @brief check if the container is empty.
     * @return true if it is empty, false otherwise.
     */
    bool empty() const {
        return root == nullptr;
    }

    /**
     * @brief merge another priority_queue into this one.
     * The other priority_queue will be cleared after merging.
     * The complexity is at most O(logn).
     * @param other the priority_queue to be merged.
     */
    void merge(priority_queue &other) {
        if (this == &other) return;
        try {
            root = merge_trees(root, other.root, comp);
            sz += other.sz;
            other.root = nullptr;
            other.sz = 0;
        } catch (...) {
            throw runtime_error();
        }
    }
};

}

#endif
