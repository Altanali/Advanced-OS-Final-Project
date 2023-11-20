#ifndef THREAD_SAFE_QUEUE_H
#define THREAD_SAFE_QUEUE_H

#include <deque>
#include <mutex>
#include <optional>
#include <algorithm>

namespace pool
{
    template <typename T, typename Lock = std::mutex>
    class ThreadSafeQueue
    {
    public:
        using value_type = T;
        using size_type = typename std::deque<T>::size_type;

        // Default constructor
        ThreadSafeQueue();

        // Add an element to the back of the queue
        void pushBack(T &&value);

        // Add an element to the front of the queue
        void pushFront(T &&value);

        // Check if the queue is empty
        [[nodiscard]] bool empty() const;

        // Remove and return an element from the front of the queue
        [[nodiscard]] std::optional<T> popFront();

        // Remove and return an element from the back of the queue
        [[nodiscard]] std::optional<T> popBack();

        // Remove and return an element from the back of the queue (steal spot)
        [[nodiscard]] std::optional<T> steal();

        // Move an item to the front of the queue
        void moveToFront(const T &item);

        // Copy the front item, move it to the back, and return the copied item
        [[nodiscard]] std::optional<T> copyFrontAndMoveToBack();

    private:
        std::deque<T> data_{};
        mutable Lock mutex_{};
    };
}

#endif // !THREAD_SAFE_QUEUE_H