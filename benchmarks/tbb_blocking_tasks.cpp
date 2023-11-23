#include <oneapi/tbb.h>
#include <time.h>
#include <iostream>
#include <thread>
#include <chrono>
#include <stdio.h>
#include <stdlib.h>
#include <assert.h>
#include "tasks.hpp"

//Vary percent of blocking tasks
//Measure: 
//	Time per task, or per batch

using namespace std;
using namespace oneapi::tbb;
const int max_threads = 16;        // Number of threads in the pool
const int num_tasks = 1000;      // Total number of tasks to execute
const int numRepeats = 3;

class ThreadTask{
	public:
		void operator()( const oneapi::tbb::blocked_range<size_t> &r) const {
			assert(r.size() == 1);
			int item = r.begin();
			//If task number == 2, block; else perform GEMM computation. 
			if(item % 2) {
				this_thread::sleep_for(chrono::milliseconds(20));
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
	double total_duration = 0;
	int iter = 0;
	int num_tasks_executed = 0;
	for(int num_threads = 1; num_threads <= max_threads; num_threads*=2) {

		global_control global_limit(
			global_control::max_allowed_parallelism, 
			num_threads
		);

		for(int repeat = 0; repeat < numRepeats; ++repeat, ++iter) {
			num_tasks_executed += num_tasks;
			auto start_time = chrono::high_resolution_clock::now();
			ParallelThreadSleep(num_tasks);
			auto end_time = chrono::high_resolution_clock::now();
			auto elapsed_time = std::chrono::duration_cast<std::chrono::milliseconds>(end_time - start_time);
			total_duration += elapsed_time.count();
		}
		cout << "Thread Count: " << num_threads << endl;
		cout << "Average time per task: " << total_duration/num_tasks_executed << " ms per task.\n";
		cout << "Tasks per second: " << num_tasks_executed/total_duration*1000 << endl;

	}
	return 0;
	
}
