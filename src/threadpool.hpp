#ifndef THREADPOOL
#define THREADPOOL

#include <atomic>
#include <barrier>
#include <deque>
#include <functional>
#include <future>
#include <memory>
#include <semaphore>
#include <thread>
#include <type_traits>

#include "thread_safe_queue.hpp"

namespace pool
{
    namespace details
    {
#ifdef __cpp_lib_move_only_function
        using default_function_type = std::move_only_function<void()>;
#else
        using default_function_type = std::function<void()>;
#endif
    } // namespace details

    template <typename FunctionType = details::default_function_type>
    class ThreadPool
    {
    public:
        // Constructor: creates a thread pool with specified number of threads
        explicit ThreadPool(const unsigned int &numberOfThreads = std::thread::hardware_concurrency())
            : tasks_(numberOfThreads)
        {
            std::size_t currentId = 0;
            for (std::size_t i = 0; i < numberOfThreads; ++i)
            {
                priorityQueue_.pushBack(size_t(currentId));
                try
                {
                    threads_.emplace_back([&, id = currentId](const std::stop_token &stop_token)
                                          {
                        do {
                            // wait until signaled
                            tasks_[id].signal.acquire();
                            do {
                                // invoke the task
                                while (auto task = tasks_[id].tasks.popFront()) {
                                    try {
                                        pendingTasks_.fetch_sub(1, std::memory_order_release);
                                        std::invoke(std::move(task.value()));
                                    } catch (...) {}
                                }

                                // try to steal a task
                                for (std::size_t j = 1; j < tasks_.size(); ++j) {
                                    const std::size_t index = (id + j) % tasks_.size();
                                    if (auto task = tasks_[index].tasks.steal()) {
                                        pendingTasks_.fetch_sub(1, std::memory_order_release);
                                        std::invoke(std::move(task.value()));
                                        // stop stealing once invoked a stolen task
                                        break;
                                    }
                                }
                            } while (pendingTasks_.load(std::memory_order_acquire) > 0);

                            priorityQueue_.moveToFront(id);
                        } while (!stop_token.stop_requested()); });
                    // increment thread ID
                    ++currentId;
                }
                catch (...)
                {
                    // catch all

                    tasks_.pop_back();                      // remove item from task
                    std::ignore = priorityQueue_.popBack(); // remove thread from queue
                }
            }
        }

        // Destructor: cleans up the thread pool
        ~ThreadPool()
        {
            // stop all threads
            for (std::size_t i = 0; i < threads_.size(); ++i)
            {
                threads_[i].request_stop();
                tasks_[i].signal.release();
                threads_[i].join();
            }
        }

        /**
         * @brief Enqueue a task into the thread pool that returns a result.
         *
         * @details Note that task execution begins once the task is enqueued
         *
         * @tparam Function An invokable type
         * @tparam Args Argument parameter pack
         * @tparam ReturnType The return type of the Function
         * @param f The callable function
         * @param args The parameters that will be passed (copied) to the function
         * @return A `std::future<ReturnType>` that can be used to retrieve the returned value
         *
         */
        template <typename Function, typename... Args, typename ReturnType = std::invoke_result_t<Function &&, Args &&...>>
        [[nodiscard]] std::future<ReturnType> enqueue(Function f, Args... args)
        {
#ifdef __cpp_lib_move_only_function
            // C++23 support
            std::promise<ReturnType> promise;
            auto future = promise.get_future();
            auto task = [func = std::move(f), ... largs = std::move(args), promise = std::move(promise)]() mutable
            {
                try
                {
                    if constexpr (std::is_same_v<ReturnType, void>)
                    {
                        func(largs...);
                        promise.set_value();
                    }
                    else
                    {
                        promise.set_value(func(largs...));
                    }
                }
                catch (...)
                {
                    promise.set_exception(std::current_exception());
                }
            };
            enqueueTask(std::move(task));
            return future;
#else
            auto sharedPromise = std::make_shared<std::promise<ReturnType>>();
            auto task = [func = std::move(f), ... largs = std::move(args), promise = sharedPromise]()
            {
                try
                {
                    if constexpr (std::is_same_v<ReturnType, void>)
                    {
                        func(largs...);
                        promise->set_value();
                    }
                    else
                    {
                        promise->set_value(func(largs...));
                    }
                }
                catch (...)
                {
                    promise->set_exception(std::current_exception());
                }
            };

            // get future before enqueuing task
            auto future = sharedPromise->get_future();
            // enqueue
            enqueueTask(std::move(task));
            return future;
#endif
        }

        /**
         * @brief Enqueue a task to be executed in the thread pool that returns void.
         *
         * @tparam Function An invokable type
         * @tparam Args Argument parameter pack for Function
         * @param func The callable to be executed
         * @param args Arguments that will be passed to the function.
         */
        template <typename Function, typename... Args>
        void enqueueDetach(Function &&func, Args &&...args)
        {
            enqueueTask(
                std::move([f = std::forward<Function>(func), ... largs = std::forward<Args>(args)]() mutable -> decltype(auto)
                          {
                    // suppress exceptions
                    try
                    {
                        std::invoke(f, largs...);
                    }
                    catch (...) {} }));
        }

        // Get the number of threads in the pool
        [[nodiscard]] auto size() const
        {
            return threads_.size();
        }

    private:
        // Structure to hold task-related information for each thread
        struct TaskItem
        {
            pool::ThreadSafeQueue<FunctionType> tasks{};
            std::binary_semaphore signal{0}; // Indicator signal the thread to stop
        };

        std::vector<std::jthread> threads_;                // vector to hold threads
        std::deque<TaskItem> tasks_;                       // deque to hold task items
        pool::ThreadSafeQueue<std::size_t> priorityQueue_; // queue to manage thread priorities
        std::atomic_int_fast64_t pendingTasks_{};          // counter for pending tasks

        // Enqueue a task into the task queue
        template <typename Function>
        void enqueueTask(Function &&f)
        {
            auto iOpt = priorityQueue_.copyFrontAndMoveToBack();
            if (!iOpt.has_value())
            {
                return;
            }
            auto i = *(iOpt);
            pendingTasks_.fetch_add(1, std::memory_order_relaxed);
            tasks_[i].tasks.pushBack(std::forward<Function>(f));
            tasks_[i].signal.release();
        }
    };
} // namespace pool

#endif // !THREADPOOL