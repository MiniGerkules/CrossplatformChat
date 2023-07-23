#pragma once

#include <queue>
#include <mutex>
#include <optional>

template <typename T>
class TSQueue {
    std::mutex mutex_;
    std::queue<T> queue_;
    std::condition_variable cv_;
    
public:
    TSQueue() = default;
    TSQueue(const TSQueue &other) = delete;
    
public:
    void push(T elem) {
        std::unique_lock lock{ mutex_ };
        queue_.push(std::move(elem));
        
        lock.unlock();
        cv_.notify_one();
    }
    
    std::optional<T> pop() {
        std::lock_guard lock{ mutex_ };
        
        if (queue_.empty()) {
            return std::nullopt;
        } else {
            T elem = queue_.front();
            queue_.pop();
            
            return elem;
        }
    }
    
    void wait() {
        std::unique_lock lock{ mutex_ };
        cv_.wait(lock, [this] { return !queue_.empty(); });
    }
};
