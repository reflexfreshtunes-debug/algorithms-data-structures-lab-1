#include "common/priority_queue.hpp"
#include <iostream>

int main() {
    std::cout << "PriorityQueue interface:\n"
              << "push(value, key) - add value\n"
              << "top()            - find most important value\n"
              << "pop()            - remove most important value\n"
              << "merge(other)     - merge and return *this\n";
    return 0;
}

