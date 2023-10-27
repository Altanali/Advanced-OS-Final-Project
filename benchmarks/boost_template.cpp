#include <iostream>
#include <boost/chrono.hpp>
#include <boost/thread/thread.hpp>
#include <boost/asio/thread_pool.hpp>
#include <boost/asio/post.hpp>
#include <unistd.h>

using namespace std;

const int num_threads = 4;        // Number of threads in the pool
const int num_tasks = 100000;     // Total number of tasks to execute
const int tasks_per_batch = 1000; // Number of tasks in each batch

void workerTask(int task_id)
{
    // Simulate some work
    cout << task_id << " is going to sleep." << endl;
    
    // boost::this_thread::sleep_for(boost::chrono::milliseconds(1));
    sleep(1);
    cout << task_id << " has woken up." << endl;

}

int main()
{
    // Create a thread pool with the specified number of threads
    boost::asio::thread_pool pool(num_threads);

    auto start_time = std::chrono::high_resolution_clock::now();

    for (int i = 0; i < num_tasks; i += tasks_per_batch)
    {
        int batch_size = std::min(tasks_per_batch, num_tasks - i);

        // Enqueue a batch of tasks
        for (int j = 0; j < batch_size; ++j)
        {
            boost::asio::post(pool, boost::bind(&workerTask, i + j));
            // pool.submit(boost::bind(workerTask, i + j));
        }

        // Wait for the batch of tasks to complete
        pool.join();
    }

    auto end_time = std::chrono::high_resolution_clock::now();
    auto elapsed_time = std::chrono::duration_cast<std::chrono::milliseconds>(end_time - start_time);
    
    cout << "Throughput benchmark results: " << endl;

}