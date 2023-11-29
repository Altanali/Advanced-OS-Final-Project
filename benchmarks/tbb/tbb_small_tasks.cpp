#include <oneapi/tbb.h>
#include <time.h>
#include <iostream>
#include <thread>
#include <chrono>
#include <stdio.h>
#include <assert.h>
#include <unordered_set>
#include "../tasks.hpp"

using namespace std;
using namespace oneapi::tbb;

const int max_threads = 16;        // Number of threads in the pool
const int num_tasks = 1000;      // Total number of tasks to execute
const int numRepeats = 3;


class ThreadTaskMatrixMultiply{
	public:
		void operator()( const oneapi::tbb::blocked_range<size_t> &r) const {
			assert(r.size() == 1);
			small_gemm_task();
			return;
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

	for(int num_threads = 1; num_threads <= max_threads; num_threads*=2) {
		double total_duration = 0;
		int iter = 0;
		int num_tasks_executed = 0;
		global_control global_limit(
			global_control::max_allowed_parallelism, 
			num_threads
		);

		for(int repeat = 0; repeat < numRepeats; ++repeat, ++iter) {
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