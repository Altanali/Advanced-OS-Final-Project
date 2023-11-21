#include "thread_pool.hpp"

namespace pool
{
    template <typename FunctionType>
    ThreadPool<FunctionType>::ThreadPool(const unsigned int &numberOfThreads) : tasks_(numberOfThreads)
    {
        std::size_t currentId = 0;
        for (std::size_t i = 0; i < numberOfThreads; ++i)
        {
            priorityQueue_.pushBack(size_t(currentId));

            try
            {
                threads_.emplace_back([&, id = currentId](const std::stop_token &stop_tok)
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
                                } catch (...) {

                                }
                            }

                            // try to steal a task
                            for (std::size_t j = 1; j < tasks_.size(); ++j) {
                                const std::size_t index = (id + j) % tasks_.size();
                                if (auto task = tasks_[index].tasks.steal()) {
                                    // steal a task
                                    pendingTasks_.fetch_sub(1, std::memory_order_release);
                                    std::invoke(std::move(task.value()));

                                    // stop stealing once we have invoked a stolen task
                                    break;
                                }
                            }
                        } while (pendingTasks_.load(std::memory_order_acquire) > 0);

                        priorityQueue_.moveToFront(id);

                    } while (!stop_tok.stop_requested()); });

                // increment thread id
                ++currentId;
            }
            catch (...)
            {
                // catch all

                // remove one item from the tasks
                tasks_.pop_back();

                // remove our thread from the priority queue
                std::ignore = priorityQueue_.popBack();
            }
        }
    }

    template <typename FunctionType>
    ThreadPool<FunctionType>::~ThreadPool()
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
     * @details Note that task execution begins once the task is enqueued.
     * @tparam Function An invokable type.
     * @tparam Args Argument parameter pack.
     * @tparam ReturnType The return type of the Function.
     * @param f The callable function.
     * @param args The parameters that will be passed (copied) to the function.
     * @return A std::future<ReturnType> that can be used to retrieve the returned value.
     */
    template <typename FunctionType>
    template <typename Function, typename... Args, typename ReturnType>
    std::future<ReturnType> ThreadPool<FunctionType>::enqueue(Function f, Args... args)
    {
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
                promise.set_exception(std::current_exception());
            }
        };

        // get the future before enqueuing the task
        auto future = sharedPromise->get_future();
        // enqueue task
        enqueueTask(std::move(task));
        return future;
    }

    /**
     * @brief Enqueue a task to be executed in the thread pool that returns void.
     * @tparam Function An invokable type.
     * @tparam Args Argument parameter pack for Function
     * @param func The callable to be executed
     * @param args Arguments that will be passed to the function.
     */
    template <typename FunctionType>
    template <typename Function, typename... Args>
    void ThreadPool<FunctionType>::enqueueDetach(Function &&f, Args &&...args)
    {
        enqueueTask(
            std::move([f = std::forward<Function>(func),
                       ... largs = std::forward<Args>(args)]() mutable -> decltype(auto)
                      {
                          try
                          {
                              std::invoke(f, largs);
                          }
                          catch (...)
                          {
                          } }));
    }

    template <typename FunctionType>
    auto ThreadPool<FunctionType>::size() const
    {
        return threads_.size();
    }
}