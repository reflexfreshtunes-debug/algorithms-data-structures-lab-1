#include "common/c_string.hpp"
#include "common/priority_queue.hpp"
#include <iostream>
#include <stdexcept>

class LeftistPriorityQueue : public PriorityQueue {
private:
    struct Node {
        int key; char* value; Node* left; Node* right; int rank;
        Node(const char* text, int priority) : key(priority), value(copy_c_string(text)), left(nullptr), right(nullptr), rank(1) {}
    };
    Node* root_;

    static int rank(Node* node) { return node == nullptr ? 0 : node->rank; }
    static Node* meld(Node* a, Node* b) {
        if (a == nullptr) return b;
        if (b == nullptr) return a;
        if (b->key < a->key) { Node* temp = a; a = b; b = temp; }
        a->right = meld(a->right, b);
        if (rank(a->left) < rank(a->right)) { Node* temp = a->left; a->left = a->right; a->right = temp; }
        a->rank = rank(a->right) + 1;
        return a;
    }
    static void destroy(Node* node) noexcept { if (node) { destroy(node->left); destroy(node->right); delete[] node->value; delete node; } }
    static Node* clone(const Node* node) {
        if (!node) return nullptr;
        Node* copy = new Node(node->value, node->key);
        try { copy->rank = node->rank; copy->left = clone(node->left); copy->right = clone(node->right); }
        catch (...) { destroy(copy); throw; }
        return copy;
    }
    void swap(LeftistPriorityQueue& other) noexcept { Node* temp = root_; root_ = other.root_; other.root_ = temp; }

public:
    LeftistPriorityQueue() : root_(nullptr) {}
    LeftistPriorityQueue(const LeftistPriorityQueue& other) : root_(clone(other.root_)) {}
    LeftistPriorityQueue(LeftistPriorityQueue&& other) noexcept : root_(other.root_) { other.root_ = nullptr; }
    LeftistPriorityQueue& operator=(const LeftistPriorityQueue& other) { if (this != &other) { LeftistPriorityQueue copy(other); swap(copy); } return *this; }
    LeftistPriorityQueue& operator=(LeftistPriorityQueue&& other) noexcept { if (this != &other) { destroy(root_); root_ = other.root_; other.root_ = nullptr; } return *this; }
    ~LeftistPriorityQueue() override { destroy(root_); }
    void push(const char* value, int key) override { Node* node = new Node(value, key); root_ = meld(root_, node); }
    const char* top() const override { if (empty()) throw std::underflow_error("Queue is empty"); return root_->value; }
    void pop() override { if (empty()) throw std::underflow_error("Queue is empty"); Node* old = root_; root_ = meld(root_->left, root_->right); delete[] old->value; delete old; }
    bool empty() const override { return root_ == nullptr; }
    PriorityQueue& merge(PriorityQueue& other) override {
        LeftistPriorityQueue* queue = dynamic_cast<LeftistPriorityQueue*>(&other);
        if (!queue) throw std::invalid_argument("Queue types must match");
        if (queue != this) { root_ = meld(root_, queue->root_); queue->root_ = nullptr; }
        return *this;
    }
};

int main() {
    LeftistPriorityQueue a, b;
    a.push("third", 3); a.push("first", 1); b.push("second", 2); a.merge(b);
    while (!a.empty()) { std::cout << a.top() << '\n'; a.pop(); }
    return 0;
}

