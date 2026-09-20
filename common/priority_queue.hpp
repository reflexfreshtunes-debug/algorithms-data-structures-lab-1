#ifndef PRIORITY_QUEUE_HPP
#define PRIORITY_QUEUE_HPP

class PriorityQueue {
public:
    virtual ~PriorityQueue() {}
    virtual void push(const char* value, int key) = 0;
    virtual const char* top() const = 0;
    virtual void pop() = 0;
    virtual bool empty() const = 0;
    virtual PriorityQueue& merge(PriorityQueue& other) = 0;
};

#endif

