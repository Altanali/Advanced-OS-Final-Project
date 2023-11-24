#include <oneapi/tbb.h>
#include <time.h>
#include <iostream>
#include <thread>
#include <chrono>
#include <stdio.h>
#include <stdlib.h>
#include <assert.h>
#include "tasks.hpp"
#include <random>
//Vary percent of blocking tasks
//Measure: 
//	Time per task, or per batch

using namespace std;
using namespace oneapi::tbb;
const int num_tasks = 1000;      // Total number of tasks to execute
const int numRepeats = 3;
const int num_threads = 8;
int blocked_tasks[num_tasks];

class ThreadTask{
	public:
		void operator()( const oneapi::tbb::blocked_range<size_t> &r) const {
			assert(r.size() == 1);
			int item = r.begin();
			//If task number == 2, block; else perform GEMM computation. 
			if(blocked_tasks[item]) {
				this_thread::sleep_for(chrono::milliseconds(100));
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
};




void ParallelThreadSleep(size_t n) {
	oneapi::tbb::parallel_for(
		oneapi::tbb::blocked_range<size_t>(0, n),
		ThreadTask(),
		oneapi::tbb::simple_partitioner()
		/*
			Simple Partitioner ensures each item a new sleep task is submitted
			per item in the range.
		*/
	);
}

 
int main() {

	

	cout << "Template benchmark results: " << endl;
	int iter = 0;
	// for(int num_threads = 1; num_threads <= max_threads; num_threads*=2) {

		global_control global_limit(
			global_control::max_allowed_parallelism, 
			num_threads
		);
		
		for(double percent_blocked = 0; percent_blocked <= 1; percent_blocked += 0.2) {
			double total_duration = 0;
			int num_tasks_executed = 0;
			int num_blocked_tasks = (int)floor(num_tasks*percent_blocked);
			cout << "Blocking " << num_blocked_tasks << " tasks.\n";

			for(int repeat = 0; repeat < numRepeats; ++repeat, ++iter) {

				num_tasks_executed += num_tasks;
				for(int block = 0; block < num_tasks; ++block) {
					blocked_tasks[block] = block < num_tasks ? 0 : 1;
				}
				std::random_device rd;
				std::mt19937 g(rd());

				std::shuffle(blocked_tasks, blocked_tasks + num_tasks, g);				auto start_time = chrono::high_resolution_clock::now();
				ParallelThreadSleep(num_tasks);
				auto end_time = chrono::high_resolution_clock::now();
				auto elapsed_time = std::chrono::duration_cast<std::chrono::milliseconds>(end_time - start_time);
				total_duration += elapsed_time.count();
			}
			cout << "Thread Count: " << num_threads << endl;
			cout << "Percent Tasks Blocked: " << percent_blocked << endl;
			cout << "Average time per task: " << total_duration/num_tasks_executed << " ms per task.\n";
			cout << "Tasks per second: " << num_tasks_executed/total_duration*1000 << endl;
			cout << endl;
		}
	// }
	return 0;
	
}
