#pragma once

#include <queue>
#include <optional>
#include <condition_variable>


template<typename T>
class SynchronizedQueue {
private:
    std::queue<T> queue;
    mutable std::mutex mutex;
    std::condition_variable cv;

public:
    SynchronizedQueue() = default;
    SynchronizedQueue(const SynchronizedQueue&) = delete;
    SynchronizedQueue& operator=(const SynchronizedQueue&) = delete;

    void push(T value);
    std::optional<T> pop();
    bool empty() const;
    size_t size() const;
    bool try_pop(T& value);
};

