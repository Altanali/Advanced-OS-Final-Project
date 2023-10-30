#include <ctime>
#include <vector>
#include <chrono>
#include <cstdlib>
#include <iostream>
#include <boost/asio.hpp>
#include <boost/asio/threadpool.hpp>

using namespace std;

const int num_threads = 4;        // Number of threads in the pool
const int num_tasks = 100000;     // Total number of tasks to execute
const int tasks_per_batch = 1000; // Number of tasks in each batch

// function to be executed by the thread pool
void workerTask(int task_id)
{
    // seed the random number generator with the current time
    srand(static_cast<unsigned>(time(nullptr)));

    cout << task_id << " beginning task." << endl;
    for (int i = 0; i < 1000; ++i)
    {
        // Simulate rolling a six-sided die
        int result = rand() % 6 + 1;
    }
    cout << task_id << " ending task." << endl;
}

int main()
{
    // Create a thread pool with the specified number of threads
    boost::asio::io_context;
    boost::asio::thread_pool pool(num_threads);

    // Start measuring time
    auto start_time = std::chrono::high_resolution_clock::now();

    // post the work to the thread pool
    for (int i = 0; i < num_tasks; i += tasks_per_batch)
    {
        int batch_size = min(tasks_per_batch, num_tasks - i);

        // Enqueue a batch of tasks
        for (int j = 0; j < batch_size; ++j)
        {
            boost::asio::post(pool, bind(workerTask, i + j));
        }

        // Wait for the batch of tasks to complete
        pool.join();
    }

    // Stop measuring time
    auto end_time = std::chrono::high_resolution_clock::now();
    auto elapsed_time = std::chrono::duration_cast<std::chrono::milliseconds>(end_time - start_time);

    // Calculate and print throughput
    auto throughput = (static_cast<double>(num_tasks) / elapsed_time.count()) * 1000;
    cout << "Throughput benchmark results: " << endl;
    cout << "Total tasks: " << num_tasks << endl;
    cout << "Elapsed time: " << elapsed_time.count() / 1000 << " seconds" << endl;
    cout << "Throughput: " << throughput << " tasks per second" << endl;

    return 0;
}