#ifndef THREAD_SAFE_QUEUE_H
#define THREAD_SAFE_QUEUE_H

#include <deque>
#include <mutex>
#include <optional>
#include <algorithm>

namespace pool
{
    /**
     * @brief A thread-safe and work-stealing queue implementation
     */
    template <typename T>
    class ThreadSafeQueue
    {
    public:
        using value_type = T;
        using size_type = typename std::deque<T>::size_type;

        // Default constructor
        ThreadSafeQueue() = default;

        /**
         * @brief Add an element to the back of the queue
         *
         * @tparam T generic type for the element being stored in the queue
         * @param value the element to be pushed to the back of the queue
         */
        void pushBack(T &&value)
        {
            std::scoped_lock lock(mutex_);
            data_.push_back(std::forward<T>(value));
        }

        /**
         * @brief Add an element to the front of the queue
         *
         * @tparam T generic type for the element being stored in the queue
         * @param value the element to be pushed to the front of the queue
         */
        void pushFront(T &&value)
        {
            std::scoped_lock lock(mutex_);
            data_.push_front(std::forward<T>(value));
        }

        /**
         * @brief Check if the queue is empty
         *
         * @return boolean to indicate if the queue is empty
         */
        [[nodiscard]] bool empty() const
        {
            std::scoped_lock lock(mutex_);
            return data_.empty();
        }

        /**
         * @brief Remove and return an element from the front of the queue
         *
         * @return the element previously stored at the front of the queue
         */
        [[nodiscard]] std::optional<T> popFront()
        {
            std::scoped_lock lock(mutex_);
            if (data_.empty())
            {
                return std::nullopt;
            }

            auto front = std::move(data_.front());
            data_.pop_front();
            return front;
        }

        /**
         * @brief Remove and return an element from the back of the queue
         *
         * @return the element previously stored at the back of the queue
         */
        [[nodiscard]] std::optional<T> popBack()
        {
            std::scoped_lock lock(mutex_);
            if (data_.empty())
            {
                return std::nullopt;
            }

            auto back = std::move(data_.back());
            data_.pop_back();
            return back;
        }

        /**
         * @brief Remove and return an element from the back of the queue (steal spot)
         *
         * @return the element previously stored at the back of the queue
         */
        [[nodiscard]] std::optional<T> steal()
        {
            std::scoped_lock lock(mutex_);
            if (data_.empty())
            {
                return std::nullopt;
            }

            auto back = std::move(data_.back());
            data_.pop_back();
            return back;
        }

        /**
         * @brief Move an item to the front of the queue
         *
         * @tparam T generic type for the element being stored in the queue
         * @param item the element to be pushed to the front of the queue
         */
        void moveToFront(const T &item)
        {
            std::scoped_lock lock(mutex_);
            auto iter = std::find(data_.begin(), data_.end(), item);

            if (iter != data_.end())
            {
                std::ignore = data_.erase(iter);
            }

            data_.push_front(item);
        }

        /**
         * @brief Copy the front item, move it to the back, and return the copied item
         *
         * @return the element that was previously at the front of the queue
         */
        [[nodiscard]] std::optional<T> copyFrontAndMoveToBack()
        {
            std::scoped_lock lock(mutex_);
            if (data_.empty())
            {
                return std::nullopt;
            }

            auto front = data_.front();
            data_.pop_front();
            data_.push_back(front);
            return front;
        }

    private:
        /**
         * @brief internal data structure to store the data
         */
        std::deque<T> data_{};

        /**
         * @brief mutex for locking
         */
        mutable std::mutex mutex_{};
    };
} // namespace pool

#endif // !THREAD_SAFE_QUEUE_H