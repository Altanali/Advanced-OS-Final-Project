#include <oneapi/tbb.h>
#include <time.h>
#include <iostream>
#include <thread>
#include <chrono>
#include <stdio.h>
#include <stdlib.h>
#include <assert.h>

using namespace std;
using namespace oneapi::tbb;
const int num_threads = 4;        // Number of threads in the pool
const int num_tasks = 10000;     // Total number of tasks to execute
const int tasks_per_batch = 1000; // Number of tasks in each batch


class ThreadSleep {
	public:
		void operator()( const oneapi::tbb::blocked_range<size_t> &r) const {
			if(r.size() > 1) {
					cout << "More than 1 task in this partition!\n";
					exit(1);
			}
			for(size_t i = r.begin(); i != r.end(); ++i) {
				this_thread::sleep_for(chrono::milliseconds(10));
			}
		}
};


void ParallelThreadSleep(size_t n) {
	oneapi::tbb::parallel_for(
		oneapi::tbb::blocked_range<size_t>(0, n),
		ThreadSleep(),
		oneapi::tbb::simple_partitioner()
		/*
			Simple Partitioner ensures each item a new sleep task is submitted
			per item in the range.
		*/
	);
}

 
int main() {

	global_control global_limit(
		global_control::max_allowed_parallelism, 
		num_threads
	);

	cout << "Template benchmark results: " << endl;
	double total_duration = 0;
	int iter = 0;
	int num_tasks_executed = 0;
	for(int i = 0; i < num_tasks; i += tasks_per_batch, ++iter) {
        int batch_size = std::min(tasks_per_batch, num_tasks - i);
		num_tasks_executed += batch_size;
		auto start_time = chrono::high_resolution_clock::now();
		ParallelThreadSleep(batch_size);
		auto end_time = chrono::high_resolution_clock::now();
		auto elapsed_time = std::chrono::duration_cast<std::chrono::milliseconds>(end_time - start_time);
		total_duration += elapsed_time.count();
		cout << "Iteration " << iter << "(" << i << "/" << num_tasks << ") total_duration: " << elapsed_time.count() << "ms.\n";
	}
	cout << "Average time per task: " << total_duration/num_tasks_executed << " ms per task.\n";
	return 0;
	
}
