#include "SynchronizedQueue.hpp"
#include <mutex>

template<typename T>
void SynchronizedQueue<T>::push(T value) {
    std::lock_guard<std::mutex> lock(mutex);
    queue.push(std::move(value));
    cv.notify_one();
}

template<typename T>
std::optional<T> SynchronizedQueue<T>::pop() {
    std::unique_lock<std::mutex> lock(mutex);
    cv.wait(lock, [this] { return !queue.empty(); });
    if (queue.empty()) {
        return std::nullopt;
    }
    T value = std::move(queue.front());
    queue.pop();
    return value;
}

template<typename T>
bool SynchronizedQueue<T>::empty() const {
    std::lock_guard<std::mutex> lock(mutex);
    return queue.empty();
}

template<typename T>
size_t SynchronizedQueue<T>::size() const {
    std::lock_guard<std::mutex> lock(mutex);
    return queue.size();
}

// Explicit template instantiation for common types
template class SynchronizedQueue<int>;
template class SynchronizedQueue<double>;
template class SynchronizedQueue<std::string>;
