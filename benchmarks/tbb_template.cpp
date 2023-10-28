#include <oneapi/tbb.h>
#include <time.h>
#include <iostream>
#include <thread>
#include <chrono>
#include <stdio.h>


using namespace std;


class ThreadSleep {
	public:
		void operator()( const oneapi::tbb::blocked_range<size_t> &r) const {
			for(size_t i = r.begin(); i != r.end(); ++i) {
				printf("%zu is going to sleep.\n", i);
				this_thread::sleep_for(chrono::milliseconds(10));
				printf("%zu has woken up.\n", i);
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
	
	auto start_time = chrono::high_resolution_clock::now();
	ParallelThreadSleep(1000);
	auto end_time = chrono::high_resolution_clock::now();
    auto elapsed_time = std::chrono::duration_cast<std::chrono::milliseconds>(end_time - start_time);

    cout << "Throughput benchmark results: " << endl;

	return 0;
	
}
