#include <ctime>
#include <chrono>
#include <cstdlib>
#include <iostream>
#include <random>
#include <stdlib.h>

#include "../src/threadpool.hpp"
#include "tasks.hpp"

using namespace pool;

const unsigned int numTasks = 1000;
const unsigned int numRepeats = 3;
unsigned int blockedTasks[numTasks];

void task(int idx)
{
    if (blockedTasks[idx])
    {
        std::this_thread::sleep_for(std::chrono::milliseconds(50));
    }
    else
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
}

int main()
{
    std::cout << "Blocking/Nonblocking Benchmark Results:" << std::endl;
    unsigned int maxThreads = 8;

    for (double percentBlocked = 0; percentBlocked <= 1; percentBlocked += 0.2)
    {
        double totalDuration = 0;
        int numTasksExecuted = 0;
        int numBlockedTasks = (int)floor(numTasks * percentBlocked);
        std::cout << "Blocking " << numBlockedTasks << " tasks" << std::endl;

        for (int repeat = 0; repeat < numRepeats; ++repeat)
        {
            ThreadPool pool{maxThreads};
            std::vector<std::future<void>> results(numTasks);
            numTasksExecuted += numTasks;

            for (int block = 0; block < numTasks; ++block)
            {
                blockedTasks[block] = (block < numTasks) ? 0 : 1;
            }

            std::random_device rd;
            std::mt19937 g(rd());

            std::shuffle(blockedTasks, blockedTasks + numTasks, g);
            auto startTime = std::chrono::high_resolution_clock::now();
            for (int i = 0; i < numTasks; ++i)
            {
                results[i] = pool.enqueue(task, i);
            }
            for (auto &result : results)
            {
                result.get();
            }

            auto endTime = std::chrono::high_resolution_clock::now();
            auto elapsedTime = std::chrono::duration_cast<std::chrono::milliseconds>(endTime - startTime);
            totalDuration += elapsedTime.count();
        }

#ifdef VERBOSE
        std::cout << "Percent of Tasks blocked: " << percentBlocked * 100 << "%, Average time per task: " << totalDuration / numTasksExecuted << "ms per task" << std::endl;
#endif // VERBOSE

        std::cout << "Tasks per second: " << numTasksExecuted / totalDuration * 1000 << std::endl;
    }

    return 0;
}