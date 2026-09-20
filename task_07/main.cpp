#include "common/c_string.hpp"
#include "common/priority_queue.hpp"
#include <iostream>
#include <stdexcept>

class BinomialPriorityQueue : public PriorityQueue {
private:
    struct Node {
        int key; char* value; int degree; Node* child; Node* sibling;
        Node(const char* text, int priority) : key(priority), value(copy_c_string(text)), degree(0), child(nullptr), sibling(nullptr) {}
    };
    Node* roots_;

    static void destroy(Node* node) noexcept {
        while (node) {
            Node* next = node->sibling;
            destroy(node->child);
            delete[] node->value;
            delete node;
            node = next;
        }
    }

    static Node* clone(const Node* node) {
        if (!node) return nullptr;
        Node* first = nullptr;
        Node** tail = &first;
        try {
            while (node) {
                Node* copy = new Node(node->value, node->key);
                copy->degree = node->degree;
                copy->child = clone(node->child);
                *tail = copy;
                tail = &copy->sibling;
                node = node->sibling;
            }
        } catch (...) { destroy(first); throw; }
        return first;
    }

    static Node* merge_root_lists(Node* a, Node* b) {
        Node* result = nullptr;
        Node** tail = &result;
        while (a && b) {
            if (a->degree <= b->degree) { *tail = a; a = a->sibling; }
            else { *tail = b; b = b->sibling; }
            tail = &((*tail)->sibling);
        }
        *tail = a ? a : b;
        return result;
    }

    static void make_child(Node* child, Node* parent) {
        child->sibling = parent->child;
        parent->child = child;
        ++parent->degree;
    }

    static Node* unite(Node* a, Node* b) {
        Node* head = merge_root_lists(a, b);
        if (!head) return nullptr;
        Node* previous = nullptr;
        Node* current = head;
        Node* next = current->sibling;
        while (next) {
            if (current->degree != next->degree ||
                (next->sibling && next->sibling->degree == current->degree)) {
                previous = current;
                current = next;
            } else if (current->key <= next->key) {
                current->sibling = next->sibling;
                make_child(next, current);
            } else {
                if (previous) previous->sibling = next;
                else head = next;
                make_child(current, next);
                current = next;
            }
            next = current->sibling;
        }
        return head;
    }

    void swap(BinomialPriorityQueue& other) noexcept { Node* temp = roots_; roots_ = other.roots_; other.roots_ = temp; }

public:
    BinomialPriorityQueue() : roots_(nullptr) {}
    BinomialPriorityQueue(const BinomialPriorityQueue& other) : roots_(clone(other.roots_)) {}
    BinomialPriorityQueue(BinomialPriorityQueue&& other) noexcept : roots_(other.roots_) { other.roots_ = nullptr; }
    BinomialPriorityQueue& operator=(const BinomialPriorityQueue& other) { if (this != &other) { BinomialPriorityQueue copy(other); swap(copy); } return *this; }
    BinomialPriorityQueue& operator=(BinomialPriorityQueue&& other) noexcept { if (this != &other) { destroy(roots_); roots_ = other.roots_; other.roots_ = nullptr; } return *this; }
    ~BinomialPriorityQueue() override { destroy(roots_); }

    void push(const char* value, int key) override { roots_ = unite(roots_, new Node(value, key)); }

    const char* top() const override {
        if (empty()) throw std::underflow_error("Queue is empty");
        Node* best = roots_;
        for (Node* node = roots_->sibling; node; node = node->sibling)
            if (node->key < best->key) best = node;
        return best->value;
    }

    void pop() override {
        if (empty()) throw std::underflow_error("Queue is empty");
        Node* best = roots_;
        Node* best_previous = nullptr;
        Node* previous = roots_;
        for (Node* node = roots_->sibling; node; node = node->sibling) {
            if (node->key < best->key) { best = node; best_previous = previous; }
            previous = node;
        }
        if (best_previous) best_previous->sibling = best->sibling;
        else roots_ = best->sibling;

        Node* reversed = nullptr;
        Node* child = best->child;
        while (child) {
            Node* next = child->sibling;
            child->sibling = reversed;
            reversed = child;
            child = next;
        }
        best->child = nullptr;
        best->sibling = nullptr;
        delete[] best->value;
        delete best;
        roots_ = unite(roots_, reversed);
    }

    bool empty() const override { return roots_ == nullptr; }

    PriorityQueue& merge(PriorityQueue& other) override {
        BinomialPriorityQueue* queue = dynamic_cast<BinomialPriorityQueue*>(&other);
        if (!queue) throw std::invalid_argument("Queue types must match");
        if (queue != this) { roots_ = unite(roots_, queue->roots_); queue->roots_ = nullptr; }
        return *this;
    }
};

int main() {
    BinomialPriorityQueue a, b;
    a.push("third", 3); a.push("first", 1); b.push("second", 2); a.merge(b);
    while (!a.empty()) { std::cout << a.top() << '\n'; a.pop(); }
    return 0;
}
