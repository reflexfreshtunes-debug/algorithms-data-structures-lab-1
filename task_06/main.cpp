#include "common/c_string.hpp"
#include "common/priority_queue.hpp"
#include <cstddef>
#include <iostream>
#include <stdexcept>

class BinaryPriorityQueue : public PriorityQueue {
private:
    struct Item { int key; char* value; Item() : key(0), value(nullptr) {} };
    Item* items_;
    std::size_t size_;
    std::size_t capacity_;

    void swap_items(std::size_t a, std::size_t b) {
        Item temp = items_[a]; items_[a] = items_[b]; items_[b] = temp;
    }

    void reserve(std::size_t capacity) {
        if (capacity <= capacity_) return;
        Item* next = new Item[capacity];
        for (std::size_t i = 0; i < size_; ++i) {
            next[i] = items_[i];
            items_[i].value = nullptr;
        }
        delete[] items_;
        items_ = next;
        capacity_ = capacity;
    }

    void clear() noexcept {
        for (std::size_t i = 0; i < size_; ++i) delete[] items_[i].value;
        delete[] items_;
        items_ = nullptr; size_ = 0; capacity_ = 0;
    }

    void swap(BinaryPriorityQueue& other) noexcept {
        Item* items = items_; items_ = other.items_; other.items_ = items;
        std::size_t size = size_; size_ = other.size_; other.size_ = size;
        std::size_t capacity = capacity_; capacity_ = other.capacity_; other.capacity_ = capacity;
    }

public:
    BinaryPriorityQueue() : items_(nullptr), size_(0), capacity_(0) {}
    BinaryPriorityQueue(const BinaryPriorityQueue& other) : items_(nullptr), size_(0), capacity_(0) {
        reserve(other.capacity_);
        try {
            for (; size_ < other.size_; ++size_) {
                items_[size_].key = other.items_[size_].key;
                items_[size_].value = copy_c_string(other.items_[size_].value);
            }
        } catch (...) { clear(); throw; }
    }
    BinaryPriorityQueue(BinaryPriorityQueue&& other) noexcept : items_(other.items_), size_(other.size_), capacity_(other.capacity_) {
        other.items_ = nullptr; other.size_ = 0; other.capacity_ = 0;
    }
    BinaryPriorityQueue& operator=(const BinaryPriorityQueue& other) {
        if (this != &other) { BinaryPriorityQueue copy(other); swap(copy); }
        return *this;
    }
    BinaryPriorityQueue& operator=(BinaryPriorityQueue&& other) noexcept {
        if (this != &other) { clear(); items_ = other.items_; size_ = other.size_; capacity_ = other.capacity_; other.items_ = nullptr; other.size_ = 0; other.capacity_ = 0; }
        return *this;
    }
    ~BinaryPriorityQueue() override { clear(); }

    void push(const char* value, int key) override {
        char* copy = copy_c_string(value);
        try { if (size_ == capacity_) reserve(capacity_ == 0 ? 4 : capacity_ * 2); }
        catch (...) { delete[] copy; throw; }
        std::size_t index = size_++;
        items_[index].key = key; items_[index].value = copy;
        while (index > 0) {
            const std::size_t parent = (index - 1) / 2;
            if (items_[parent].key <= items_[index].key) break;
            swap_items(parent, index); index = parent;
        }
    }

    const char* top() const override {
        if (empty()) throw std::underflow_error("Queue is empty");
        return items_[0].value;
    }

    void pop() override {
        if (empty()) throw std::underflow_error("Queue is empty");
        delete[] items_[0].value;
        --size_;
        if (size_ == 0) { items_[0].value = nullptr; return; }
        items_[0] = items_[size_]; items_[size_].value = nullptr;
        std::size_t index = 0;
        while (true) {
            std::size_t smallest = index;
            const std::size_t left = index * 2 + 1;
            const std::size_t right = left + 1;
            if (left < size_ && items_[left].key < items_[smallest].key) smallest = left;
            if (right < size_ && items_[right].key < items_[smallest].key) smallest = right;
            if (smallest == index) break;
            swap_items(index, smallest); index = smallest;
        }
    }

    bool empty() const override { return size_ == 0; }

    PriorityQueue& merge(PriorityQueue& other) override {
        BinaryPriorityQueue* queue = dynamic_cast<BinaryPriorityQueue*>(&other);
        if (queue == nullptr) throw std::invalid_argument("Queue types must match");
        if (queue == this) return *this;
        for (std::size_t i = 0; i < queue->size_; ++i) push(queue->items_[i].value, queue->items_[i].key);
        queue->clear();
        return *this;
    }
};

int main() {
    BinaryPriorityQueue first;
    BinaryPriorityQueue second;
    first.push("low", 10); first.push("high", 1);
    second.push("middle", 5);
    first.merge(second);
    while (!first.empty()) { std::cout << first.top() << '\n'; first.pop(); }
    return 0;
}

