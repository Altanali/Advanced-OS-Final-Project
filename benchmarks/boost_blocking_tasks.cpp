#include <ctime>
#include <thread>
#include <chrono>
#include <cstdlib>
#include <iostream>
#include <boost/asio.hpp>
#include <boost/asio/thread_pool.hpp>
#include "tasks.hpp"
#include <stdlib.h>
using namespace std;
using namespace boost::asio;

const int num_tasks = 1000;      // Total number of tasks to execute
const int numRepeats = 3;
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
			
}

int main()
{

    std::cout << "Blocking/Nonblocking Benchmark Results:" << std::endl;
	int num_threads = 8;
	for(double percent_blocked = 0; percent_blocked <= 1; percent_blocked += 0.2) {
		double total_duration = 0;
		int num_tasks_executed = 0;
		int num_blocked_tasks = (int)floor(num_tasks*percent_blocked);
		cout << "Blocking " << num_blocked_tasks << " tasks.\n";
		for(int repeat = 0; repeat < numRepeats; ++repeat) {
			thread_pool threadPool(num_threads);

			num_tasks_executed += num_tasks;
			for(int block = 0; block < num_tasks; ++block) {
				blocked_tasks[block] = block < num_blocked_tasks ? 1 : 0;
			}
			random_shuffle(blocked_tasks, blocked_tasks + num_tasks);
			auto start_time = std::chrono::high_resolution_clock::now();
			// Measure the performance of tasks
			for (int i = 0; i < num_tasks; ++i)
			{
				post(threadPool, bind(&task, i));
			}

			threadPool.join();

			auto end_time = std::chrono::high_resolution_clock::now();
			auto elapsed_time = std::chrono::duration_cast<std::chrono::milliseconds>(end_time - start_time);
			total_duration += elapsed_time.count();
		}

		std::cout << "Thread Count: " << num_threads << endl;
		std::cout << "Percent Tasks Blocked: " << percent_blocked << endl;
		std::cout << "Average time per task: " << total_duration/num_tasks_executed << " ms per task.\n";
		std::cout << "Tasks per second: " << num_tasks_executed/total_duration*1000 << endl;
		std::cout << endl;
    }

    return 0;
}