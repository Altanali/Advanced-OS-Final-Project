#include <oneapi/tbb.h>
#include <oneapi/tbb/spin_mutex.h>
#include <time.h>
#include <iostream>
#include <thread>
#include <chrono>
#include <stdio.h>
#include <assert.h>
#include <unordered_set>
#include "tasks.hpp"
#include <vector>
#include <stdlib.h>

using namespace std;
using namespace oneapi::tbb;

const int max_threads = 16;        // Number of threads in the pool
const int num_tasks = 1000;      // Total number of tasks to execute
const int numRepeats = 3;

vector<oneapi::tbb::spin_mutex> locks(num_tasks/2); //non blocking mutex
vector<int> task_order;
class ThreadTaskMatrixMultiply{
	public:
		void operator()( const oneapi::tbb::blocked_range<size_t> &r) const {

			assert(r.size() == 1);
			size_t item = task_order[r.begin()]; 
			int lock_idx = item % 2 ? (item - 1)/2 : item/2;
			int m = 100;
			int n = 100;
			int k = 100;
			int rsA, rsB, rsC, 
			csA, csB, csC;

			rsA = rsC = m;
			rsB = k;
			csA = csB = csC = 1;
			{
				oneapi::tbb::spin_mutex::scoped_lock lock(locks[lock_idx]);
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


void ParallelThreadTask(size_t n) {
	oneapi::tbb::parallel_for(
		oneapi::tbb::blocked_range<size_t>(0, n),
		ThreadTaskMatrixMultiply(),
		oneapi::tbb::simple_partitioner()
		/*
			Simple Partitioner ensures each item a new sleep task is submitted
			per item in the range.
		*/
	);
}

 
 
int main() {

	

	cout << "Template benchmark results: " << endl;
	for(int i = 0; i < num_tasks; ++i) task_order.push_back(i);
	for(int num_threads = 1; num_threads <= max_threads; num_threads*=2) {
		double total_duration = 0;
		int iter = 0;
		int num_tasks_executed = 0;
		global_control global_limit(
			global_control::max_allowed_parallelism, 
			num_threads
		);

		for(int repeat = 0; repeat < numRepeats; ++repeat, ++iter) {
			random_shuffle(task_order.begin(), task_order.end());
			num_tasks_executed += num_tasks;
			auto start_time = chrono::high_resolution_clock::now();
			ParallelThreadTask(num_tasks);
			auto end_time = chrono::high_resolution_clock::now();
			auto elapsed_time = std::chrono::duration_cast<std::chrono::milliseconds>(end_time - start_time);
			total_duration += elapsed_time.count();
		}
		cout << "Thread Count: " << num_threads << endl;
		cout << "Average time per task: " << total_duration/num_tasks_executed << " ms per task.\n";
		cout << "Tasks per second: " << num_tasks_executed/total_duration*1000 << endl;
		cout << endl;
	}
	return 0;
	
}