#include <oneapi/tbb.h>
#include <time.h>
#include <iostream>
#include <thread>
#include <chrono>
#include <stdio.h>
#include <assert.h>
#include <unordered_set>
#include "tasks.hpp"

using namespace std;
using namespace oneapi::tbb;

const int maxThreads = 16; // Maximum number of threads to test
const int numTasks = 1000; // Number of tasks to execute
const int numRepeats = 3;  // Number of times to repeat the test


class ThreadTaskMatrixMultiply{
	public:
		void operator()( const oneapi::tbb::blocked_range<size_t> &r) const {

			assert(r.size() == 1);
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

    std::cout << "OneTBB Scalability Benchmark Results:" << std::endl;

	for(int numThreads = 1; numThreads <= maxThreads; numThreads *= 2) {
		

		// Setup configuration to fix number of threads used by OneTBB
		oneapi::tbb::global_control global_limit(
			oneapi::tbb::global_control::max_allowed_parallelism, 
			numThreads
		);

		assert(global_control::active_value(global_control::max_allowed_parallelism) == numThreads);

        double totalDuration = 0.0;

		for(int repeat = 0; repeat < numRepeats; ++repeat) {
			auto startTime = chrono::high_resolution_clock::now();
			ParallelThreadTask(numTasks); 
			auto endTime = chrono::high_resolution_clock::now();
			auto elapsedTime = std::chrono::duration_cast<std::chrono::milliseconds>(endTime - startTime).count();
			totalDuration += elapsedTime;
		}

		double aveDuration = totalDuration / numRepeats;
        double aveThroughput = numTasks / aveDuration * 1000;

        std::cout << "Threads: " << numThreads << ", Throughput: "
                  << aveThroughput << " tasks per second" << std::endl;

	}
	return 0;
	
}
