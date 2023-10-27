#include <iostream>
#include <chrono>
#include <boost/thread/thread.hpp>
#include <boost/threadpool.hpp>

const int num_threads = 4;        // Number of threads in the pool
const int num_tasks = 100000;     // Total number of tasks to execute
const int tasks_per_batch = 1000; // Number of tasks in each batch

void workerTask(int task_id)
{
    // Simulate some work
    boost::this_thread::sleep_for(boost::chrono::milliseconds(1));
}

int main()
{
    // Create a thread pool with the specified number of threads
    boost::threadpool::thread_pool<> pool(num_threads);

    auto start_time = std::chrono::high_resolution_clock::now();

    for (int i = 0; i < num_tasks; i += tasks_per_batch)
    {
        int batch_size = std::min(tasks_per_batch, num_tasks - i);

        // Enqueue a batch of tasks
        for (int j = 0; j < batch_size; ++j)
        {
            pool.submit(boost::bind(workerTask, i + j));
        }

        // Wait for the batch of tasks to complete
        pool.wait();
    }

    auto end_time = std::chrono::high_resolution_clock::now();
    auto elapsed_time = std::chrono::duration_cast<std::chrono::milliseconds>(end_time - start_time);

    std::cout << "Throughput benchmark results:" << std::endl;
    std::cout << "Number of threads:" << num_threads << std::endl;
    std::cout << "Number of tasks:" << num_tasks << std::endl;
    std::cout << "Elapsed times (ms):" << elapsed_time << std::endl;
    std::cout << "Throughput (tasks per second)" << (static_cast<double>(num_tasks) / elapsed_time) * 1000 << std::endl;

    return 0;
}