#include "common/c_string.hpp"
#include "common/priority_queue.hpp"
#include <cstddef>
#include <iostream>
#include <stdexcept>

class FibonacciPriorityQueue : public PriorityQueue {
private:
    struct Node {
        int key; char* value; int degree; bool mark;
        Node* parent; Node* child; Node* left; Node* right;
        Node(const char* text, int priority)
            : key(priority), value(copy_c_string(text)), degree(0), mark(false),
              parent(nullptr), child(nullptr), left(this), right(this) {}
    };

    Node* minimum_;
    std::size_t count_;

    static void insert_into_ring(Node*& ring, Node* node) {
        if (!ring) { ring = node; node->left = node->right = node; return; }
        node->right = ring->right;
        node->left = ring;
        ring->right->left = node;
        ring->right = node;
    }

    static void remove_from_ring(Node*& ring, Node* node) {
        if (node->right == node) ring = nullptr;
        else {
            node->left->right = node->right;
            node->right->left = node->left;
            if (ring == node) ring = node->right;
        }
        node->left = node->right = node;
    }

    void add_root(Node* node) {
        node->parent = nullptr;
        insert_into_ring(minimum_, node);
        if (node->key < minimum_->key) minimum_ = node;
    }

    static void destroy_ring(Node* start) noexcept {
        if (!start) return;
        Node* node = start;
        do {
            Node* next = node->right;
            destroy_ring(node->child);
            delete[] node->value;
            delete node;
            node = next;
        } while (node != start);
    }

    void copy_ring_values(const Node* start) {
        if (!start) return;
        const Node* node = start;
        do {
            push(node->value, node->key);
            copy_ring_values(node->child);
            node = node->right;
        } while (node != start);
    }

    void link_as_child(Node* child, Node* parent) {
        remove_from_ring(minimum_, child);
        child->parent = parent;
        child->mark = false;
        insert_into_ring(parent->child, child);
        ++parent->degree;
    }

    void consolidate() {
        if (!minimum_) return;
        std::size_t root_count = 0;
        Node* node = minimum_;
        do { ++root_count; node = node->right; } while (node != minimum_);

        Node** roots = new Node*[root_count];
        node = minimum_;
        for (std::size_t i = 0; i < root_count; ++i) { roots[i] = node; node = node->right; }
        Node** degrees = nullptr;
        try { degrees = new Node*[count_ + 1](); }
        catch (...) { delete[] roots; throw; }

        for (std::size_t i = 0; i < root_count; ++i) {
            Node* current = roots[i];
            if (current->parent) continue;
            int degree = current->degree;
            while (degrees[degree]) {
                Node* other = degrees[degree];
                if (other->key < current->key) { Node* temp = current; current = other; other = temp; }
                link_as_child(other, current);
                degrees[degree] = nullptr;
                ++degree;
            }
            degrees[degree] = current;
        }

        minimum_ = nullptr;
        for (std::size_t i = 0; i <= count_; ++i) {
            if (degrees[i]) {
                degrees[i]->left = degrees[i]->right = degrees[i];
                add_root(degrees[i]);
            }
        }
        delete[] degrees;
        delete[] roots;
    }

    void swap(FibonacciPriorityQueue& other) noexcept {
        Node* minimum = minimum_; minimum_ = other.minimum_; other.minimum_ = minimum;
        std::size_t count = count_; count_ = other.count_; other.count_ = count;
    }

public:
    FibonacciPriorityQueue() : minimum_(nullptr), count_(0) {}
    FibonacciPriorityQueue(const FibonacciPriorityQueue& other) : minimum_(nullptr), count_(0) {
        try { copy_ring_values(other.minimum_); }
        catch (...) { destroy_ring(minimum_); minimum_ = nullptr; count_ = 0; throw; }
    }
    FibonacciPriorityQueue(FibonacciPriorityQueue&& other) noexcept : minimum_(other.minimum_), count_(other.count_) {
        other.minimum_ = nullptr; other.count_ = 0;
    }
    FibonacciPriorityQueue& operator=(const FibonacciPriorityQueue& other) {
        if (this != &other) { FibonacciPriorityQueue copy(other); swap(copy); }
        return *this;
    }
    FibonacciPriorityQueue& operator=(FibonacciPriorityQueue&& other) noexcept {
        if (this != &other) { destroy_ring(minimum_); minimum_ = other.minimum_; count_ = other.count_; other.minimum_ = nullptr; other.count_ = 0; }
        return *this;
    }
    ~FibonacciPriorityQueue() override { destroy_ring(minimum_); }

    void push(const char* value, int key) override { add_root(new Node(value, key)); ++count_; }
    const char* top() const override { if (empty()) throw std::underflow_error("Queue is empty"); return minimum_->value; }

    void pop() override {
        if (empty()) throw std::underflow_error("Queue is empty");
        Node* removed = minimum_;
        const int children = removed->degree;
        for (int i = 0; i < children; ++i) {
            Node* child = removed->child;
            remove_from_ring(removed->child, child);
            child->parent = nullptr;
            add_root(child);
        }
        remove_from_ring(minimum_, removed);
        delete[] removed->value;
        delete removed;
        --count_;
        if (minimum_) consolidate();
    }

    bool empty() const override { return minimum_ == nullptr; }

    PriorityQueue& merge(PriorityQueue& other) override {
        FibonacciPriorityQueue* queue = dynamic_cast<FibonacciPriorityQueue*>(&other);
        if (!queue) throw std::invalid_argument("Queue types must match");
        if (queue == this || !queue->minimum_) return *this;
        if (!minimum_) {
            minimum_ = queue->minimum_;
        } else {
            Node* a_right = minimum_->right;
            Node* b_left = queue->minimum_->left;
            minimum_->right = queue->minimum_;
            queue->minimum_->left = minimum_;
            a_right->left = b_left;
            b_left->right = a_right;
            if (queue->minimum_->key < minimum_->key) minimum_ = queue->minimum_;
        }
        count_ += queue->count_;
        queue->minimum_ = nullptr;
        queue->count_ = 0;
        return *this;
    }
};

int main() {
    FibonacciPriorityQueue a, b;
    a.push("third", 3); a.push("first", 1); b.push("second", 2); a.merge(b);
    while (!a.empty()) { std::cout << a.top() << '\n'; a.pop(); }
    return 0;
}
