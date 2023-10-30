#include <ctime>
#include <thread>
#include <chrono>
#include <cstdlib>
#include <iostream>
#include <boost/asio.hpp>
#include <boost/asio/thread_pool.hpp>

using namespace boost::asio;

// primality check helper
bool isPrime(int n)
{
    if (n <= 1)
    {
        return false;
    }
    if (n <= 3)
    {
        return true;
    }

    if (n % 2 == 0 || n % 3 == 0)
    {
        return false;
    }

    for (int i = 5; i * i <= n; i += 6)
    {
        if (n % i == 0 || n % (i + 2) == 0)
        {
            return false;
        }
    }

    return true;
}

// task for each thread to perform
void task()
{
    // set the random number generator with the current time
    std::srand(static_cast<unsigned>(std::time(nullptr)));
    int upperLimit = 100; // upper limit for random numbers

    int randomNum = std::rand() % (upperLimit - 2) + 2;
#ifdef VERBOSE
    std::cout << "Random Number: " << randomNum << std::endl;
    isPrime(randomNum) ? std::cout << "Prime" << std::endl : std::cout << "Not Prime" << std::endl;
#endif // VERBOSE
}

int main()
{
    const int maxThreads = 16; // Maximum number of threads to test
    const int numTasks = 1000; // Number of tasks to execute
    const int numRepeats = 3;  // Number of times to repeat the test

    std::cout << "Scalability Benchmark Results:" << std::endl;

    for (int numThreads = 1; numThreads <= maxThreads; numThreads *= 2)
    {
        double totalDuration = 0.0;

        for (int repeat = 0; repeat <= numRepeats; ++repeat)
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
                  << aveThroughput << "tasks per second" << std::endl;
    }

    return 0;
}