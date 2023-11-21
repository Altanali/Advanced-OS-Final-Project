#ifndef THREAD_POOL_H
#define THREAD_POOL_H

#include <atomic>
#include <barrier>
#include <deque>
#include <functional>
#include <future>
#include <memory>
#include <semaphore>
#include <thread>
#include <type_traits>

#ifdef __has_include
#if __has_include(<version>)
#include <version>
#endif
#endif

#include "thread_safe_queue.hpp"

namespace pool
{
    namespace details
    {
        using default_function_type = std::function<void()>;
    } // namespace details

    template <typename FunctionType = details::default_function_type>
    class ThreadPool
    {
    public:
        // Constructor: creates a thread pool with a specified number of threads
        explicit ThreadPool(const unsigned int &numberOfThreads = std::thread::hardware_concurrency());

        // Destructor: cleans up the thread pool
        ~ThreadPool();

        // Disallow copy operations
        ThreadPool(const ThreadPool &) = delete;
        ThreadPool &operator=(const ThreadPool &) = delete;

        // Enqueue a task that returns a result
        template <typename Function, typename... Args, typename ReturnType = std::invoke_result_t<Function &&, Args &&...>>
        [[nodiscard]] std::future<ReturnType> enqueue(Function f, Args... args);

        // Enqueue a task that doesn't return a result and immediately detaches it
        template <typename Function, typename... Args>
        void enqueueDetach(Function &&f, Args &&...args);

        // Get the number of threads in the pool
        [[nodiscard]] auto size() const;

    private:
        // Enqueue a task into the task queue
        template <typename Function>
        void enqueueTask(Function &&f)
        {
            auto iOpt = priorityQueue_.copyFrontAndMoveToBack();
            if (!iOpt.has_value())
            {
                // would only be a problem if there were 0 threads
                return;
            }
            auto i = *(iOpt);
            pendingTasks_.fetch_add(1, std::memory_order_relaxed);
            tasks_[i].tasks.pushBack(std::forward<Function>(f));
            tasks_[i].signal.release();
        }

        // Structure to hold task-related information for each thread
        struct taskItem
        {
            pool::ThreadSafeQueue<FunctionType> tasks{};
            std::binary_semaphore signal{0}; // Flag to signal the thread to stop
        }

        // Vector to hold threads
        std::vector<std::thread>
            threads_;

        // Deque to store task items
        std::deque<taskItem> tasks_;

        // Queue to manage thread priorities
        pool::ThreadSafeQueue<std::size_t> priorityQueue_;

        // Counter for pending tasks
        std::atomic_int_fast64_t pendingTasks_{};
    };

} // namespace pool
#endif // !THREAD_POOL_H