#include <ctime>
#include <thread>
#include <chrono>
#include <cstdlib>
#include <iostream>
#include "../tasks.hpp"
#include <stdlib.h>
#include <cmath>
#include <semaphore>
#include <condition_variable>
#include <mutex>
#include <vector>

using namespace std;

const int num_tasks = 1000;      // Total number of tasks to execute
const int numRepeats = 3;
const int max_threads = 8;        // Number of threads in the pool
int task_counter = 0;
constexpr std::ptrdiff_t max_sema_threads{max_threads};
counting_semaphore sema{max_sema_threads};
mutex mtx;
condition_variable cv;
mutex thread_mtx;


int blocked_tasks[num_tasks];


// task for each thread to perform
void task(int idx)
{
	if(blocked_tasks[idx]) {
		std::this_thread::sleep_for(std::chrono::milliseconds(100));
	} else {
		int m = 100;
		int n = 100;
		int k = 100;
		int rsA, rsB, rsC, 
		csA, csB, csC;

		rsA = rsC = m;
		rsB = k;
		csA = csB = csC = 1;

		double *A = randomMatrix(m, k);
		double *B = randomMatrix(k, n);
		double *C = randomMatrix(m, n);

		GEMM(m, n, k, 
			A, rsA, csA, 
			B, rsB, csB,
			C, rsC, csC);

		
		free(A);
		free(B);
		free(C);
	}

	sema.release(); 
	{
	scoped_lock<mutex> lck(thread_mtx);
		++task_counter;
		cv.notify_one();
		// cout << "Finished by: " << id << endl;
	}
			
}

int main()
{

    std::cout << "Blocking/Nonblocking Benchmark Results:" << std::endl;
	for(double percent_blocked = 0; percent_blocked <= 1; percent_blocked += 0.2) {
		double total_duration = 0;
		int num_tasks_executed = 0;
		int num_blocked_tasks = (int)floor(num_tasks*percent_blocked);
		cout << "Blocking " << num_blocked_tasks << " tasks.\n";
		for(int repeat = 0; repeat < numRepeats; ++repeat) {


			for(int block = 0; block < num_tasks; ++block) {
				blocked_tasks[block] = block < num_blocked_tasks ? 1 : 0;
			}
			random_shuffle(blocked_tasks, blocked_tasks + num_tasks);
			auto start_time = std::chrono::high_resolution_clock::now();
			
			// Measure the performance of tasks
			vector<jthread> thread_set;

			task_counter = 0;

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
			auto end_time = std::chrono::high_resolution_clock::now();
			auto elapsed_time = std::chrono::duration_cast<std::chrono::milliseconds>(end_time - start_time);
			total_duration += elapsed_time.count();
			num_tasks_executed += num_tasks;
		}

		std::cout << "Thread Count: " << max_threads << endl;
		std::cout << "Percent Tasks Blocked: " << percent_blocked << endl;
		std::cout << "Average time per task: " << total_duration/num_tasks_executed << " ms per task.\n";
		std::cout << "Tasks per second: " << num_tasks_executed/total_duration*1000 << endl;
		std::cout << endl;
    }

    return 0;
}