#include <ctime>
#include <thread>
#include <chrono>
#include <cstdlib>
#include <iostream>
#include <boost/asio.hpp>
#include <boost/asio/thread_pool.hpp>
#include "tasks.hpp"
using namespace boost::asio;

const int maxThreads = 16; // Maximum number of threads to test
const int numTasks = 1000; // Number of tasks to execute
const int numRepeats = 3;  // Number of times to repeat the test

// task for each thread to perform
void task()
{
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

int main()
{

    std::cout << "Scalability Benchmark Results:" << std::endl;

    for (int numThreads = 1; numThreads <= maxThreads; numThreads *= 2)
    {
        double totalDuration = 0.0;

        for (int repeat = 0; repeat < numRepeats; ++repeat)
        {
            thread_pool threadPool(numThreads);

            // Measure the performance of tasks
            auto startTime = std::chrono::high_resolution_clock::now();
            for (int i = 0; i < numTasks; ++i)
            {
                post(threadPool, task);
            }

            threadPool.join();

            auto endTime = std::chrono::high_resolution_clock::now();
            auto elapsedTime = std::chrono::duration_cast<std::chrono::milliseconds>(endTime - startTime);
            totalDuration += elapsedTime.count();
            double throughput = numTasks / elapsedTime.count() * 1000;
#ifdef VERBOSE
            std::cout << "Throughput (iteration " << repeat + 1 << ", "
                      << numThreads << " threads): " << throughput << std::endl;
#endif // VERBOSE
        }

        double aveDuration = totalDuration / numRepeats;
        double aveThroughput = numTasks / aveDuration * 1000;

        std::cout << "Threads: " << numThreads << ", Throughput: "
                  << aveThroughput << " tasks per second" << std::endl;
    }

    return 0;
}