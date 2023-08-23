#pragma once

#include <memory>
#include <functional>

template <typename T>
class Delegate {
public:
    template <typename... Args>
    using Method = void (T::*)(Args...);

private:
    std::weak_ptr<T> delegate_;

public:
    Delegate(std::weak_ptr<T> delegate) : delegate_{ std::move(delegate) } {
    }

    template <typename... Args>
    void callIfCan(Method<Args...> method, Args... args) {
        if (auto delegatePtr = delegate_.lock()) {
            (delegatePtr.get()->*method)(args...);
        }
    }
};
