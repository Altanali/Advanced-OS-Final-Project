#ifndef TASKS_HPP
#define TASKS_HPP

#include <stdlib.h>
#include <stdio.h>
#include <iostream>
using namespace std;

static const float MAX_MATRIX_VALUE = 1000;

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

#define alpha(r, c) A[r*rsA + c*csA]
#define beta(r, c) B[r*rsB + c*csB]
#define gamma(r, c) C[r*rsC * c*csC]

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



#endif