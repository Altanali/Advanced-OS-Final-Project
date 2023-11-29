#ifndef TASKS_HPP
#define TASKS_HPP

#include <stdlib.h>
#include <stdio.h>
#include <iostream>
#include <fstream>
#include <sstream>
#include <sys/resource.h>
#include <chrono>
#include <thread>
#include <functional>
#include <semaphore>
using namespace std;

static const float MAX_MATRIX_VALUE = 1000;
static counting_semaphore rss_out_sema{1};
// Basic algorithm for determining if a number is prime or not. 
bool isPrime(long long n)
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



double *randomMatrix(int m, int n) {
    double *matrix = (double *)malloc(sizeof(double)*m*n);
    if(!matrix) {
        cout << "bad malloc\n";
        exit(1);
    }
    for(int i = 0; i < m; ++i) 
        for(int j = 0; j < n; ++j)
            matrix[i*m + j] = drand48()*MAX_MATRIX_VALUE;
    return matrix;

}


void GEMM(
    int m, int n, int k, 
    double *A, int rsA, int csA, 
    double *B, int rsB, int csB, 
    double *C, int rsC, int csC
) {
    int i, j, p;
    for(i = 0; i < m; ++i) {
        for(j = 0; j < n; ++j) {
            for(p = 0; p < k; ++p) {
                // gamma(i, j) += alpha(i, p)*beta(p, j);
                C[i*m + j] = A[i*n + k]*B[p*k + j];
            }
        }
    }
}


void small_gemm_task() {
    int m = 10;
    int n = 10;
    int k = 10;
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


long long get_curr_rss() {
    ifstream statFile("/proc/self/stat");
    string line;
    getline(statFile, line);
    istringstream iss(line);
    string entry;
    for(int i = 0; i < 24; ++i) {
        getline(iss, entry, ' ');
    }
    long long rss = stoi(entry);

    return rss;
}

long long get_curr_time_long() {
    auto curr_time = std::chrono::high_resolution_clock::now();
	auto curr_time_ms = std::chrono::time_point_cast<std::chrono::milliseconds>(curr_time).time_since_epoch();
    auto value = std::chrono::duration_cast<std::chrono::milliseconds>(curr_time_ms);
    return value.count();
}

void track_rss(string out_file, long long start_time = -1) {
    ofstream out(out_file);

    string out_message;
    long long time;
    long long rss;
    while(1) {
        if(rss_out_sema.try_acquire()) break;
        this_thread::sleep_for(chrono::milliseconds(5));
        long long duration = get_curr_time_long();
        if(start_time != -1) {
            time = duration - start_time;
        } else {
            time = duration;
        }
        rss = get_curr_rss();
        out_message = to_string(time) + "," + to_string(rss) + "\n";
        out.write(out_message.c_str(), out_message.size());
    }
    out.flush();
}
#endif