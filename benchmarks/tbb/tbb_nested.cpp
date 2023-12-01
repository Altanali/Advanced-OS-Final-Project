#include <oneapi/tbb.h>
#include <time.h>
#include <iostream>
#include <thread>
#include <chrono>
#include <stdio.h>
#include <assert.h>
#include <unordered_set>
#include <thread>
#include "../tasks.hpp"

using namespace std;
using namespace oneapi::tbb;

const int max_threads = 16;        // Number of threads in the pool
const int num_tasks = 3;      // Total number of tasks to execute
const int numRepeats = 3;
const int num_sub_tasks = 200;

unordered_set<thread::id> thread_ids;

 
 
int main() {


	rss_out_sema.acquire();
	jthread track_rss_thread(bind(track_rss, "tbb_scalability_rss.csv", get_curr_time_long()));
    std::cout << "Scalability Benchmark Results:" << std::endl;
	
	for(int num_threads = 4; num_threads <= max_threads; ++num_threads) {
		double total_duration = 0;
		int iter = 0;
		int num_tasks_executed = 0;
		global_control global_limit(
			global_control::max_allowed_parallelism, 
			num_threads
		);

		for(int repeat = 0; repeat < numRepeats; ++repeat, ++iter) {
			thread_ids.clear();
			num_tasks_executed += num_tasks*num_sub_tasks;
			auto start_time = chrono::high_resolution_clock::now();

			oneapi::tbb::parallel_for( 0, num_tasks, []( int i ) {
				oneapi::tbb::parallel_for( 0, num_sub_tasks, []( int j ) { 
					big_gemm_task();
				});
			});

			auto end_time = chrono::high_resolution_clock::now();
			auto elapsed_time = std::chrono::duration_cast<std::chrono::milliseconds>(end_time - start_time);
			total_duration += elapsed_time.count();


		}
		
		cout << num_tasks_executed/total_duration*1000 << endl;
	}

	rss_out_sema.release();
	track_rss_thread.join();
	struct rusage usage;
	getrusage(RUSAGE_SELF, &usage); 
	cout << "MaxRSS: " << usage.ru_maxrss << endl;


	return 0;
	
}