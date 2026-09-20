#include "common/c_string.hpp"
#include "common/priority_queue.hpp"
#include <iostream>
#include <stdexcept>

class SkewPriorityQueue : public PriorityQueue {
private:
    struct Node {
        int key; char* value; Node* left; Node* right;
        Node(const char* text, int priority) : key(priority), value(copy_c_string(text)), left(nullptr), right(nullptr) {}
    };
    Node* root_;

    static Node* meld(Node* a, Node* b) {
        if (!a) return b;
        if (!b) return a;
        if (b->key < a->key) { Node* temp = a; a = b; b = temp; }
        Node* old_left = a->left;
        a->left = meld(a->right, b);
        a->right = old_left;
        return a;
    }
    static void destroy(Node* node) noexcept { if (node) { destroy(node->left); destroy(node->right); delete[] node->value; delete node; } }
    static Node* clone(const Node* node) {
        if (!node) return nullptr;
        Node* copy = new Node(node->value, node->key);
        try { copy->left = clone(node->left); copy->right = clone(node->right); }
        catch (...) { destroy(copy); throw; }
        return copy;
    }
    void swap(SkewPriorityQueue& other) noexcept { Node* temp = root_; root_ = other.root_; other.root_ = temp; }

public:
    SkewPriorityQueue() : root_(nullptr) {}
    SkewPriorityQueue(const SkewPriorityQueue& other) : root_(clone(other.root_)) {}
    SkewPriorityQueue(SkewPriorityQueue&& other) noexcept : root_(other.root_) { other.root_ = nullptr; }
    SkewPriorityQueue& operator=(const SkewPriorityQueue& other) { if (this != &other) { SkewPriorityQueue copy(other); swap(copy); } return *this; }
    SkewPriorityQueue& operator=(SkewPriorityQueue&& other) noexcept { if (this != &other) { destroy(root_); root_ = other.root_; other.root_ = nullptr; } return *this; }
    ~SkewPriorityQueue() override { destroy(root_); }
    void push(const char* value, int key) override { root_ = meld(root_, new Node(value, key)); }
    const char* top() const override { if (empty()) throw std::underflow_error("Queue is empty"); return root_->value; }
    void pop() override { if (empty()) throw std::underflow_error("Queue is empty"); Node* old = root_; root_ = meld(root_->left, root_->right); delete[] old->value; delete old; }
    bool empty() const override { return root_ == nullptr; }
    PriorityQueue& merge(PriorityQueue& other) override {
        SkewPriorityQueue* queue = dynamic_cast<SkewPriorityQueue*>(&other);
        if (!queue) throw std::invalid_argument("Queue types must match");
        if (queue != this) { root_ = meld(root_, queue->root_); queue->root_ = nullptr; }
        return *this;
    }
};

int main() {
    SkewPriorityQueue a, b;
    a.push("third", 3); a.push("first", 1); b.push("second", 2); a.merge(b);
    while (!a.empty()) { std::cout << a.top() << '\n'; a.pop(); }
    return 0;
}
