#include <oneapi/tbb.h>
#include <time.h>
#include <iostream>
#include <thread>
#include <chrono>
#include <stdio.h>
#include <assert.h>
#include <unordered_set>
#include "../tasks.hpp"

using namespace oneapi::tbb;

const int max_threads = 16;
const int max_repeats = 3;


class ThreadTask{
	public:
		void operator()( const oneapi::tbb::blocked_range<size_t> &r) const {
			assert(r.size() == 1);
			int item = 2;
			item += 4;
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
    oneapi::tbb::task_scheduler_handle handle{tbb::attach{}};

    std::cout << "OneTBB Scalability Benchmark Results:" << std::endl;

	for(int numThreads = 1; numThreads <= max_threads; numThreads *= 2) {
        double totalCreationDuration = 0;
		double totalTerminationDuration = 0;
		global_control global_limit(
			global_control::max_allowed_parallelism, 
			numThreads
		);
		for(int i = 0; i < max_repeats; ++i) {
			
			auto period1 = chrono::high_resolution_clock::now();
			ParallelThreadSleep(10000);
			oneapi::tbb::finalize(handle);

			auto period2 = std::chrono::high_resolution_clock::now();

			auto period3 = std::chrono::high_resolution_clock::now();

			totalCreationDuration += std::chrono::duration_cast<std::chrono::milliseconds>(period2 - period1).count();
			totalTerminationDuration += std::chrono::duration_cast<std::chrono::milliseconds>(period3 - period2).count();
		}

		std::cout << "Thread's Spawned: " << numThreads << endl;
		std::cout << "Total Creation Duration: " << totalCreationDuration << endl;
		std::cout << "Average thread creation: " << totalCreationDuration/(numThreads*max_repeats)*1000 << std::endl;
		std::cout << std::endl;

	}
	return 0;
	
}
