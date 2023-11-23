#include <ctime>
#include <chrono>
#include <cstdlib>
#include <iostream>

#include "../src/threadpool.hpp"
#include "tasks.hpp"

using namespace pool;

const unsigned int maxThreads = 16; // Maximum number of threads
const unsigned int numTasks = 1000; // Number of tasks to execute
const unsigned int numRepeats = 3;  // Number of times to repeat the test

// task for each thread to perform
void task()
{
    int m = 100;
    int n = 100;
    int k = 100;
    int rsA, rsB, rsC, csA, csB, csC;

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

    for (unsigned int numThreads = 1; numThreads <= maxThreads; numThreads *= 2)
    {
        double totalDuration = 0.0;
        for (unsigned int repeat = 0; repeat < numRepeats; ++repeat)
        {
            ThreadPool pool{numThreads};
            std::vector<std::future<void>> results(numTasks);

            // Measure the peformance of tasks
            auto startTime = std::chrono::high_resolution_clock::now();
            for (unsigned int i = 0; i < numTasks; ++i)
            {
                results[i] = pool.enqueue(task);
            }
            for (auto &result : results)
            {
                result.get();
            }

            auto endTime = std::chrono::high_resolution_clock::now();
            auto elapsedTime = std::chrono::duration_cast<std::chrono::milliseconds>(endTime - startTime);
            totalDuration += elapsedTime.count();

#ifdef VERBOSE
            double throuput = numTasks / elapsedTime.count() * 1000;
            std::cout << "Throughput (iteration " << repeat + 1 << ", " << numThreads << " threads): " << throuput << std::endl;
#endif // VERBOSE
        }

        double aveDuration = totalDuration / numRepeats;
        double aveThroughput = numTasks / aveDuration * 1000;

        std::cout << "Threads: " << numThreads << ", Throughput: " << aveThroughput << " tasks per second" << std::endl;
    }

    return 0;
}