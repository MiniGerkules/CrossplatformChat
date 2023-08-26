#pragma once

#include <mutex>
#include <unordered_map>

template <typename Key, typename Value>
class TSUnorderedMap final {
    std::mutex mutex_;
    std::unordered_map<Key, Value> map_;

public:
    auto begin() const { return map_.begin(); }
    auto end() const { return map_.end(); }

    auto cbegin() const { return map_.cbegin(); }
    auto cend() const { return map_.cend(); }

public:
    Value& operator[]( const Key& key ) {
        std::lock_guard lock{ mutex_ };
        return map_[key];
    }

    auto size() {
        std::lock_guard lock{ mutex_ };
        return map_.size();
    }

    void insert(std::pair<Key, Value> pair) {
        std::lock_guard lock{ mutex_ };
        map_.insert(std::move(pair));
    }

    void erase(const Key &key) {
        std::lock_guard lock{ mutex_ };
        map_.erase(key);
    }

    void clear() {
        std::lock_guard lock{ mutex_ };
        map_.clear();
    }
};
