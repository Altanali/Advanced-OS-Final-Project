#include <time.h>
#include <iostream>
#include <thread>
#include <chrono>
#include <stdio.h>
#include <stdlib.h>
#include <assert.h>
#include "../tasks.hpp"
#include <semaphore>
#include <condition_variable>
#include <mutex>
#include <vector>
using namespace std;

const int max_threads = 1;        // Number of threads in the pool
const int num_tasks = 1000;      // Total number of tasks to execute
const int numRepeats = 3;
int task_counter = 0;
constexpr std::ptrdiff_t max_sema_threads{max_threads};
counting_semaphore sema{max_sema_threads};
mutex mtx;
condition_variable cv;
mutex thread_mtx;


void task(int id=-1) {

	small_gemm_task();

	sema.release(); 
	{
	scoped_lock<mutex> lck(thread_mtx);
		++task_counter;
		cv.notify_one();
		// cout << "Finished by: " << id << endl;
	}
	return;
}


int main()
{

    std::cout << "Scalability Benchmark Results:" << std::endl;

    for (int num_threads = max_threads; num_threads <= max_threads; num_threads *= 2)
    {
        double total_duration = 0;
        int num_tasks_executed = 0;

        for (int repeat = 0; repeat < numRepeats; ++repeat)
        {
			task_counter = 0;
            // Measure the performance of tasks
            auto start_time = std::chrono::high_resolution_clock::now();
			vector<jthread> thread_set;
            for (int i = 0; i < num_tasks; ++i)
            {
				sema.acquire(); //semaphore counter == max_threads, blocks when max_threads are running concurrently. threads call sema.release() before exiting.
				thread_set.emplace_back(jthread(task, i));
            }

			for(auto &t : thread_set) t.join();
			
			unique_lock<mutex> lck(mtx);
			cv.wait(lck, []() {
				return task_counter == num_tasks; //monitor to wait until all 1000 tasks have been completed 
			});

			// for(int i = 0; i < num_tasks; ++i) task();
			

			auto end_time = std::chrono::high_resolution_clock::now();
			auto elapsed_time = std::chrono::duration_cast<std::chrono::milliseconds>(end_time - start_time);
			total_duration += elapsed_time.count();
			num_tasks_executed += num_tasks;

        }

		cout << "Hardware concurrency: " << thread::hardware_concurrency() << endl;
        cout << "Thread Count: " << num_threads << endl;
		cout << "Average time per task: " << total_duration/num_tasks_executed << " ms per task.\n";
		cout << "Tasks per second: " << num_tasks_executed/total_duration*1000 << endl;
		cout << endl;

	}
    return 0;
}