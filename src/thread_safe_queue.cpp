#include "thread_safe_queue.hpp"

namespace pool
{
    template <typename T, typename Lock>
    ThreadSafeQueue<T, Lock>::ThreadSafeQueue() = default;

    template <typename T, typename Lock>
    void ThreadSafeQueue<T, Lock>::pushBack(T &&value)
    {
        std::scoped_lock lock(mutex_);
        data_.push_back(std::forward<T>(value));
    }

    template <typename T, typename Lock>
    void ThreadSafeQueue<T, Lock>::pushFront(T &&value)
    {
        std::scoped_lock lock(mutex_);
        data_.push_front(std::forward<T>(value));
    }

    template <typename T, typename Lock>
    bool ThreadSafeQueue<T, Lock>::empty() const
    {
        std::scoped_lock lock(mutex_);
        return data_.empty();
    }

    template <typename T, typename Lock>
    std::optional<T> ThreadSafeQueue<T, Lock>::popFront()
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

    template <typename T, typename Lock>
    std::optional<T> ThreadSafeQueue<T, Lock>::popBack()
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

    template <typename T, typename Lock>
    void ThreadSafeQueue<T, Lock>::moveToFront(const T &item)
    {
        std::scoped_lock lock(mutex_);
        auto iter = std::find(data_.begin(), data_.end(), item);

        if (iter != data_.end())
        {
            std::ignore = data_.erase(iter);
        }

        data_.push_front(item);
    }

    template <typename T, typename Lock>
    std::optional<T> ThreadSafeQueue<T, Lock>::copyFrontAndMoveToBack()
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
}