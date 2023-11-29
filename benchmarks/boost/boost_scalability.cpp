#include <ctime>
#include <thread>
#include <chrono>
#include <cstdlib>
#include <iostream>
#include <boost/asio.hpp>
#include <boost/asio/thread_pool.hpp>
#include "../tasks.hpp"

using namespace boost::asio;
using namespace std;

const int max_threads = 16;        // Number of threads in the pool
const int num_tasks = 1000;      // Total number of tasks to execute
const int numRepeats = 3;


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

    for (int num_threads = 1; num_threads <= max_threads; num_threads *= 2)
    {
        double total_duration = 0;
        int num_tasks_executed = 0;

        for (int repeat = 0; repeat < numRepeats; ++repeat)
        {
            thread_pool threadPool(num_threads);

            // Measure the performance of tasks
            auto start_time = std::chrono::high_resolution_clock::now();
            for (int i = 0; i < num_tasks; ++i)
            {
                post(threadPool, task);
            }

            threadPool.join();

			auto end_time = std::chrono::high_resolution_clock::now();
			auto elapsed_time = std::chrono::duration_cast<std::chrono::milliseconds>(end_time - start_time);
			total_duration += elapsed_time.count();
			num_tasks_executed += num_tasks;

        }

        cout << "Thread Count: " << num_threads << endl;
		cout << "Average time per task: " << total_duration/num_tasks_executed << " ms per task.\n";
		cout << "Tasks per second: " << num_tasks_executed/total_duration*1000 << endl;
		cout << endl;
	}

    return 0;
}