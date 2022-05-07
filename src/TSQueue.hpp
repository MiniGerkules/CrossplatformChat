#pragma once
#include <mutex>
#include <queue>

template <typename T>
class TSQueue {
protected:
    mutable std::mutex queueMut;
    std::queue<T> queue;

    std::condition_variable waitMsg;
    std::mutex waitMut;

public:
    TSQueue() = default;
    //TSQueue(const TSQueue&) = delete;
    //TSQueue& operator=(const TSQueue&) = delete;
    virtual ~TSQueue() { clear(); }

public:
    const T& front() const {
        std::scoped_lock lock{ queueMut };
        return queue.front();
    }

    const T& back() const {
        std::scoped_lock lock{ queueMut };
        return queue.back();
    }

    void push(const T& newItem) {
        std::scoped_lock lock{ queueMut };
        queue.push(newItem);

        std::unique_lock uniqueLock{ waitMut };
        waitMsg.notify_one();
    }

    void pop() {
        std::scoped_lock lock{ queueMut };
        queue.pop();
    }

    bool empty() const {
        std::scoped_lock lock{ queueMut };
        return queue.empty();
    }

    size_t size() const {
        std::scoped_lock lock{ queueMut };
        return queue.size();
    }

    void clear() {
        std::scoped_lock lock{ queueMut };
        while (!queue.empty()) queue.pop();
    }

    void wait() {
        std::unique_lock uniqueLock{ waitMut };
        waitMsg.wait(uniqueLock, [this]() { return !empty(); });
    }
};
